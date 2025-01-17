#pragma once

#include <hal/io.h>

#include "object.h"

namespace Hjert::Core {

struct Vmo : public BaseObject<Vmo, Hj::Type::VMO> {
    using _Mem = Var<Hal::PmmMem, Hal::DmaRange>;
    _Mem _mem;

    static Res<Strong<Vmo>> alloc(usize size, Hj::VmoFlags);

    static Res<Strong<Vmo>> makeDma(Hal::DmaRange prange);

    Vmo(_Mem mem) : _mem(std::move(mem)) {}

    Hal::PmmRange range();

    bool isDma() {
        return _mem.is<Hal::DmaRange>();
    }
};

} // namespace Hjert::Core
