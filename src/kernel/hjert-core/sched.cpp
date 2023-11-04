#include <karm-logger/logger.h>

#include "arch.h"
#include "mem.h"
#include "sched.h"
#include "space.h"
#include "task.h"

namespace Hjert::Core {

static Opt<Sched> _sched;

Res<> Sched::init(Handover::Payload &) {
    logInfo("sched: initializing...");
    auto bootTask = try$(Task::create(Mode::SUPER, try$(Space::create())));
    bootTask->label("entry");
    try$(bootTask->ready(0, 0, {}));
    _sched.emplace(std::move(bootTask));
    return Ok();
}

Sched &Sched::instance() {
    return *_sched;
}

Sched::Sched(Strong<Task> boot)
    : _tasks{boot},
      _prev(boot),
      _curr(boot),
      _idle(boot) {
}

Res<> Sched::enqueue(Strong<Task> task) {
    LockScope scope(_lock);
    _tasks.pushBack(std::move(task));
    return Ok();
}

void Sched::schedule(TimeSpan span) {
    LockScope scope(_lock);

    _stamp += span;
    _prev = _curr;
    _curr->_sliceEnd = _stamp;
    auto next = _idle;
    // HACK: to make sure the idle task is always scheduled last
    _idle->_sliceEnd = _stamp + 1;

    for (usize i = 0; i < _tasks.len(); ++i) {
        auto &t = _tasks[i];
        auto state = t->eval(_stamp);
        if (state == State::EXITED) {
            logInfo("{}: exited", *t);
            _tasks.removeAt(i--);
        } else if (state == State::RUNNABLE and
                   t->_sliceEnd <= next->_sliceEnd) {
            next = t;
        }
    }

    _curr = next;
}

} // namespace Hjert::Core
