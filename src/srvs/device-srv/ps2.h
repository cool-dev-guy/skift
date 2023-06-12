#pragma once

#include <hal/io.h>
#include "base.h"

namespace Dev::Ps2 {

struct I8042;

struct Device : public Node {
    I8042 &_ctrl;

    I8042 &ctrl() {
        return _ctrl;
    }

    Device(I8042 &ctrl)
        : _ctrl(ctrl) {}

    virtual ~Device() = default;
};

enum struct Status : u8 {
    OUT_BUF = 1 << 0,
    IN_BUF = 1 << 1,
    SYS = 1 << 2,
    CMD_DATA = 1 << 3,
    KEY_LOCK = 1 << 4,
    AUX_BUF = 1 << 5,
    TIMEOUT = 1 << 6,
    PARITY = 1 << 7,
};

FlagsEnum$(Status);

enum struct Cmd : u8 {
    READ_CONFIG = 0x20,
    WRITE_CONFIG = 0x60,
    DISABLE_AUX = 0xA7,
    ENABLE_AUX = 0xA8,
    TEST_AUX = 0xA9,
    TEST_CONTROLLER = 0xAA,
    TEST_MAIN = 0xAB,
    DISABLE_MAIN = 0xAD,
    ENABLE_MAIN = 0xAE,
};

enum struct Configs : u8 {
    FIRST_PORT_INTERRUPT_ENABLE = (1 << 0),
    SECOND_PORT_INTERRUPT_ENABLE = (1 << 1),
    FIRST_PORT_CLOCK_DISABLE = (1 << 4),
    SECOND_PORT_CLOCK_DISABLE = (1 << 5),
    FIRST_TRANSLATION = (1 << 6),
};

FlagsEnum$(Configs);

struct I8042 : public Node {
    Strong<Hal::Io> _io;

    using DataReg = Hal::Reg<u8, 0x0>;
    using StatusReg = Hal::Reg<u8, 0x4>;
    using CmdReg = Hal::Reg<u8, 0x4>;

    I8042(Strong<Hal::Io> io)
        : _io(io) {}

    Hal::Io &io() {
        return *_io;
    }

    Res<> init() override;

    Res<> flush();

    /* --- Data and Status --- */

    Res<Flags<Status>> readStatus();

    Res<> waitRead();

    Res<> waitWrite();

    Res<Byte> readData();

    Res<> writeData(Byte data);

    Res<> writeCmd(Cmd cmd);

    /* --- Configs --- */

    Res<> writeConfig(Flags<Configs> cfg);

    Res<Flags<Configs>> readConfig();
};

struct Keyboard : public Device {
    bool _esc;

    using Device::Device;

    Res<> init() override;

    Res<> event(Events::Event &e) override;
};

struct Mouse : public Device {
    u8 _cycle;
    Array<u8, 4> _buf;
    bool _hasWheel;

    using Device::Device;

    Res<> init() override;

    Res<> event(Events::Event &e) override;
};

} // namespace Dev::Ps2