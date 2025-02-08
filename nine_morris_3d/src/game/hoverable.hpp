#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

// Generic visual object that can be hovered with the mouse pointer
class HoverableObj {
public:
    HoverableObj() = default;
    HoverableObj(int id, std::shared_ptr<sm::ModelNode> model)
        : m_id(id), m_model(model) {}

    virtual ~HoverableObj() = default;

    HoverableObj(const HoverableObj&) = default;
    HoverableObj& operator=(const HoverableObj&) = default;
    HoverableObj(HoverableObj&&) = default;
    HoverableObj& operator=(HoverableObj&&) = default;

    int get_id() const { return m_id; }
    std::shared_ptr<sm::ModelNode> get_model() const { return m_model; }
protected:
    int m_id {-1};
    std::shared_ptr<sm::ModelNode> m_model;
};
