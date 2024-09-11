#include "game/board.hpp"

#include <algorithm>
#include <cstdarg>
#include <cstdio>

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

BoardObj::BoardObj(const sm::Renderable& board, const sm::Renderable& board_paint)
    : m_renderable(board), m_paint_renderable(board_paint) {
    m_renderable.transform.scale = 20.0f;

    m_paint_renderable.transform.scale = 20.0f;
    m_paint_renderable.transform.position.y = 0.062f;
}

void BoardObj::set_board_paint_renderable(const sm::Renderable& renderable) {
    m_paint_renderable.override_renderable_private(renderable);
}

void BoardObj::set_renderables(
    const sm::Renderable& board,
    const sm::Renderable& board_paint,
    const std::vector<sm::Renderable>& nodes,
    const std::vector<sm::Renderable>& white_pieces,
    const std::vector<sm::Renderable>& black_pieces
) {
    m_renderable.override_renderable_private(board);
    set_board_paint_renderable(board_paint);

    for (int i {0}; i < node_count(); i++) {
        m_nodes[i].set_renderable(nodes[i]);
    }

    for (int i {0}; i < piece_count() / 2; i++) {
        m_pieces[i].set_renderable(white_pieces[i]);
    }

    for (int i {piece_count() / 2}; i < piece_count(); i++) {
        m_pieces[i].set_renderable(black_pieces[i - piece_count() / 2]);
    }
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

std::string BoardObj::format(const char* format, ...) {
    std::string buffer;
    buffer.resize(128);

    va_list args;
    va_start(args, format);

    std::vsnprintf(buffer.data(), 128, format, args);

    va_end(args);

    return buffer;
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

void BoardObj::initialize_nodes(const std::vector<sm::Renderable>& renderables) {
    for (int i {0}; i < static_cast<int>(m_nodes.size()); i++) {
        m_nodes[i] = NodeObj(i, NODE_POSITIONS[i], renderables[i]);
    }
}

void BoardObj::initialize_piece_in_air(
    const std::vector<sm::Renderable>& renderables,
    int index,
    int renderable_index,
    float x,
    float y,
    PieceType piece
) {
    // Offset pieces' IDs, so that they are different from nodes' IDs

    m_pieces[index] = PieceObj(
        index + static_cast<int>(m_nodes.size()),
        glm::vec3(x, PIECE_Y_POSITION_AIR_INITIAL, y),
        renderables[renderable_index],
        piece
    );
}

void BoardObj::update_movement() {
    for (PieceObj& piece : m_pieces) {
        piece.update_movement();
    }
}

void BoardObj::update_nodes_highlight(GameOver game_over, std::function<bool()>&& highlight) {
    if (game_over != GameOver::None) {
        std::for_each(m_nodes.begin(), m_nodes.end(), [](NodeObj& node) {
            node.set_highlighted(false);
        });

        return;
    }

    if (!highlight()) {
        std::for_each(m_nodes.begin(), m_nodes.end(), [](NodeObj& node) {
            node.set_highlighted(false);
        });

        return;
    }

    for (NodeObj& node : m_nodes) {
        node.set_highlighted(node.get_id() == m_hovered_id);
    }
}

void BoardObj::update_pieces_highlight(
    GameOver game_over,
    int user_selected_index,
    std::function<bool(const PieceObj&)>&& highlight
) {
    if (game_over != GameOver::None) {
        std::for_each(m_pieces.begin(), m_pieces.end(), [](PieceObj& piece) {
            piece.get_renderable().get_material()->flags &= ~sm::Material::Outline;
        });

        return;
    }

    for (PieceObj& piece : m_pieces) {
        if (piece.get_id() == m_hovered_id && highlight(piece)) {
            piece.get_renderable().get_material()->flags |= sm::Material::Outline;
            piece.get_renderable().outline.color = ORANGE;
        } else {
            piece.get_renderable().get_material()->flags &= ~sm::Material::Outline;
        }
    }

    // Override, if the piece is actually selected
    if (user_selected_index != -1) {
        const int piece_id {m_nodes[user_selected_index].piece_id};

        if (piece_id != -1) {
            m_pieces[piece_id - m_nodes.size()].get_renderable().get_material()->flags |= sm::Material::Outline;
            m_pieces[piece_id - m_nodes.size()].get_renderable().outline.color = RED;
        }
    }
}

void BoardObj::update_nodes(sm::Ctx& ctx) {
    for (NodeObj& node : m_nodes) {
        node.update(ctx);
    }
}

void BoardObj::update_pieces(sm::Ctx& ctx) {
    for (PieceObj& piece : m_pieces) {
        piece.update(ctx);
    }
}
