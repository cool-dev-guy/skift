#include <elf/image.h>
#include <handover/main.h>
#include <karm-base/size.h>
#include <karm-logger/logger.h>
#include <karm-sys/chan.h>

#include "arch.h"
#include "cpu.h"
#include "domain.h"
#include "mem.h"
#include "sched.h"
#include "space.h"
#include "task.h"

namespace Hjert::Core {

Res<> validateAndDump(u64 magic, Handover::Payload &payload) {
    if (not Handover::valid(magic, payload)) {
        logInfo("entry: handover: invalid");
        return Error::invalidInput("Invalid handover payload");
    }

    logInfo("entry: handover: valid");
    logInfo("entry: handover: agent: '{}'", payload.agentName());

    usize totalFree = 0;
    logInfo("entry: dumping handover records...");
    for (auto const &record : payload) {
        logInfo(
            " - {} {x}-{x} ({}kib)",
            record.name(),
            record.start,
            record.end(),
            record.size / 1024);

        if (record.tag == Handover::FREE) {
            totalFree += record.size;
        }

        if (record.tag == Handover::FILE) {
            auto const *name = payload.stringAt(record.file.name);
            logInfo("   - file: '{}'", name);
        }
    }

    logInfo("entry: handover: total free: {}mib", toMib(totalFree));

    return Ok();
}

Res<> enterUserspace(Handover::Payload &payload) {
    auto const *record = payload.fileByName("bundle://grund-system/_bin");
    if (not record) {
        logInfo("entry: handover: no init file");
        return Error::invalidInput("No init file");
    }

    auto space = try$(Space::create());
    space->label("init-space");

    logInfo("entry: mapping elf...");
    auto elfVmo = try$(Vmo::makeDma(record->range<Hal::DmaRange>()));
    elfVmo->label("elf-shared");
    auto elfRange = try$(kmm().pmm2Kmm(elfVmo->range()));
    Elf::Image image{elfRange.bytes()};

    if (not image.valid()) {
        logInfo("entry: invalid elf");
        return Error::invalidInput("Invalid elf");
    }

    for (auto prog : image.programs()) {
        if (prog.type() != Elf::Program::LOAD) {
            continue;
        }

        usize size = alignUp(max(prog.memsz(), prog.filez()), Hal::PAGE_SIZE);

        if ((prog.flags() & Elf::ProgramFlags::WRITE) == Elf::ProgramFlags::WRITE) {
            auto sectionVmo = try$(Vmo::alloc(size, Hj::VmoFlags::UPPER));
            sectionVmo->label("elf-writeable");
            auto sectionRange = try$(kmm().pmm2Kmm(sectionVmo->range()));
            logInfo("entry: mapping section: {x}-{x}", sectionRange.start, sectionRange.end());
            copy(prog.bytes(), sectionRange.mutBytes());
            try$(space->map({prog.vaddr(), size}, sectionVmo, 0, Hj::MapFlags::READ | Hj::MapFlags::WRITE));
        } else {
            try$(space->map({prog.vaddr(), size}, elfVmo, prog.offset(), Hj::MapFlags::READ | Hj::MapFlags::EXEC));
        }
    }

    logInfo("entry: mapping handover...");
    auto handoverBase = ((usize)&payload) - Handover::KERNEL_BASE;
    auto handoverSize = payload.size;
    auto handoverVmo = try$(Vmo::makeDma({handoverBase, handoverSize}));
    handoverVmo->label("handover");
    auto handoverRange = try$(space->map({}, handoverVmo, 0, Hj::MapFlags::READ));

    logInfo("entry: mapping stack...");
    auto stackVmo = try$(Vmo::alloc(kib(64), Hj::VmoFlags::UPPER));
    stackVmo->label("stack");
    auto stackRange = try$(space->map({}, stackVmo, 0, Hj::MapFlags::READ | Hj::MapFlags::WRITE));
    logInfo("entry: stack: {x}-{x}", stackRange.start, stackRange.end());

    logInfo("entry: creating task...");
    auto domain = try$(Domain::create());
    domain->label("init-domain");
    auto task = try$(Task::create(Mode::USER, space, domain));
    task->label("init-task");

    try$(task->ready(image.header().entry, stackRange.end(), {handoverRange.start}));
    try$(Sched::instance().enqueue(task));

    return Ok();
}

Res<> init(u64 magic, Handover::Payload &payload) {
    try$(Arch::init(payload));
    try$(validateAndDump(magic, payload));
    try$(Mem::init(payload));
    try$(Sched::init(payload));

    logInfo("entry: everything is ready, enabling interrupts...");
    Arch::cpu().retainEnable();
    Arch::cpu().enableInterrupts();

    logInfo("entry: entering userspace...");
    try$(enterUserspace(payload));

    logInfo("entry: entering idle loop...");
    Task::self().label("idle");
    Task::self().enter(Mode::IDLE);
    while (true)
        Arch::cpu().relaxe();
}

} // namespace Hjert::Core

/* --- Handover Entry Point ------ ------------------------------------------ */

HandoverRequests$(
    Handover::requestStack(),
    Handover::requestFb(),
    Handover::requestFiles());

Res<> entryPoint(u64 magic, Handover::Payload &payload) {
    return Hjert::Core::init(magic, payload);
}
