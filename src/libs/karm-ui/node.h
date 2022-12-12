#pragma once

#include <karm-base/func.h>
#include <karm-base/rc.h>
#include <karm-debug/logger.h>
#include <karm-events/events.h>
#include <karm-gfx/context.h>
#include <karm-layout/size.h>

namespace Karm::Ui {

inline constexpr bool debugShowLayoutBounds = false;
inline constexpr bool debugShowRepaintBounds = false;
inline constexpr bool debugShowEmptyBounds = false;
inline constexpr bool debugShowScrollBounds = false;

struct Node;

using Child = Strong<Node>;
using Other = Strong<Node>;
using Children = Vec<Child>;
using Visitor = Func<void(Node &)>;

/* --- Node ----------------------------------------------------------------- */

struct Node {
    virtual ~Node() = default;

    virtual Opt<Child> reconcile(Other o) { return o; }

    virtual void paint(Gfx::Context &, Math::Recti) {}

    virtual void event(Events::Event &) {}

    virtual void bubble(Events::Event &) {}

    virtual void layout(Math::Recti) {}

    virtual Math::Vec2i size(Math::Vec2i s, Layout::Hint) { return s; }

    virtual Math::Recti bound() { return {}; }

    virtual void visit(Visitor &) {}

    virtual Node *parent() { return nullptr; }

    virtual void attach(Node *) {}

    virtual void detach(Node *) {}

    virtual void *query(Meta::Id) { return nullptr; }
};

/* --- LeafNode ------------------------------------------------------------- */

template <typename Crtp>
struct LeafNode : public Node {
    Node *_parent = nullptr;

    virtual void reconcile(Crtp &) {}

    Opt<Child> reconcile(Other o) override {
        if (not o.is<Crtp>()) {
            return o;
        }
        reconcile(o.unwrap<Crtp>());
        return NONE;
    }

    void bubble(Events::Event &e) override {
        if (_parent and not e.accepted)
            _parent->bubble(e);
    }

    Node *parent() override {
        return _parent;
    }

    void attach(Node *parent) override {
        _parent = parent;
    }

    void detach(Node *parent) override {
        if (_parent == parent)
            _parent = nullptr;
    }

    void *query(Meta::Id id) override {
        if (id == Meta::makeId<Crtp>()) {
            return static_cast<Crtp *>(this);
        }

        return _parent ? _parent->query(id) : nullptr;
    }
};

/* --- GroupNode ------------------------------------------------------------ */

template <typename Crtp>
struct GroupNode : public LeafNode<Crtp> {
    Children _children;
    Math::Recti _bound{};

    GroupNode() = default;

    GroupNode(Children children) : _children(children) {
        for (auto &c : _children) {
            c->attach(this);
        }
    }

    ~GroupNode() {
        for (auto &c : _children) {
            c->detach(this);
        }
    }

    Children &children() {
        return _children;
    }

    Children const &children() const {
        return _children;
    }

    void reconcile(Crtp &o) override {
        auto &us = children();
        auto &them = o.children();

        for (size_t i = 0; i < them.len(); i++) {
            if (i < us.len()) {
                us.replace(i, tryOr(us[i]->reconcile(them[i]), us[i]));
            } else {
                us.insert(i, them[i]);
            }
            us[i]->attach(this);
        }

        us.truncate(them.len());
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        for (auto &child : children()) {
            if (not child->bound().colide(r))
                continue;

            child->paint(g, r);
        }
    }

    void event(Events::Event &e) override {
        if (e.accepted)
            return;

        for (auto &child : children()) {
            child->event(e);
            if (e.accepted)
                return;
        }
    }

    void layout(Math::Recti r) override {
        _bound = r;

        for (auto &child : children()) {
            child->layout(r);
        }
    }

    Math::Recti bound() override {
        return _bound;
    }

    void visit(Visitor &v) override {
        for (auto &child : children()) {
            v(*child);
        }
    }
};

/* --- ProxyNode ------------------------------------------------------------ */

template <typename Crtp>
struct ProxyNode : public LeafNode<Crtp> {
    Child _child;

    ProxyNode(Child child) : _child(child) {
        _child->attach(this);
    }

    ~ProxyNode() {
        _child->detach(this);
    }

    Node &child() {
        return *_child;
    }

    Node const &child() const {
        return *_child;
    }

    void reconcile(Crtp &o) override {
        _child = tryOr(_child->reconcile(o._child), _child);
        _child->attach(this);
        LeafNode<Crtp>::reconcile(o);
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        child().paint(g, r);
    }

    void event(Events::Event &e) override {
        if (e.accepted)
            return;

        child().event(e);
    }

    void layout(Math::Recti r) override {
        child().layout(r);
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        return child().size(s, hint);
    }

    Math::Recti bound() override {
        return _child->bound();
    }

    void visit(Visitor &v) override {
        v(*_child);
    }
};

} // namespace Karm::Ui
