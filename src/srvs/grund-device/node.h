#pragma once

#include <karm-async/async.h>
#include <karm-base/rc.h>
#include <karm-base/vec.h>

namespace Grund::Device {

struct IrqEvent {
    usize irq;
};

struct Node : public Meta::Static {
    usize _id = 0;
    Node *_parent = nullptr;
    Vec<Strong<Node>> _children = {};

    Node();

    virtual ~Node();

    virtual Res<> init();

    virtual Res<> event(Async::Event &e);

    virtual Res<> bubble(Async::Event &e);

    Res<> attach(Strong<Node> child);

    void detach(Strong<Node> child);

    auto operator<=>(Node const &o) const {
        return _id <=> o._id;
    }

    auto operator==(Node const &o) const {
        return _id == o._id;
    }
};

} // namespace Grund::Device
