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
    PieceObj(int id, const sm::Renderable& renderable, glm::vec3 position, PieceType type);

    PieceType get_type() const { return m_type; }

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
};
