#include "game/standard_game_board.hpp"

#include <algorithm>
#include <utility>
#include <cassert>

#include <nine_morris_3d_engine/external/imgui.h++>

#define PIECE(index) (index - StandardGameBoard::NODES)

StandardGameBoard::StandardGameBoard(
    const sm::Renderable& board,
    const sm::Renderable& board_paint,
    const std::vector<sm::Renderable>& nodes,
    const std::vector<sm::Renderable>& white_pieces,
    const std::vector<sm::Renderable>& black_pieces,
    std::function<void(const Move&)>&& move_callback
)
    : BoardObj(board, board_paint), m_move_callback(std::move(move_callback)) {
    initialize_nodes(m_nodes, nodes);

    for (int i {0}; i < PIECES / 2; i++) {
        initialize_piece_in_air(m_pieces, m_nodes, white_pieces, i, i, -3.0f, static_cast<float>(i) * 0.5f - 2.0f, PieceType::White);
    }

    for (int i {PIECES / 2}; i < PIECES; i++) {
        initialize_piece_in_air(m_pieces, m_nodes, black_pieces, i, i - PIECES / 2, 3.0f, static_cast<float>(i - PIECES / 2) * -0.5f + 2.0f, PieceType::Black);
    }

    m_legal_moves = generate_moves();
}

void StandardGameBoard::update(sm::Ctx& ctx, glm::vec3 ray, glm::vec3 camera) {
    update_hovered_id(ray, camera, [this]() {
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

        return renderables;
    });

#ifdef __GNUG__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wparentheses"
#endif

    update_nodes_highlight(m_nodes, m_game_over, [this]() {
        return (
            m_plies < 18 && m_take_action_index == -1 ||
            m_plies >= 18 && m_selected_index != -1 && m_take_action_index == -1
        );
    });

    update_pieces_highlight(m_pieces, m_nodes, m_game_over, m_selected_index, [this](const PieceObj& piece) {
        return (
            m_take_action_index != -1 && static_cast<Player>(piece.get_type()) != m_turn && piece.node_id != -1 ||
            m_plies >= 18 && static_cast<Player>(piece.get_type()) == m_turn && m_take_action_index == -1
        );
    });

#ifdef __GNUG__
    #pragma GCC diagnostic pop
#endif

    ctx.add_renderable(m_renderable);
    ctx.add_renderable(m_paint_renderable);

    update_nodes(ctx, m_nodes);
    update_pieces(ctx, m_pieces);
}

void StandardGameBoard::update_movement() {
    BoardObj::update_movement(m_pieces);
}

void StandardGameBoard::user_click_press() {
    BoardObj::user_click_press(m_game_over);
}

void StandardGameBoard::user_click_release() {
    BoardObj::user_click_release(m_game_over, [this]() {
        if (m_plies >= 18) {
            if (m_take_action_index != -1) {
                if (is_piece_id(m_hovered_id)) {
                    try_move_take(m_selected_index, m_take_action_index, m_pieces[PIECE(m_hovered_id)].node_id);
                }
            } else {
                if (is_node_id(m_hovered_id)) {
                    try_move(m_selected_index, m_nodes[m_hovered_id].get_id());
                }

                if (is_piece_id(m_hovered_id)) {
                    select(m_pieces[PIECE(m_hovered_id)].node_id);
                }
            }
        } else {
            if (m_take_action_index != -1) {
                if (is_piece_id(m_hovered_id)) {
                    try_place_take(m_take_action_index, m_pieces[PIECE(m_hovered_id)].node_id);
                }
            } else {
                if (is_node_id(m_hovered_id)) {
                    try_place(m_nodes[m_hovered_id].get_id());
                }
            }
        }
    });
}

void StandardGameBoard::place_piece(int place_index) {  // TODO test these
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

void StandardGameBoard::place_take_piece(int place_index, int take_index) {
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

void StandardGameBoard::move_piece(int source_index, int destination_index) {
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

    do_move_animation(
        m_pieces[PIECE(m_nodes[source_index].piece_id)],
        m_nodes[destination_index],
        []() {},
        !has_three_pieces(m_board, m_pieces[PIECE(m_nodes[source_index].piece_id)])
    );
}

void StandardGameBoard::move_take_piece(int source_index, int destination_index, int take_index) {
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

    do_move_animation(
        m_pieces[PIECE(m_nodes[source_index].piece_id)],
        m_nodes[destination_index],
        [=, this]() {
            m_pieces[PIECE(take_piece_id)].to_remove = true;

            do_take_animation(m_pieces[PIECE(take_piece_id)], [=, this]() {
                m_pieces[PIECE(take_piece_id)].active = false;
            });
        },
        !has_three_pieces(m_board, m_pieces[PIECE(m_nodes[source_index].piece_id)])
    );
}

void StandardGameBoard::debug() {
    if (ImGui::Begin("Debug Board")) {
        ImGui::Text("turn %s", turn_string(m_turn));
        ImGui::Text("game_over %s", game_over_string(m_game_over));
        ImGui::Text("plies %u", m_plies);
        ImGui::Text("plies_without_advancement %u", m_plies_without_advancement);
        ImGui::Text("positions %lu", m_positions.size());
        ImGui::Text("legal_moves %lu", m_legal_moves.size());
        ImGui::Text("clicked_id %d", m_clicked_id);
        ImGui::Text("hovered_id %d", m_hovered_id);
        ImGui::Text("selected_index %d", m_selected_index);
        ImGui::Text("take_action_index %d", m_take_action_index);
    }

    ImGui::End();
}

void StandardGameBoard::select(int index) {
    if (m_selected_index == -1) {
        if (m_board[index] == static_cast<Piece>(m_turn)) {
            m_selected_index = index;
        }
    } else {
        if (index == m_selected_index) {
            if (m_take_action_index == -1) {
                m_selected_index = -1;
            }
        } else if (m_board[index] == static_cast<Piece>(m_turn)) {
            if (m_take_action_index == -1) {
                m_selected_index = index;
            }
        }
    }
}

void StandardGameBoard::try_place(int place_index) {
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

void StandardGameBoard::try_place_take(int place_index, int take_index) {
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

void StandardGameBoard::try_move(int source_index, int destination_index) {
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

void StandardGameBoard::try_move_take(int source_index, int destination_index, int take_index) {
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

void StandardGameBoard::user_place(int place_index) {
    place(place_index);

    const int id {new_piece_to_place(static_cast<PieceType>(opponent(m_turn)))};

    m_pieces[PIECE(id)].node_id = place_index;
    m_nodes[place_index].piece_id = id;

    do_place_animation(m_pieces[PIECE(id)], m_nodes[place_index], []() {});
}

void StandardGameBoard::user_place_take_just_place(int place_index) {
    m_take_action_index = place_index;

    const int id {new_piece_to_place(static_cast<PieceType>(m_turn))};

    m_pieces[PIECE(id)].node_id = place_index;
    m_nodes[place_index].piece_id = id;

    do_place_animation(m_pieces[PIECE(id)], m_nodes[place_index], []() {});
}

void StandardGameBoard::user_place_take(int place_index, int take_index) {
    place_take(place_index, take_index);

    m_pieces[PIECE(m_nodes[take_index].piece_id)].node_id = -1;

    const int take_piece_id {m_nodes[take_index].piece_id};

    m_nodes[take_index].piece_id = -1;
    m_pieces[PIECE(take_piece_id)].to_remove = true;

    do_take_animation(m_pieces[PIECE(take_piece_id)], [this, take_piece_id]() {
        m_pieces[PIECE(take_piece_id)].active = false;
    });
}

void StandardGameBoard::user_move(int source_index, int destination_index) {
    move(source_index, destination_index);

    m_pieces[PIECE(m_nodes[source_index].piece_id)].node_id = destination_index;
    m_nodes[destination_index].piece_id = m_nodes[source_index].piece_id;

    const int move_piece_id {m_nodes[source_index].piece_id};

    m_nodes[source_index].piece_id = -1;

    do_move_animation(
        m_pieces[PIECE(move_piece_id)],
        m_nodes[destination_index],
        []() {},
        !has_three_pieces(m_board, m_pieces[PIECE(move_piece_id)])
    );
}

void StandardGameBoard::user_move_take_just_move(int source_index, int destination_index) {
    m_take_action_index = destination_index;

    m_pieces[PIECE(m_nodes[source_index].piece_id)].node_id = destination_index;
    m_nodes[destination_index].piece_id = m_nodes[source_index].piece_id;

    const int move_piece_id {m_nodes[source_index].piece_id};

    m_nodes[source_index].piece_id = -1;

    do_move_animation(
        m_pieces[PIECE(move_piece_id)],
        m_nodes[destination_index],
        []() {},
        !has_three_pieces(m_board, m_pieces[PIECE(move_piece_id)])
    );
}

void StandardGameBoard::user_move_take(int source_index, int destination_index, int take_index) {
    move_take(source_index, destination_index, take_index);

    m_pieces[PIECE(m_nodes[take_index].piece_id)].node_id = -1;

    const int take_piece_id {m_nodes[take_index].piece_id};

    m_nodes[take_index].piece_id = -1;
    m_pieces[PIECE(take_piece_id)].to_remove = true;

    do_take_animation(m_pieces[PIECE(take_piece_id)], [this, take_piece_id]() {
        m_pieces[PIECE(take_piece_id)].active = false;
    });
}

void StandardGameBoard::place(int place_index) {
    assert(m_board[place_index] == Piece::None);

    m_board[place_index] = static_cast<Piece>(m_turn);

    finish_turn();
    check_winner_blocking();

    m_move_callback(create_place(place_index));
}

void StandardGameBoard::place_take(int place_index, int take_index) {
    assert(m_board[place_index] == Piece::None);
    assert(m_board[take_index] != Piece::None);

    m_board[place_index] = static_cast<Piece>(m_turn);
    m_board[take_index] = Piece::None;

    finish_turn();
    check_winner_material();
    check_winner_blocking();

    m_move_callback(create_place_take(place_index, take_index));
}

void StandardGameBoard::move(int source_index, int destination_index) {
    assert(m_board[source_index] != Piece::None);
    assert(m_board[destination_index] == Piece::None);

    std::swap(m_board[source_index], m_board[destination_index]);

    finish_turn(false);
    check_winner_blocking();
    check_fifty_move_rule();
    check_threefold_repetition({m_board, m_turn});

    m_move_callback(create_move(source_index, destination_index));
}

void StandardGameBoard::move_take(int source_index, int destination_index, int take_index) {
    assert(m_board[source_index] != Piece::None);
    assert(m_board[destination_index] == Piece::None);
    assert(m_board[take_index] != Piece::None);

    std::swap(m_board[source_index], m_board[destination_index]);
    m_board[take_index] = Piece::None;

    finish_turn();
    check_winner_material();
    check_winner_blocking();

    m_move_callback(create_move_take(source_index, destination_index, take_index));
}

void StandardGameBoard::finish_turn(bool advancement) {
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

    m_selected_index = -1;
    m_take_action_index = -1;
}

void StandardGameBoard::check_winner_material() {
    if (m_game_over != GameOver::None) {
        return;
    }

    if (m_plies < 18) {
        return;
    }

    if (count_pieces(m_board, m_turn) < 3) {
        m_game_over = GameOver(
            opponent(m_turn),
            format("%s player cannot make any more mills.", if_player_white(m_turn, "White", "Black"))
        );
    }
}

void StandardGameBoard::check_winner_blocking() {
    if (m_game_over != GameOver::None) {
        return;
    }

    if (m_legal_moves.empty()) {
        m_game_over = GameOver(
            opponent(m_turn),
            format("%s player has no more legal moves to make.", if_player_white(m_turn, "White", "Black"))
        );
    }
}

void StandardGameBoard::check_fifty_move_rule() {
    if (m_game_over != GameOver::None) {
        return;
    }

    if (m_plies_without_advancement == 100) {
        m_game_over = GameOver(
            GameOver::TieBetweenBothPlayers,
            "Fifty moves have been made without a mill."
        );
    }
}

void StandardGameBoard::check_threefold_repetition(const Position& position) {
    if (m_game_over != GameOver::None) {
        return;
    }

    unsigned int repetitions {1};

    for (auto iter {m_positions.begin()}; iter != std::prev(m_positions.end()); iter++) {
        if (*iter == position) {
            if (++repetitions == 3) {
                m_game_over = GameOver(
                    GameOver::TieBetweenBothPlayers,
                    "The same position has appeared for the third time."
                );
                return;
            }
        }
    }
}

int StandardGameBoard::new_piece_to_place(PieceType type) {
    for (const PieceObj& piece : m_pieces) {
        if (piece.get_type() == type && piece.active && !piece.to_remove && piece.node_id == -1) {
            return piece.get_id();
        }
    }

    assert(false);
}

bool StandardGameBoard::is_node_id(int id) {
    return id >= 0 && id <= NODES - 1;
}

bool StandardGameBoard::is_piece_id(int id) {
    return id >= NODES && id <= NODES + PIECES - 1;
}

bool StandardGameBoard::has_three_pieces(const Board& board, const PieceObj& piece) {
    return count_pieces(board, static_cast<Player>(piece.get_type())) == 3;
}

std::vector<StandardGameBoard::Move> StandardGameBoard::generate_moves() const {
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

void StandardGameBoard::generate_moves_phase1(Board& board, std::vector<Move>& moves, Player player) {
    for (int i {0}; i < NODES; i++) {
        if (board[i] != Piece::None) {
            continue;
        }

        make_place_move(board, player, i);

        if (is_mill(board, player, i)) {
            const Player opponent_player {opponent(player)};
            const bool all_in_mills {all_pieces_in_mills(board, opponent_player)};

            for (int j {0}; j < NODES; j++) {
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

void StandardGameBoard::generate_moves_phase2(Board& board, std::vector<Move>& moves, Player player) {
    for (int i {0}; i < NODES; i++) {
        if (board[i] != static_cast<Piece>(player)) {
            continue;
        }

        const auto free_positions {neighbor_free_positions(board, i)};

        for (int j {0}; j < static_cast<int>(free_positions.size()); j++) {
            make_move_move(board, i, free_positions[j]);

            if (is_mill(board, player, free_positions[j])) {
                const Player opponent_player {opponent(player)};
                const bool all_in_mills {all_pieces_in_mills(board, opponent_player)};

                for (int k {0}; k < NODES; k++) {
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

void StandardGameBoard::generate_moves_phase3(Board& board, std::vector<Move>& moves, Player player) {
    for (int i {0}; i < NODES; i++) {
        if (board[i] != static_cast<Piece>(player)) {
            continue;
        }

        for (int j {0}; j < NODES; j++) {
            if (board[j] != Piece::None) {
                continue;
            }

            make_move_move(board, i, j);

            if (is_mill(board, player, j)) {
                const Player opponent_player {opponent(player)};
                const bool all_in_mills {all_pieces_in_mills(board, opponent_player)};

                for (int k {0}; k < NODES; k++) {
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

void StandardGameBoard::make_place_move(Board& board, Player player, int place_index) {
    assert(board[place_index] == Piece::None);

    board[place_index] = static_cast<Piece>(player);
}

void StandardGameBoard::unmake_place_move(Board& board, int place_index) {
    assert(board[place_index] != Piece::None);

    board[place_index] = Piece::None;
}

void StandardGameBoard::make_move_move(Board& board, int source_index, int destination_index) {
    assert(board[source_index] != Piece::None);
    assert(board[destination_index] == Piece::None);

    std::swap(board[source_index], board[destination_index]);
}

void StandardGameBoard::unmake_move_move(Board& board, int source_index, int destination_index) {
    assert(board[source_index] == Piece::None);
    assert(board[destination_index] != Piece::None);

    std::swap(board[source_index], board[destination_index]);
}

bool StandardGameBoard::all_pieces_in_mills(const Board& board, Player player) {
    for (int i {0}; i < NODES; i++) {
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

std::vector<int> StandardGameBoard::neighbor_free_positions(const Board& board, int index) {
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

StandardGameBoard::Move StandardGameBoard::create_place(int place_index) {
    Move move;
    move.type = MoveType::Place;
    move.place.place_index = place_index;

    return move;
}

StandardGameBoard::Move StandardGameBoard::create_place_take(int place_index, int take_index) {
    Move move;
    move.type = MoveType::PlaceTake;
    move.place_take.place_index = place_index;
    move.place_take.take_index = take_index;

    return move;
}

StandardGameBoard::Move StandardGameBoard::create_move(int source_index, int destination_index) {
    Move move;
    move.type = MoveType::Move;
    move.move.source_index = source_index;
    move.move.destination_index = destination_index;

    return move;
}

StandardGameBoard::Move StandardGameBoard::create_move_take(int source_index, int destination_index, int take_index) {
    Move move;
    move.type = MoveType::MoveTake;
    move.move_take.source_index = source_index;
    move.move_take.destination_index = destination_index;
    move.move_take.take_index = take_index;

    return move;
}
