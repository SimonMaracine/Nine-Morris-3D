#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

// Generic visual object that can be hovered with the mouse pointer
class HoverableObj {
public:
    HoverableObj() = default;
    HoverableObj(int id, const sm::Renderable& renderable)
        : m_id(id), m_renderable(renderable) {}

    virtual ~HoverableObj() = default;

    HoverableObj(const HoverableObj&) = default;
    HoverableObj& operator=(const HoverableObj&) = default;
    HoverableObj(HoverableObj&&) = default;
    HoverableObj& operator=(HoverableObj&&) = default;

    int get_id() const { return m_id; }
    const sm::Renderable& get_renderable() const { return m_renderable; }
    sm::Renderable& get_renderable() { return m_renderable; }
protected:
    int m_id {-1};
    sm::Renderable m_renderable;
};
