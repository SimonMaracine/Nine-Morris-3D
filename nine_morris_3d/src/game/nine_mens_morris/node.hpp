#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/hoverable.hpp"

class NodeObj : public HoverableObj {
public:
    NodeObj() = default;
    NodeObj(int id, glm::vec3 position, std::shared_ptr<sm::ModelNode> model);

    glm::vec3 get_position() const override { return m_model->position; }
    glm::vec3 get_rotation() const override { return m_model->rotation; }
    float get_scale() const override { return m_model->scale; }
    const sm::utils::AABB& get_aabb() const override { return m_model->get_aabb(); }

    std::shared_ptr<sm::ModelNode> get_model() const { return m_model; }
    void set_highlighted(bool highlighted) { m_highlighted = highlighted; }

    void update(sm::Ctx& ctx);

    int piece_id {-1};
private:
    bool m_highlighted {false};
    std::shared_ptr<sm::ModelNode> m_model;
};
