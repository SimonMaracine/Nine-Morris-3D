#include "game/board.hpp"

#include <algorithm>

#include <nine_morris_3d_engine/external/glm.h++>

#include "game/ray.hpp"

static constexpr float PIECE_Y_POSITION_AIR_MOVE {0.75f};
static constexpr float PIECE_Y_POSITION_AIR_TAKE {2.0f};

static glm::mat4 transformation_matrix(glm::vec3 position, glm::vec3 rotation, float scale) {
    glm::mat4 matrix {1.0f};
    matrix = glm::translate(matrix, position);
    matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::scale(matrix, glm::vec3(scale));

    return matrix;
}

BoardObj::BoardObj(const sm::Renderable& board, const sm::Renderable& board_paint) {
    m_renderable = board;
    m_renderable.transform.scale = 20.0f;

    set_board_paint_renderable(board_paint);
}

void BoardObj::set_board_paint_renderable(const sm::Renderable& board_paint) {
    m_paint_renderable = board_paint;
    m_paint_renderable.transform.scale = 20.0f;
    m_paint_renderable.transform.position.y = 0.062f;
}

void BoardObj::user_click_press(GameOver game_over) {
    if (game_over != GameOver::None) {
        return;
    }

    m_clicked_id = m_hovered_id;
}

void BoardObj::user_click_release(GameOver game_over, std::function<void()>&& callback) {
    if (game_over != GameOver::None) {
        m_clicked_id = -1;
        return;
    }

    if (m_hovered_id == -1 || m_hovered_id != m_clicked_id) {
        m_clicked_id = -1;
        return;
    }

    m_clicked_id = -1;

    callback();
}

void BoardObj::update_hovered_id(glm::vec3 ray, glm::vec3 camera, std::function<std::vector<std::pair<int, sm::Renderable>>()>&& get_renderables) {
    if (camera.y < 0.0f) {
        m_hovered_id = -1;
        return;
    }

    auto renderables {get_renderables()};

    std::sort(renderables.begin(), renderables.end(), [camera](const auto& lhs, const auto& rhs) {
        const auto left {glm::distance(lhs.second.transform.position, camera)};
        const auto right {glm::distance(rhs.second.transform.position, camera)};

        return left > right;
    });

    bool hover {false};

    for (const auto& [id, renderable] : renderables) {
        const auto& transform {renderable.transform};
        const glm::mat4 to_world_space {transformation_matrix(transform.position, transform.rotation, transform.scale)};

        sm::utils::AABB aabb;
        aabb.min = to_world_space * glm::vec4(renderable.get_aabb().min, 1.0f);
        aabb.max = to_world_space * glm::vec4(renderable.get_aabb().max, 1.0f);

        if (ray_aabb_collision(ray, camera, aabb)) {
            m_hovered_id = id;
            hover = true;
        }
    }

    if (!hover) {
        m_hovered_id = -1;
    }
}

const char* BoardObj::turn_string(Player turn) {
    switch (turn) {
        case Player::White:
            return "White";
        case Player::Black:
            return "Black";
    }

    return {};
}

const char* BoardObj::game_over_string(GameOver game_over) {
    switch (game_over) {
        case GameOver::None:
            return "None";
        case GameOver::WinnerWhite:
            return "WinnerWhite";
        case GameOver::WinnerBlack:
            return "WinnerBlack";
        case GameOver::TieBetweenBothPlayers:
            return "TieBetweenBothPlayers";
    }

    return {};
}

Player BoardObj::opponent(Player player) {
    if (player == Player::White) {
        return Player::Black;
    } else {
        return Player::White;
    }
}

void BoardObj::do_place_animation(PieceObj& piece, const NodeObj& node, std::function<void()>&& on_finish) {
    const glm::vec3 origin {piece.get_renderable().transform.position};
    const glm::vec3 target0 {piece.get_renderable().transform.position.x, PIECE_Y_POSITION_AIR_MOVE, piece.get_renderable().transform.position.z};
    const glm::vec3 target1 {node.get_renderable().transform.position.x, PIECE_Y_POSITION_AIR_MOVE, node.get_renderable().transform.position.z};
    const glm::vec3 target {node.get_renderable().transform.position.x, PIECE_Y_POSITION_BOARD, node.get_renderable().transform.position.z};

    piece.move_three_step(origin, target0, target1, target, std::move(on_finish));
}

void BoardObj::do_move_animation(PieceObj& piece, const NodeObj& node, std::function<void()>&& on_finish, bool direct) {
    if (direct) {
        const glm::vec3 origin {piece.get_renderable().transform.position};
        const glm::vec3 target {node.get_renderable().transform.position.x, PIECE_Y_POSITION_BOARD, node.get_renderable().transform.position.z};

        piece.move_direct(origin, target, std::move(on_finish));
    } else {
        const glm::vec3 origin {piece.get_renderable().transform.position};
        const glm::vec3 target0 {piece.get_renderable().transform.position.x, PIECE_Y_POSITION_AIR_MOVE, piece.get_renderable().transform.position.z};
        const glm::vec3 target1 {node.get_renderable().transform.position.x, PIECE_Y_POSITION_AIR_MOVE, node.get_renderable().transform.position.z};
        const glm::vec3 target {node.get_renderable().transform.position.x, PIECE_Y_POSITION_BOARD, node.get_renderable().transform.position.z};

        piece.move_three_step(origin, target0, target1, target, std::move(on_finish));
    }
}

void BoardObj::do_take_animation(PieceObj& piece, std::function<void()>&& on_finish) {
    const glm::vec3 origin {piece.get_renderable().transform.position};
    const glm::vec3 target {piece.get_renderable().transform.position.x, PIECE_Y_POSITION_AIR_TAKE, piece.get_renderable().transform.position.z};

    piece.move_direct(origin, target, std::move(on_finish));
}
