#include "game/standard_board.hpp"

#include <algorithm>
#include <utility>
#include <cassert>

#include <nine_morris_3d_engine/external/glm.h++>
#include <nine_morris_3d_engine/external/imgui.h++>

#include "game/ray.hpp"

#define PIECE(index) (index - 24)

static constexpr float NODE_Y_POSITION {0.063f};
static const glm::vec3 NODE_POSITIONS[24] {
    glm::vec3(2.046f, NODE_Y_POSITION, 2.062f),    // 0
    glm::vec3(-0.008f, NODE_Y_POSITION, 2.089f),   // 1
    glm::vec3(-2.101f, NODE_Y_POSITION, 2.076f),   // 2
    glm::vec3(1.480f, NODE_Y_POSITION, 1.512f),    // 3
    glm::vec3(0.001f, NODE_Y_POSITION, 1.513f),    // 4
    glm::vec3(-1.509f, NODE_Y_POSITION, 1.502f),   // 5
    glm::vec3(0.889f, NODE_Y_POSITION, 0.898f),    // 6
    glm::vec3(0.001f, NODE_Y_POSITION, 0.906f),    // 7
    glm::vec3(-0.930f, NODE_Y_POSITION, 0.892f),   // 8
    glm::vec3(2.058f, NODE_Y_POSITION, 0.031f),    // 9
    glm::vec3(1.481f, NODE_Y_POSITION, 0.025f),    // 10
    glm::vec3(0.894f, NODE_Y_POSITION, 0.026f),    // 11
    glm::vec3(-0.934f, NODE_Y_POSITION, 0.050f),   // 12
    glm::vec3(-1.508f, NODE_Y_POSITION, 0.050f),   // 13
    glm::vec3(-2.083f, NODE_Y_POSITION, 0.047f),   // 14
    glm::vec3(0.882f, NODE_Y_POSITION, -0.894f),   // 15
    glm::vec3(0.011f, NODE_Y_POSITION, -0.900f),   // 16
    glm::vec3(-0.930f, NODE_Y_POSITION, -0.890f),  // 17
    glm::vec3(1.477f, NODE_Y_POSITION, -1.455f),   // 18
    glm::vec3(0.006f, NODE_Y_POSITION, -1.463f),   // 19
    glm::vec3(-1.493f, NODE_Y_POSITION, -1.458f),  // 20
    glm::vec3(2.063f, NODE_Y_POSITION, -2.046f),   // 21
    glm::vec3(0.001f, NODE_Y_POSITION, -2.061f),   // 22
    glm::vec3(-2.081f, NODE_Y_POSITION, -2.045f)   // 23
};

static glm::mat4 transformation_matrix(glm::vec3 position, glm::vec3 rotation, float scale) {
    glm::mat4 matrix {1.0f};
    matrix = glm::translate(matrix, position);
    matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::scale(matrix, glm::vec3(scale));

    return matrix;
}

StandardBoard::StandardBoard(
    const sm::Renderable& board,
    const sm::Renderable& board_paint,
    const std::vector<sm::Renderable>& nodes,
    const std::vector<sm::Renderable>& white_pieces,
    const std::vector<sm::Renderable>& black_pieces,
    std::function<void(const Move&)>&& move_callback
)
    : m_move_callback(std::move(move_callback)) {
    m_renderable = board;
    m_renderable.transform.scale = 20.0f;

    m_paint_renderable = board_paint;
    m_paint_renderable.transform.scale = 20.0f;
    m_paint_renderable.transform.position.y = 0.062f;

    for (int i {0}; i < 24; i++) {
        m_nodes[i] = NodeObj(i, NODE_POSITIONS[i], nodes[i]);
    }

    // Offset pieces' IDs, so that they are different from nodes' IDs

    for (int i {0}; i < 9; i++) {
        m_pieces[i] = PieceObj(i + 24, glm::vec3(-3.0f, 0.5f, static_cast<float>(i) * 0.5f - 2.0f), white_pieces[i], PieceType::White);
    }

    for (int i {9}; i < 18; i++) {
        m_pieces[i] = PieceObj(i + 24, glm::vec3(3.0f, 0.5f, static_cast<float>(i - 9) * -0.5f + 2.0f), black_pieces[i - 9], PieceType::Black);
    }

    m_legal_moves = generate_moves();
}

void StandardBoard::set_board_paint_renderable(const sm::Renderable& board_paint) {
    m_paint_renderable = board_paint;

    m_paint_renderable = board_paint;  // FIXME dry
    m_paint_renderable.transform.scale = 20.0f;
    m_paint_renderable.transform.position.y = 0.062f;
}

void StandardBoard::update(sm::Ctx& ctx, glm::vec3 ray, glm::vec3 camera) {
    update_hovered_id(ray, camera);
    update_nodes();
    update_pieces();

    ctx.add_renderable(m_renderable);
    ctx.add_renderable(m_paint_renderable);

    for (auto& node : m_nodes) {
        node.update(ctx);
    }

    for (auto& piece : m_pieces) {
        piece.update(ctx);
    }
}

void StandardBoard::update_movement() {
    for (PieceObj& piece : m_pieces) {
        piece.update_movement();
    }
}

void StandardBoard::user_click_press() {
    if (m_game_over != GameOver::None) {
        return;
    }

    m_clicked_id = m_hovered_id;
}

void StandardBoard::user_click_release() {
    if (m_game_over != GameOver::None) {
        m_clicked_id = -1;
        return;
    }

    if (m_hovered_id == -1 || m_hovered_id != m_clicked_id) {
        m_clicked_id = -1;
        return;
    }

    m_clicked_id = -1;

    if (m_plies >= 18) {
        if (m_user_must_take_piece) {
            if (is_piece_id(m_hovered_id)) {
                try_move_take(m_user_stored_index1, m_user_stored_index2, m_pieces[PIECE(m_hovered_id)].node_id);
            }
        } else {
            if (is_node_id(m_hovered_id)) {
                try_move(m_user_stored_index1, m_nodes[m_hovered_id].get_id());
            }

            if (is_piece_id(m_hovered_id)) {
                select(m_pieces[PIECE(m_hovered_id)].node_id);
            }
        }
    } else {
        if (m_user_must_take_piece) {
            if (is_piece_id(m_hovered_id)) {
                try_place_take(m_user_stored_index2, m_pieces[PIECE(m_hovered_id)].node_id);
            }
        } else {
            if (is_node_id(m_hovered_id)) {
                try_place(m_nodes[m_hovered_id].get_id());
            }
        }
    }
}

void StandardBoard::place_piece(int place_index) {  // TODO test these
    const auto iter {std::find_if(m_legal_moves.begin(), m_legal_moves.end(), [=](const Move& move) {
        return move.type == MoveType::Place && move.place.place_index == place_index;
    })};

    assert(iter != m_legal_moves.end());

    place(place_index);

    const int id {new_piece_to_place(static_cast<PieceType>(opponent(m_turn)))};

    m_pieces[PIECE(id)].node_id = place_index;
    m_nodes[place_index].piece_id = id;

    do_place_animation(m_pieces[PIECE(id)], m_nodes[place_index], []() {});
}

void StandardBoard::place_take_piece(int place_index, int take_index) {
    const auto iter {std::find_if(m_legal_moves.begin(), m_legal_moves.end(), [=](const Move& move) {
        return (
            move.type == MoveType::PlaceTake &&
            move.place_take.place_index == place_index &&
            move.place_take.take_index == take_index
        );
    })};

    assert(iter != m_legal_moves.end());

    place_take(place_index, take_index);

    const int id {new_piece_to_place(static_cast<PieceType>(opponent(m_turn)))};

    m_pieces[PIECE(id)].node_id = place_index;
    m_pieces[PIECE(m_nodes[take_index].piece_id)].node_id = -1;
    m_nodes[place_index].piece_id = id;

    const int take_piece_id {m_nodes[take_index].piece_id};

    m_nodes[take_index].piece_id = -1;

    do_place_animation(m_pieces[PIECE(id)], m_nodes[place_index], [=, this]() {
        m_pieces[PIECE(take_piece_id)].to_remove = true;

        do_take_animation(m_pieces[PIECE(take_piece_id)], [=, this]() {
            m_pieces[PIECE(take_piece_id)].active = false;
        });
    });
}

void StandardBoard::move_piece(int source_index, int destination_index) {
    const auto iter {std::find_if(m_legal_moves.begin(), m_legal_moves.end(), [=](const Move& move) {
        return (
            move.type == MoveType::Move &&
            move.move.source_index == source_index &&
            move.move.destination_index == destination_index
        );
    })};

    assert(iter != m_legal_moves.end());

    move(source_index, destination_index);

    m_pieces[PIECE(m_nodes[source_index].piece_id)].node_id = destination_index;
    m_nodes[destination_index].piece_id = m_nodes[source_index].piece_id;
    m_nodes[source_index].piece_id = -1;

    do_move_animation(m_pieces[PIECE(m_nodes[source_index].piece_id)], m_nodes[destination_index], []() {});
}

void StandardBoard::move_take_piece(int source_index, int destination_index, int take_index) {
    const auto iter {std::find_if(m_legal_moves.begin(), m_legal_moves.end(), [=](const Move& move) {
        return (
            move.type == MoveType::MoveTake &&
            move.move_take.source_index == source_index &&
            move.move_take.destination_index == destination_index &&
            move.move_take.take_index == take_index
        );
    })};

    assert(iter != m_legal_moves.end());

    move_take(source_index, destination_index, take_index);

    m_pieces[PIECE(m_nodes[source_index].piece_id)].node_id = destination_index;
    m_nodes[destination_index].piece_id = m_nodes[source_index].piece_id;
    m_nodes[source_index].piece_id = -1;
    m_pieces[PIECE(m_nodes[take_index].piece_id)].node_id = -1;

    const int take_piece_id {m_nodes[take_index].piece_id};

    m_nodes[take_index].piece_id = -1;

    do_move_animation(m_pieces[PIECE(m_nodes[source_index].piece_id)], m_nodes[destination_index], [=, this]() {
        m_pieces[PIECE(take_piece_id)].to_remove = true;

        do_take_animation(m_pieces[PIECE(take_piece_id)], [=, this]() {
            m_pieces[PIECE(take_piece_id)].active = false;
        });
    });
}

void StandardBoard::debug() {
    if (ImGui::Begin("Debug Board")) {
        const char* turn {};
        switch (m_turn) {
            case Player::White:
                turn = "White";
                break;
            case Player::Black:
                turn = "Black";
                break;
        }

        const char* game_over {};
        switch (m_game_over) {
            case GameOver::None:
                game_over = "None";
                break;
            case GameOver::WinnerWhite:
                game_over = "WinnerWhite";
                break;
            case GameOver::WinnerBlack:
                game_over = "WinnerBlack";
                break;
            case GameOver::TieBetweenBothPlayers:
                game_over = "TieBetweenBothPlayers";
                break;
        }

        ImGui::Text("turn %s", turn);
        ImGui::Text("game_over %s", game_over);
        ImGui::Text("plies %u", m_plies);
        ImGui::Text("plies_without_advancement %u", m_plies_without_advancement);
        ImGui::Text("positions %lu", m_positions.size());
        ImGui::Text("legal_moves %lu", m_legal_moves.size());
        ImGui::Text("clicked_id %d", m_clicked_id);
        ImGui::Text("hovered_id %d", m_hovered_id);
        ImGui::Text("user_stored_index1 %d", m_user_stored_index1);
        ImGui::Text("user_stored_index2 %d", m_user_stored_index2);
        ImGui::Text("user_must_take_piece %s", m_user_must_take_piece ? "true" : "false");
    }

    ImGui::End();
}

void StandardBoard::update_hovered_id(glm::vec3 ray, glm::vec3 camera) {
    if (camera.y < 0.0f) {
        m_hovered_id = -1;
        return;
    }

    std::vector<std::pair<int, sm::Renderable>> renderables;

    for (const NodeObj& node : m_nodes) {
        renderables.push_back(std::make_pair(node.get_id(), node.get_renderable()));
    }

    for (const PieceObj& piece : m_pieces) {
        if (!(piece.active && !piece.to_remove)) {
            continue;
        }

        renderables.push_back(std::make_pair(piece.get_id(), piece.get_renderable()));
    }

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

#ifdef __GNUG__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wparentheses"
#endif

void StandardBoard::update_nodes() {
    if (m_game_over != GameOver::None) {
        std::for_each(m_nodes.begin(), m_nodes.end(), [](NodeObj& node) {
            node.set_highlighted(false);
        });

        return;
    }

    /*
        Highlight if:
        first phase and not must take piece
        second phase and piece selected and not must take piece
    */
    if (!(m_plies < 18 && m_user_stored_index2 == -1 || m_plies >= 18 && m_user_stored_index1 != -1 && m_user_stored_index2 == -1)) {
        std::for_each(m_nodes.begin(), m_nodes.end(), [](NodeObj& node) {
            node.set_highlighted(false);
        });

        return;
    }

    for (NodeObj& node : m_nodes) {
        node.set_highlighted(node.get_id() == m_hovered_id);
    }
}

void StandardBoard::update_pieces() {
    if (m_game_over != GameOver::None) {
        std::for_each(m_pieces.begin(), m_pieces.end(), [](PieceObj& piece) {
            piece.get_renderable().get_material()->flags &= ~sm::Material::Outline;
        });

        return;
    }

    /*
        Highlight if:
        must take piece and piece is opponent's piece and piece is on the board
        second phase and piece is ours and not must take piece
    */
    for (PieceObj& piece : m_pieces) {
        const bool highlight {
            m_user_stored_index2 != -1 && static_cast<Player>(piece.get_type()) != m_turn && piece.node_id != -1 ||
            m_plies >= 18 && static_cast<Player>(piece.get_type()) == m_turn && m_user_stored_index2 == -1
        };

        if (piece.get_id() == m_hovered_id && highlight) {
            piece.get_renderable().get_material()->flags |= sm::Material::Outline;
            piece.get_renderable().outline.color = glm::vec3(0.96f, 0.58f, 0.15f);
        } else {
            piece.get_renderable().get_material()->flags &= ~sm::Material::Outline;
        }
    }

    // Override, if the piece is actually selected
    if (m_user_stored_index1 != -1) {
        const int piece_id {m_nodes[m_user_stored_index1].piece_id};

        if (piece_id != -1) {
            m_pieces[PIECE(piece_id)].get_renderable().get_material()->flags |= sm::Material::Outline;
            m_pieces[PIECE(piece_id)].get_renderable().outline.color = glm::vec3(0.8f, 0.16f, 0.3f);
        }
    }
}

#ifdef __GNUG__
    #pragma GCC diagnostic pop
#endif

void StandardBoard::select(int index) {
    if (m_user_stored_index1 == -1) {
        if (m_board[index] == static_cast<Piece>(m_turn)) {
            m_user_stored_index1 = index;
        }
    } else {
        if (index == m_user_stored_index1) {
            if (m_user_stored_index2 == -1) {
                m_user_stored_index1 = -1;
            }
        } else if (m_board[index] == static_cast<Piece>(m_turn)) {
            if (m_user_stored_index2 == -1) {
                m_user_stored_index1 = index;
            }
        }
    }
}

void StandardBoard::try_place(int place_index) {
    auto iter {std::find_if(m_legal_moves.begin(), m_legal_moves.end(), [=](const Move& move) {
        return move.type == MoveType::Place && move.place.place_index == place_index;
    })};

    if (iter != m_legal_moves.end()) {
        user_place(place_index);
        return;
    }

    iter = std::find_if(m_legal_moves.begin(), m_legal_moves.end(), [=](const Move& move) {
        return move.type == MoveType::PlaceTake && move.place_take.place_index == place_index;
    });

    if (iter != m_legal_moves.end()) {
        user_place_take_just_place(place_index);
    }
}

void StandardBoard::try_place_take(int place_index, int take_index) {
    const auto iter {std::find_if(m_legal_moves.begin(), m_legal_moves.end(), [=](const Move& move) {
        return (
            move.type == MoveType::PlaceTake &&
            move.place_take.place_index == place_index &&
            move.place_take.take_index == take_index
        );
    })};

    if (iter != m_legal_moves.end()) {
        user_place_take(place_index, take_index);
    }
}

void StandardBoard::try_move(int source_index, int destination_index) {
    auto iter {std::find_if(m_legal_moves.begin(), m_legal_moves.end(), [=](const Move& move) {
        return (
            move.type == MoveType::Move &&
            move.move.source_index == source_index &&
            move.move.destination_index == destination_index
        );
    })};

    if (iter != m_legal_moves.end()) {
        user_move(source_index, destination_index);
        return;
    }

    iter = std::find_if(m_legal_moves.begin(), m_legal_moves.end(), [=](const Move& move) {
        return (
            move.type == MoveType::MoveTake &&
            move.move_take.source_index == source_index &&
            move.move_take.destination_index == destination_index
        );
    });

    if (iter != m_legal_moves.end()) {
        user_move_take_just_move(source_index, destination_index);
    }
}

void StandardBoard::try_move_take(int source_index, int destination_index, int take_index) {
    const auto iter {std::find_if(m_legal_moves.begin(), m_legal_moves.end(), [=](const Move& move) {
        return (
            move.type == MoveType::MoveTake &&
            move.move_take.source_index == source_index &&
            move.move_take.destination_index == destination_index &&
            move.move_take.take_index == take_index
        );
    })};

    if (iter != m_legal_moves.end()) {
        user_move_take(source_index, destination_index, take_index);
    }
}

void StandardBoard::user_place(int place_index) {
    place(place_index);

    const int id {new_piece_to_place(static_cast<PieceType>(opponent(m_turn)))};

    m_pieces[PIECE(id)].node_id = place_index;
    m_nodes[place_index].piece_id = id;

    do_place_animation(m_pieces[PIECE(id)], m_nodes[place_index], []() {});
}

void StandardBoard::user_place_take_just_place(int place_index) {
    m_user_must_take_piece = true;
    m_user_stored_index2 = place_index;

    const int id {new_piece_to_place(static_cast<PieceType>(m_turn))};

    m_pieces[PIECE(id)].node_id = place_index;
    m_nodes[place_index].piece_id = id;

    do_place_animation(m_pieces[PIECE(id)], m_nodes[place_index], []() {});
}

void StandardBoard::user_place_take(int place_index, int take_index) {
    place_take(place_index, take_index);

    m_pieces[PIECE(m_nodes[take_index].piece_id)].node_id = -1;

    const int take_piece_id {m_nodes[take_index].piece_id};

    m_nodes[take_index].piece_id = -1;
    m_pieces[PIECE(take_piece_id)].to_remove = true;

    do_take_animation(m_pieces[PIECE(take_piece_id)], [this, take_piece_id]() {
        m_pieces[PIECE(take_piece_id)].active = false;
    });
}

void StandardBoard::user_move(int source_index, int destination_index) {
    move(source_index, destination_index);

    m_pieces[PIECE(m_nodes[source_index].piece_id)].node_id = destination_index;
    m_nodes[destination_index].piece_id = m_nodes[source_index].piece_id;

    const int move_piece_id {m_nodes[source_index].piece_id};

    m_nodes[source_index].piece_id = -1;

    do_move_animation(m_pieces[PIECE(move_piece_id)], m_nodes[destination_index], []() {});
}

void StandardBoard::user_move_take_just_move(int source_index, int destination_index) {
    m_user_must_take_piece = true;
    m_user_stored_index2 = destination_index;

    m_pieces[PIECE(m_nodes[source_index].piece_id)].node_id = destination_index;
    m_nodes[destination_index].piece_id = m_nodes[source_index].piece_id;

    const int move_piece_id {m_nodes[source_index].piece_id};

    m_nodes[source_index].piece_id = -1;

    do_move_animation(m_pieces[PIECE(move_piece_id)], m_nodes[destination_index], []() {});
}

void StandardBoard::user_move_take(int source_index, int destination_index, int take_index) {
    move_take(source_index, destination_index, take_index);

    m_pieces[PIECE(m_nodes[take_index].piece_id)].node_id = -1;

    const int take_piece_id {m_nodes[take_index].piece_id};

    m_nodes[take_index].piece_id = -1;
    m_pieces[PIECE(take_piece_id)].to_remove = true;

    do_take_animation(m_pieces[PIECE(take_piece_id)], [this, take_piece_id]() {
        m_pieces[PIECE(take_piece_id)].active = false;
    });
}

void StandardBoard::place(int place_index) {
    assert(m_board[place_index] == Piece::None);

    m_board[place_index] = static_cast<Piece>(m_turn);

    Move move;
    move.type = MoveType::Place;
    move.place.place_index = place_index;

    finish_turn();
    check_winner_blocking();

    m_move_callback(move);
}

void StandardBoard::place_take(int place_index, int take_index) {
    assert(m_board[place_index] == Piece::None);
    assert(m_board[take_index] != Piece::None);

    m_board[place_index] = static_cast<Piece>(m_turn);
    m_board[take_index] = Piece::None;

    Move move;
    move.type = MoveType::PlaceTake;
    move.place_take.place_index = place_index;
    move.place_take.take_index = take_index;

    finish_turn();
    check_winner_material();
    check_winner_blocking();

    m_move_callback(move);
}

void StandardBoard::move(int source_index, int destination_index) {
    assert(m_board[source_index] != Piece::None);
    assert(m_board[destination_index] == Piece::None);

    std::swap(m_board[source_index], m_board[destination_index]);

    Move move;
    move.type = MoveType::Move;
    move.move.source_index = source_index;
    move.move.destination_index = destination_index;

    finish_turn(false);
    check_winner_blocking();
    check_fifty_move_rule();
    check_threefold_repetition({m_board, m_turn});

    m_move_callback(move);
}

void StandardBoard::move_take(int source_index, int destination_index, int take_index) {
    assert(m_board[source_index] != Piece::None);
    assert(m_board[destination_index] == Piece::None);
    assert(m_board[take_index] != Piece::None);

    std::swap(m_board[source_index], m_board[destination_index]);
    m_board[take_index] = Piece::None;

    Move move;
    move.type = MoveType::MoveTake;
    move.move_take.source_index = source_index;
    move.move_take.destination_index = destination_index;
    move.move_take.take_index = take_index;

    finish_turn();
    check_winner_material();
    check_winner_blocking();

    m_move_callback(move);
}

void StandardBoard::finish_turn(bool advancement) {
    if (m_turn == Player::White) {
        m_turn = Player::Black;
    } else {
        m_turn = Player::White;
    }

    m_plies++;
    m_legal_moves = generate_moves();

    if (advancement) {
        m_plies_without_advancement = 0;
        m_positions.clear();
    } else {
        m_plies_without_advancement++;
    }

    m_positions.push_back({m_board, m_turn});

    m_user_stored_index1 = -1;
    m_user_stored_index2 = -1;
    m_user_must_take_piece = false;
}

void StandardBoard::check_winner_material() {
    if (m_game_over != GameOver::None) {
        return;
    }

    if (m_plies < 18) {
        return;
    }

    if (count_pieces(m_board, m_turn) < 3) {
        m_game_over = static_cast<GameOver>(opponent(m_turn));
    }
}

void StandardBoard::check_winner_blocking() {
    if (m_game_over != GameOver::None) {
        return;
    }

    if (m_legal_moves.empty()) {
        m_game_over = static_cast<GameOver>(opponent(m_turn));
    }
}

void StandardBoard::check_fifty_move_rule() {
    if (m_game_over != GameOver::None) {
        return;
    }

    if (m_plies_without_advancement == 100) {
        m_game_over = GameOver::TieBetweenBothPlayers;
    }
}

void StandardBoard::check_threefold_repetition(const Position& position) {
    if (m_game_over != GameOver::None) {
        return;
    }

    unsigned int repetitions {1};

    for (auto iter {m_positions.begin()}; iter != std::prev(m_positions.end()); iter++) {
        if (*iter == position) {
            if (++repetitions == 3) {
                m_game_over = GameOver::TieBetweenBothPlayers;
                return;
            }
        }
    }
}

void StandardBoard::do_place_animation(PieceObj& piece, const NodeObj& node, std::function<void()>&& on_finish) {
    const glm::vec3 origin {piece.get_renderable().transform.position};
    const glm::vec3 target0 {piece.get_renderable().transform.position.x, 0.75f, piece.get_renderable().transform.position.z};
    const glm::vec3 target1 {node.get_renderable().transform.position.x, 0.75f, node.get_renderable().transform.position.z};
    const glm::vec3 target {node.get_renderable().transform.position.x, 0.135f, node.get_renderable().transform.position.z};

    piece.move_three_step(origin, target0, target1, target, std::move(on_finish));
}

void StandardBoard::do_move_animation(PieceObj& piece, const NodeObj& node, std::function<void()>&& on_finish) {
    if (count_pieces(m_board, static_cast<Player>(piece.get_type())) > 3) {
        const glm::vec3 origin {piece.get_renderable().transform.position};
        const glm::vec3 target {node.get_renderable().transform.position.x, 0.135f, node.get_renderable().transform.position.z};

        piece.move_direct(origin, target, std::move(on_finish));
    } else {
        const glm::vec3 origin {piece.get_renderable().transform.position};
        const glm::vec3 target0 {piece.get_renderable().transform.position.x, 0.75f, piece.get_renderable().transform.position.z};
        const glm::vec3 target1 {node.get_renderable().transform.position.x, 0.75f, node.get_renderable().transform.position.z};
        const glm::vec3 target {node.get_renderable().transform.position.x, 0.135f, node.get_renderable().transform.position.z};

        piece.move_three_step(origin, target0, target1, target, std::move(on_finish));
    }
}

void StandardBoard::do_take_animation(PieceObj& piece, std::function<void()>&& on_finish) {
    const glm::vec3 origin {piece.get_renderable().transform.position};
    const glm::vec3 target {piece.get_renderable().transform.position.x, 2.0f, piece.get_renderable().transform.position.z};

    piece.move_direct(origin, target, std::move(on_finish));
}

int StandardBoard::new_piece_to_place(PieceType type) {
    for (const PieceObj& piece : m_pieces) {
        if (piece.get_type() == type && piece.active && !piece.to_remove && piece.node_id == -1) {
            return piece.get_id();
        }
    }

    assert(false);
}

bool StandardBoard::is_node_id(int id) {
    return id >= 0 && id <= 23;
}

bool StandardBoard::is_piece_id(int id) {
    return id >= 24 && id <= 24 + 17;
}

std::vector<Move> StandardBoard::generate_moves() const {
    std::vector<Move> moves;
    Board board {m_board};

    if (m_plies < 18) {
        generate_moves_phase1(board, moves, m_turn);
    } else {
        if (count_pieces(board, m_turn) == 3) {
            generate_moves_phase3(board, moves, m_turn);
        } else {
            generate_moves_phase2(board, moves, m_turn);
        }
    }

    return moves;
}

void StandardBoard::generate_moves_phase1(Board& board, std::vector<Move>& moves, Player player) {
    for (int i {0}; i < 24; i++) {
        if (board[i] != Piece::None) {
            continue;
        }

        make_place_move(board, player, i);

        if (is_mill(board, player, i)) {
            const Player opponent_player {opponent(player)};
            const bool all_in_mills {all_pieces_in_mills(board, opponent_player)};

            for (int j {0}; j < 24; j++) {
                if (board[j] != static_cast<Piece>(opponent_player)) {
                    continue;
                }

                if (is_mill(board, opponent_player, j) && !all_in_mills) {
                    continue;
                }

                moves.push_back(create_place_take(i, j));
            }
        } else {
            moves.push_back(create_place(i));
        }

        unmake_place_move(board, i);
    }
}

void StandardBoard::generate_moves_phase2(Board& board, std::vector<Move>& moves, Player player) {
    for (int i {0}; i < 24; i++) {
        if (board[i] != static_cast<Piece>(player)) {
            continue;
        }

        const auto free_positions {neighbor_free_positions(board, i)};

        for (int j {0}; j < static_cast<int>(free_positions.size()); j++) {
            make_move_move(board, i, free_positions[j]);

            if (is_mill(board, player, free_positions[j])) {
                const Player opponent_player {opponent(player)};
                const bool all_in_mills {all_pieces_in_mills(board, opponent_player)};

                for (int k {0}; k < 24; k++) {
                    if (board[k] != static_cast<Piece>(opponent_player)) {
                        continue;
                    }

                    if (is_mill(board, opponent_player, k) && !all_in_mills) {
                        continue;
                    }

                    moves.push_back(create_move_take(i, free_positions[j], k));
                }
            } else {
                moves.push_back(create_move(i, free_positions[j]));
            }

            unmake_move_move(board, i, free_positions[j]);
        }
    }
}

void StandardBoard::generate_moves_phase3(Board& board, std::vector<Move>& moves, Player player) {
    for (int i {0}; i < 24; i++) {
        if (board[i] != static_cast<Piece>(player)) {
            continue;
        }

        for (int j {0}; j < 24; j++) {
            if (board[j] != Piece::None) {
                continue;
            }

            make_move_move(board, i, j);

            if (is_mill(board, player, j)) {
                const Player opponent_player {opponent(player)};
                const bool all_in_mills {all_pieces_in_mills(board, opponent_player)};

                for (int k {0}; k < 24; k++) {
                    if (board[k] != static_cast<Piece>(opponent_player)) {
                        continue;
                    }

                    if (is_mill(board, opponent_player, k) && !all_in_mills) {
                        continue;
                    }

                    moves.push_back(create_move_take(i, j, k));
                }
            } else {
                moves.push_back(create_move(i, j));
            }

            unmake_move_move(board, i, j);
        }
    }
}

void StandardBoard::make_place_move(Board& board, Player player, int place_index) {
    assert(board[place_index] == Piece::None);

    board[place_index] = static_cast<Piece>(player);
}

void StandardBoard::unmake_place_move(Board& board, int place_index) {
    assert(board[place_index] != Piece::None);

    board[place_index] = Piece::None;
}

void StandardBoard::make_move_move(Board& board, int source_index, int destination_index) {
    assert(board[source_index] != Piece::None);
    assert(board[destination_index] == Piece::None);

    std::swap(board[source_index], board[destination_index]);
}

void StandardBoard::unmake_move_move(Board& board, int source_index, int destination_index) {
    assert(board[source_index] == Piece::None);
    assert(board[destination_index] != Piece::None);

    std::swap(board[source_index], board[destination_index]);
}

#ifdef __GNUG__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wparentheses"
#endif

#define IS_PC(const_index) (board[const_index] == piece)

bool StandardBoard::is_mill(const Board& board, Player player, int index) {
    const Piece piece {static_cast<Piece>(player)};

    switch (index) {
        case 0:
            if (IS_PC(1) && IS_PC(2) || IS_PC(9) && IS_PC(21))
                return true;
            break;
        case 1:
            if (IS_PC(0) && IS_PC(2) || IS_PC(4) && IS_PC(7))
                return true;
            break;
        case 2:
            if (IS_PC(0) && IS_PC(1) || IS_PC(14) && IS_PC(23))
                return true;
            break;
        case 3:
            if (IS_PC(4) && IS_PC(5) || IS_PC(10) && IS_PC(18))
                return true;
            break;
        case 4:
            if (IS_PC(3) && IS_PC(5) || IS_PC(1) && IS_PC(7))
                return true;
            break;
        case 5:
            if (IS_PC(3) && IS_PC(4) || IS_PC(13) && IS_PC(20))
                return true;
            break;
        case 6:
            if (IS_PC(7) && IS_PC(8) || IS_PC(11) && IS_PC(15))
                return true;
            break;
        case 7:
            if (IS_PC(6) && IS_PC(8) || IS_PC(1) && IS_PC(4))
                return true;
            break;
        case 8:
            if (IS_PC(6) && IS_PC(7) || IS_PC(12) && IS_PC(17))
                return true;
            break;
        case 9:
            if (IS_PC(0) && IS_PC(21) || IS_PC(10) && IS_PC(11))
                return true;
            break;
        case 10:
            if (IS_PC(9) && IS_PC(11) || IS_PC(3) && IS_PC(18))
                return true;
            break;
        case 11:
            if (IS_PC(9) && IS_PC(10) || IS_PC(6) && IS_PC(15))
                return true;
            break;
        case 12:
            if (IS_PC(13) && IS_PC(14) || IS_PC(8) && IS_PC(17))
                return true;
            break;
        case 13:
            if (IS_PC(12) && IS_PC(14) || IS_PC(5) && IS_PC(20))
                return true;
            break;
        case 14:
            if (IS_PC(12) && IS_PC(13) || IS_PC(2) && IS_PC(23))
                return true;
            break;
        case 15:
            if (IS_PC(16) && IS_PC(17) || IS_PC(6) && IS_PC(11))
                return true;
            break;
        case 16:
            if (IS_PC(15) && IS_PC(17) || IS_PC(19) && IS_PC(22))
                return true;
            break;
        case 17:
            if (IS_PC(15) && IS_PC(16) || IS_PC(8) && IS_PC(12))
                return true;
            break;
        case 18:
            if (IS_PC(19) && IS_PC(20) || IS_PC(3) && IS_PC(10))
                return true;
            break;
        case 19:
            if (IS_PC(18) && IS_PC(20) || IS_PC(16) && IS_PC(22))
                return true;
            break;
        case 20:
            if (IS_PC(18) && IS_PC(19) || IS_PC(5) && IS_PC(13))
                return true;
            break;
        case 21:
            if (IS_PC(22) && IS_PC(23) || IS_PC(0) && IS_PC(9))
                return true;
            break;
        case 22:
            if (IS_PC(21) && IS_PC(23) || IS_PC(16) && IS_PC(19))
                return true;
            break;
        case 23:
            if (IS_PC(21) && IS_PC(22) || IS_PC(2) && IS_PC(14))
                return true;
            break;
    }

    return false;
}

#ifdef __GNUG__
    #pragma GCC diagnostic pop
#endif

bool StandardBoard::all_pieces_in_mills(const Board& board, Player player) {
    for (int i {0}; i < 24; i++) {
        if (board[i] != static_cast<Piece>(player)) {
            continue;
        }

        if (!is_mill(board, player, i)) {
            return false;
        }
    }

    return true;
}

#define IS_FREE_CHECK(const_index) \
    if (board[const_index] == Piece::None) { \
        result.push_back(const_index); \
    }

std::vector<int> StandardBoard::neighbor_free_positions(const Board& board, int index) {
    std::vector<int> result;
    result.reserve(4);

    switch (index) {
        case 0:
            IS_FREE_CHECK(1)
            IS_FREE_CHECK(9)
            break;
        case 1:
            IS_FREE_CHECK(0)
            IS_FREE_CHECK(2)
            IS_FREE_CHECK(4)
            break;
        case 2:
            IS_FREE_CHECK(1)
            IS_FREE_CHECK(14)
            break;
        case 3:
            IS_FREE_CHECK(4)
            IS_FREE_CHECK(10)
            break;
        case 4:
            IS_FREE_CHECK(1)
            IS_FREE_CHECK(3)
            IS_FREE_CHECK(5)
            IS_FREE_CHECK(7)
            break;
        case 5:
            IS_FREE_CHECK(4)
            IS_FREE_CHECK(13)
            break;
        case 6:
            IS_FREE_CHECK(7)
            IS_FREE_CHECK(11)
            break;
        case 7:
            IS_FREE_CHECK(4)
            IS_FREE_CHECK(6)
            IS_FREE_CHECK(8)
            break;
        case 8:
            IS_FREE_CHECK(7)
            IS_FREE_CHECK(12)
            break;
        case 9:
            IS_FREE_CHECK(0)
            IS_FREE_CHECK(10)
            IS_FREE_CHECK(21)
            break;
        case 10:
            IS_FREE_CHECK(3)
            IS_FREE_CHECK(9)
            IS_FREE_CHECK(11)
            IS_FREE_CHECK(18)
            break;
        case 11:
            IS_FREE_CHECK(6)
            IS_FREE_CHECK(10)
            IS_FREE_CHECK(15)
            break;
        case 12:
            IS_FREE_CHECK(8)
            IS_FREE_CHECK(13)
            IS_FREE_CHECK(17)
            break;
        case 13:
            IS_FREE_CHECK(5)
            IS_FREE_CHECK(12)
            IS_FREE_CHECK(14)
            IS_FREE_CHECK(20)
            break;
        case 14:
            IS_FREE_CHECK(2)
            IS_FREE_CHECK(13)
            IS_FREE_CHECK(23)
            break;
        case 15:
            IS_FREE_CHECK(11)
            IS_FREE_CHECK(16)
            break;
        case 16:
            IS_FREE_CHECK(15)
            IS_FREE_CHECK(17)
            IS_FREE_CHECK(19)
            break;
        case 17:
            IS_FREE_CHECK(12)
            IS_FREE_CHECK(16)
            break;
        case 18:
            IS_FREE_CHECK(10)
            IS_FREE_CHECK(19)
            break;
        case 19:
            IS_FREE_CHECK(16)
            IS_FREE_CHECK(18)
            IS_FREE_CHECK(20)
            IS_FREE_CHECK(22)
            break;
        case 20:
            IS_FREE_CHECK(13)
            IS_FREE_CHECK(19)
            break;
        case 21:
            IS_FREE_CHECK(9)
            IS_FREE_CHECK(22)
            break;
        case 22:
            IS_FREE_CHECK(19)
            IS_FREE_CHECK(21)
            IS_FREE_CHECK(23)
            break;
        case 23:
            IS_FREE_CHECK(14)
            IS_FREE_CHECK(22)
            break;
    }

    return result;
}

Move StandardBoard::create_place(int place_index) {
    Move move;
    move.type = MoveType::Place;
    move.place.place_index = place_index;

    return move;
}

Move StandardBoard::create_place_take(int place_index, int take_index) {
    Move move;
    move.type = MoveType::PlaceTake;
    move.place_take.place_index = place_index;
    move.place_take.take_index = take_index;

    return move;
}

Move StandardBoard::create_move(int source_index, int destination_index) {
    Move move;
    move.type = MoveType::Move;
    move.move.source_index = source_index;
    move.move.destination_index = destination_index;

    return move;
}

Move StandardBoard::create_move_take(int source_index, int destination_index, int take_index) {
    Move move;
    move.type = MoveType::MoveTake;
    move.move_take.source_index = source_index;
    move.move_take.destination_index = destination_index;
    move.move_take.take_index = take_index;

    return move;
}

unsigned int StandardBoard::count_pieces(const Board& board, Player player) {
    unsigned int result {0};

    for (const Piece piece : board) {
        result += static_cast<unsigned int>(piece == static_cast<Piece>(player));
    }

    return result;
}

Player StandardBoard::opponent(Player player) {
    if (player == Player::White) {
        return Player::Black;
    } else {
        return Player::White;
    }
}
