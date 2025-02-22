#pragma once

#include <functional>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/hoverable.hpp"

enum class PieceType {
    White = 1,
    Black = 2
};

enum class PieceMovement {
    None,
    Direct,
    ThreeStep
};

class PieceObj : public HoverableObj {
public:
    using OnFinish = std::function<void(PieceObj&)>;

    PieceObj() = default;
    PieceObj(int id, glm::vec3 position, PieceType type, std::shared_ptr<sm::ModelNode> model);

    glm::vec3 get_position() const override { return m_model->position; }
    glm::vec3 get_rotation() const override { return m_model->rotation; }
    float get_scale() const override { return m_model->scale; }
    const sm::utils::AABB& get_aabb() const override { return m_model->get_aabb(); }

    PieceType get_type() const { return m_type; }
    bool is_moving() const { return m_movement != PieceMovement::None; }
    std::shared_ptr<sm::ModelNode> get_model() const { return m_model; }

    void update(sm::Ctx& ctx);
    void update_movement();

    void move_direct(glm::vec3 origin, glm::vec3 target, OnFinish&& on_finish);
    void move_three_step(glm::vec3 origin, glm::vec3 target0, glm::vec3 target1, glm::vec3 target, OnFinish&& on_finish);

    bool active {true};
    bool to_remove {false};
    int node_id {-1};
private:
    void direct_movement(glm::vec3 origin, glm::vec3 target, auto on_arrive);
    void direct_movement();
    void threestep_movement();
    void finish_movement();

    PieceType m_type {};

    PieceMovement m_movement {PieceMovement::None};
    glm::vec3 m_origin {};
    glm::vec3 m_target0 {};
    glm::vec3 m_target1 {};
    glm::vec3 m_target {};
    bool m_reached_target0 {false};
    bool m_reached_target1 {false};
    OnFinish m_on_finish;

    std::shared_ptr<sm::ModelNode> m_model;
};
