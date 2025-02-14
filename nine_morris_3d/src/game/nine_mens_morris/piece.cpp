#include "game/nine_mens_morris/piece.hpp"

#include <nine_morris_3d_engine/external/resmanager.h++>

static constexpr float PIECE_BASE_VELOCITY {0.28f};
static constexpr float PIECE_MIN_VELOCITY_STRENGTH {0.01f};
static constexpr float PIECE_MAX_VELOCITY_STRENGTH {1.0f};
static constexpr float PIECE_DISTANCE_ERROR {0.001f};

PieceObj::PieceObj(int id, std::shared_ptr<sm::OutlinedModelNode> model, glm::vec3 position, PieceType type)
    : HoverableObj(id, model), m_type(type) {
    m_model->position = position;
}

void PieceObj::update(sm::Ctx& ctx) {
    if (active) {
        const auto board_model {ctx.render_3d()->find_node("board"_H)};
        board_model->add_node(m_model);
    }
}

void PieceObj::update_movement() {
    switch (m_movement) {
        case PieceMovement::None:
            break;
        case PieceMovement::Direct:
            direct_movement();
            break;
        case PieceMovement::ThreeStep:
            threestep_movement();
            break;
    }
}

void PieceObj::move_direct(glm::vec3 origin, glm::vec3 target, OnFinish&& on_finish) {
    m_movement = PieceMovement::Direct;
    m_origin = origin;
    m_target = target;
    m_on_finish = std::move(on_finish);
}

void PieceObj::move_three_step(glm::vec3 origin, glm::vec3 target0, glm::vec3 target1, glm::vec3 target, OnFinish&& on_finish) {
    m_movement = PieceMovement::ThreeStep;
    m_origin = origin;
    m_target0 = target0;
    m_target1 = target1;
    m_target = target;
    m_on_finish = std::move(on_finish);
}

void PieceObj::direct_movement(glm::vec3 origin, glm::vec3 target, auto on_arrive) {
    const float initial_distance {glm::distance(origin, target)};
    const float current_distance {glm::distance(m_model->position, target)};
    const glm::vec3 difference {target - m_model->position};

    const float velocity_strength {
        glm::clamp(
            initial_distance * PIECE_BASE_VELOCITY * sm::utils::map(current_distance, 0.0f, initial_distance, 0.0f, 1.0f),
            PIECE_MIN_VELOCITY_STRENGTH,
            PIECE_MAX_VELOCITY_STRENGTH
        )
    };

    const glm::vec3 velocity {glm::normalize(difference) * velocity_strength};

    m_model->position += velocity;

    if (glm::distance(m_model->position, target) < PIECE_MIN_VELOCITY_STRENGTH + PIECE_DISTANCE_ERROR) {
        on_arrive();
    }
}

void PieceObj::direct_movement() {
    direct_movement(m_origin, m_target, [this]() { finish_movement(); });
}

void PieceObj::threestep_movement() {
    if (!m_reached_target0) {
        direct_movement(m_origin, m_target0, [this]() {
            m_reached_target0 = true;
        });
    } else if (!m_reached_target1) {
        direct_movement(m_target0, m_target1, [this]() {
            m_reached_target1 = true;
        });
    } else {
        direct_movement(m_target1, m_target, [this]() {
            finish_movement();
        });
    }
}

void PieceObj::finish_movement() {
    m_model->position = m_target;
    m_on_finish(*this);

    m_movement = PieceMovement::None;
    m_origin = {};
    m_target0 = {};
    m_target1 = {};
    m_target = {};
    m_reached_target0 = false;
    m_reached_target1 = false;
    m_on_finish = {};
}
