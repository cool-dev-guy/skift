#include <karm-logger/logger.h>

#include "arch.h"
#include "sched.h"
#include "space.h"
#include "task.h"

namespace Hjert::Core {

Res<Strong<Task>> Task::create(
    Mode mode,
    Opt<Strong<Space>> space,
    Opt<Strong<Domain>> domain) {

    logInfo("task: creating task...");
    auto stack = try$(Stack::create());
    auto task = makeStrong<Task>(mode, std::move(stack), space, domain);
    return Ok(task);
}

Task &Task::self() {
    return *Sched::instance()._curr;
}

Task::Task(Mode mode,
           Stack stack,
           Opt<Strong<Space>> space,
           Opt<Strong<Domain>> domain)
    : _mode(mode),
      _stack(std::move(stack)),
      _space(space),
      _domain(domain) {
}

Res<> Task::ensure(Hj::Pledge pledge) {
    ObjectLockScope scope(*this);

    if (not _pledges.has(pledge)) {
        return Error::permissionDenied("task does not have pledge");
    }
    return Ok();
}

Res<> Task::pledge(Hj::Pledge pledge) {
    ObjectLockScope scope(*this);
    if (not _pledges.has(pledge))
        return Error::permissionDenied("task does not have pledge");
    _pledges = pledge;
    return Ok();
}

Res<> Task::ready(usize ip, usize sp, Hj::Args args) {
    logInfo("{} readying for execustion (ip: {x}, sp: {x}) starting...", *this, ip, sp);
    ObjectLockScope scope(*this);
    _ctx = try$(Arch::createCtx(_mode, ip, sp, stack().loadSp(), args));
    return Ok();
}

Res<> Task::block(Blocker blocker) {
    // NOTE: Can't use ObjectLockScope here because we need to yield
    //       outside of the lock.
    _lock.acquire();
    _block = std::move(blocker);
    _lock.release();
    Arch::yield();
    return Ok();
}

void Task::crash() {
    logError("{}: crashed", *this);
    signal(Hj::Sigs::EXITED | Hj::Sigs::CRASHED,
           Hj::Sigs::NONE);
}

State Task::eval(TimeStamp now) {
    ObjectLockScope scope(*this);

    if (_ret())
        return State::EXITED;

    if (_block) {
        if ((*_block)() == now) {
            return State::BLOCKED;
        }
        _block = NONE;
    }

    return State::RUNNABLE;
}

void Task::save(Arch::Frame const &frame) {
    (*_ctx)->save(frame);
}

void Task::load(Arch::Frame &frame) {
    if (_space)
        (*_space)->activate();

    (*_ctx)->load(frame);
}

void Task::enter(Mode mode) {
    ObjectLockScope scope(*this);
    _mode = mode;
}

void Task::leave() {
    // NOTE: Can't use ObjectLockScope here because we need to yield
    //       outside of the lock.
    _lock.acquire();
    _mode = Mode::USER;
    bool yield = _ret();
    _lock.release();

    if (yield)
        Arch::yield();
}

} // namespace Hjert::Core
