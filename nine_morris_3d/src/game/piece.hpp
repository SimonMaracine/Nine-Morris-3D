#pragma once

#include <functional>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

enum class PieceType {
    White = 1,
    Black = 2
};

enum class PieceMovement {
    None,
    Direct,
    ThreeStep
};

class PieceObj {
public:
    PieceObj() = default;
    PieceObj(int id, glm::vec3 position, const sm::Renderable& renderable, PieceType type);

    const sm::Renderable& get_renderable() const { return m_renderable; }
    sm::Renderable& get_renderable() { return m_renderable; }
    int get_id() const { return m_id; }
    PieceType get_type() const { return m_type; }

    void update(sm::Ctx& ctx);
    void update_movement();

    void move_direct(glm::vec3 origin, glm::vec3 target, std::function<void()>&& on_finish);
    void move_three_step(glm::vec3 origin, glm::vec3 target0, glm::vec3 target1, glm::vec3 target, std::function<void()>&& on_finish);

    bool active {true};
    bool to_remove {false};
    int node_id {-1};
private:
    void direct_movement(glm::vec3 origin, glm::vec3 target, auto on_arrive);
    void direct_movement();
    void threestep_movement();
    void finish_movement();

    int m_id {-1};
    PieceType m_type {};

    PieceMovement m_movement {PieceMovement::None};
    glm::vec3 m_origin {};
    glm::vec3 m_target0 {};
    glm::vec3 m_target1 {};
    glm::vec3 m_target {};
    bool m_reached_target0 {false};
    bool m_reached_target1 {false};
    std::function<void()> m_on_finish;

    sm::Renderable m_renderable;
};
