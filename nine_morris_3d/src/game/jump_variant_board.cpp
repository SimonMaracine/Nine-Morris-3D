#include "jump_variant_board.hpp"

#include <algorithm>
#include <utility>
#include <cassert>

#include <nine_morris_3d_engine/external/imgui.h++>

#define PIECE(index) (index - JumpVariantBoard::NODES)

JumpVariantBoard::JumpVariantBoard(
    const sm::Renderable& board,
    const sm::Renderable& board_paint,
    const std::vector<sm::Renderable>& nodes,
    const std::vector<sm::Renderable>& white_pieces,
    const std::vector<sm::Renderable>& black_pieces,
    std::function<void(const Move&)>&& move_callback
)
    : BoardObj(board, board_paint), m_move_callback(std::move(move_callback)) {
    m_nodes.resize(NODES);
    m_pieces.resize(PIECES);

    initialize_nodes(nodes);

    initialize_piece_on_board(m_board, white_pieces, 0, 4, 0, PieceType::White);
    initialize_piece_on_board(m_board, white_pieces, 1, 13, 1, PieceType::White);
    initialize_piece_on_board(m_board, white_pieces, 2, 15, 2, PieceType::White);
    initialize_piece_on_board(m_board, black_pieces, 3, 5, 0, PieceType::Black);
    initialize_piece_on_board(m_board, black_pieces, 4, 11, 1, PieceType::Black);
    initialize_piece_on_board(m_board, black_pieces, 5, 16, 2, PieceType::Black);

    m_legal_moves = generate_moves();
}

void JumpVariantBoard::update(sm::Ctx& ctx, glm::vec3 ray, glm::vec3 camera) {
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

    update_nodes_highlight(m_game_over, [this]() {
        return m_selected_index != -1;
    });

    update_pieces_highlight(m_game_over, m_selected_index, [this](const PieceObj& piece) {
        return static_cast<Player>(piece.get_type()) == m_turn;
    });

    ctx.add_renderable(m_renderable);
    ctx.add_renderable(m_paint_renderable);

    update_nodes(ctx);
    update_pieces(ctx);
}

void JumpVariantBoard::update_movement() {
    BoardObj::update_movement();
}

void JumpVariantBoard::user_click_press() {
    BoardObj::user_click_press(m_game_over);
}

void JumpVariantBoard::user_click_release() {
    BoardObj::user_click_release(m_game_over, [this]() {
        if (is_node_id(m_hovered_id)) {
            try_move(m_selected_index, m_nodes[m_hovered_id].get_id());
        }

        if (is_piece_id(m_hovered_id)) {
            select(m_pieces[PIECE(m_hovered_id)].node_id);
        }
    });
}

void JumpVariantBoard::move_piece(int source_index, int destination_index) {
    const auto iter {std::find_if(m_legal_moves.begin(), m_legal_moves.end(), [=](const Move& move) {
        return (
            move.source_index == source_index &&
            move.destination_index == destination_index
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
        false
    );
}

void JumpVariantBoard::debug() {
    if (ImGui::Begin("Debug Board")) {
        ImGui::Text("turn %s", turn_string(m_turn));
        ImGui::Text("game_over %s", game_over_string(m_game_over));
        ImGui::Text("plies_without_advancement %u", m_plies_without_advancement);
        ImGui::Text("positions %lu", m_positions.size());
        ImGui::Text("legal_moves %lu", m_legal_moves.size());
        ImGui::Text("clicked_id %d", m_clicked_id);
        ImGui::Text("hovered_id %d", m_hovered_id);
        ImGui::Text("selected_index %d", m_selected_index);
    }

    ImGui::End();
}

void JumpVariantBoard::select(int index) {
    if (m_selected_index == -1) {
        if (m_board[index] == static_cast<Piece>(m_turn)) {
            m_selected_index = index;
        }
    } else {
        if (index == m_selected_index) {
            m_selected_index = -1;
        } else if (m_board[index] == static_cast<Piece>(m_turn)) {
            m_selected_index = index;
        }
    }
}

void JumpVariantBoard::try_move(int source_index, int destination_index) {
    const auto iter {std::find_if(m_legal_moves.begin(), m_legal_moves.end(), [=](const Move& move) {
        return (
            move.source_index == source_index &&
            move.destination_index == destination_index
        );
    })};

    if (iter != m_legal_moves.end()) {
        user_move(source_index, destination_index);
        return;
    }
}

void JumpVariantBoard::user_move(int source_index, int destination_index) {
    move(source_index, destination_index);

    m_pieces[PIECE(m_nodes[source_index].piece_id)].node_id = destination_index;
    m_nodes[destination_index].piece_id = m_nodes[source_index].piece_id;

    const int move_piece_id {std::exchange(m_nodes[source_index].piece_id, -1)};

    do_move_animation(
        m_pieces[PIECE(move_piece_id)],
        m_nodes[destination_index],
        []() {},
        false
    );
}

void JumpVariantBoard::move(int source_index, int destination_index) {
    assert(m_board[source_index] != Piece::None);
    assert(m_board[destination_index] == Piece::None);

    std::swap(m_board[source_index], m_board[destination_index]);

    finish_turn();
    check_winner();
    check_fifty_move_rule();
    check_threefold_repetition({m_board, m_turn});

    m_move_callback(create_move(source_index, destination_index));
}

void JumpVariantBoard::finish_turn() {
    if (m_turn == Player::White) {
        m_turn = Player::Black;
    } else {
        m_turn = Player::White;
    }

    m_legal_moves = generate_moves();

    m_plies_without_advancement++;

    m_positions.push_back({m_board, m_turn});

    m_selected_index = -1;
}

void JumpVariantBoard::check_winner() {
    if (m_game_over != GameOver::None) {
        return;
    }

    for (int i {0}; i < NODES; i++) {
        if (m_board[i] != static_cast<Piece>(opponent(m_turn))) {
            continue;
        }

        if (is_mill(m_board, opponent(m_turn), i)) {
            m_game_over = GameOver(
                opponent(m_turn),
                format("%s player has made a mill.", if_player_white(m_turn, "Black", "White"))
            );
        }
    }
}

void JumpVariantBoard::check_fifty_move_rule() {
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

void JumpVariantBoard::check_threefold_repetition(const Position& position) {
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

bool JumpVariantBoard::is_node_id(int id) {
    return id >= 0 && id <= NODES - 1;
}

bool JumpVariantBoard::is_piece_id(int id) {
    return id >= NODES && id <= NODES + PIECES - 1;
}

std::vector<JumpVariantBoard::Move> JumpVariantBoard::generate_moves() const {
    std::vector<Move> moves;

    for (int i {0}; i < NODES; i++) {
        if (m_board[i] != static_cast<Piece>(m_turn)) {
            continue;
        }

        for (int j {0}; j < NODES; j++) {
            if (m_board[j] != Piece::None) {
                continue;
            }

            moves.push_back(create_move(i, j));
        }
    }

    return moves;
}

JumpVariantBoard::Move JumpVariantBoard::create_move(int source_index, int destination_index) {
    Move move;
    move.source_index = source_index;
    move.destination_index = destination_index;

    return move;
}
