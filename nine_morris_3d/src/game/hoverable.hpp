#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

// Generic object that can be hovered with the mouse pointer
class HoverableObj {
public:
    HoverableObj() = default;
    explicit HoverableObj(int id)
        : m_id(id) {}

    virtual ~HoverableObj() = default;

    HoverableObj(const HoverableObj&) = default;
    HoverableObj& operator=(const HoverableObj&) = default;
    HoverableObj(HoverableObj&&) = default;
    HoverableObj& operator=(HoverableObj&&) = default;

    virtual glm::vec3 get_position() const = 0;
    virtual glm::vec3 get_rotation() const = 0;
    virtual float get_scale() const = 0;
    virtual const sm::utils::AABB& get_aabb() const = 0;

    int get_id() const { return m_id; }
protected:
    int m_id {-1};
};
