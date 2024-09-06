#include "jump_plus_variant_board.hpp"

#include <algorithm>
#include <utility>
#include <cassert>

#include <nine_morris_3d_engine/external/imgui.h++>

#define PIECE(index) (index - JumpPlusVariantBoard::NODES)

JumpPlusVariantBoard::JumpPlusVariantBoard(
    const sm::Renderable& board,
    const sm::Renderable& board_paint,
    const std::vector<sm::Renderable>& nodes,
    const std::vector<sm::Renderable>& white_pieces,
    const std::vector<sm::Renderable>& black_pieces,
    std::function<void(const Move&)>&& move_callback
)
    : BoardObj(board, board_paint), m_move_callback(std::move(move_callback)) {

    for (int i {0}; i < NODES; i++) {
        m_nodes[i] = NodeObj(i, NODE_POSITIONS[i], nodes[i]);
    }

    // Offset pieces' IDs, so that they are different from nodes' IDs

    m_pieces[0] = PieceObj(0 + NODES, glm::vec3(NODE_POSITIONS[0].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[0].z), white_pieces[0], PieceType::White);
    m_pieces[1] = PieceObj(1 + NODES, glm::vec3(NODE_POSITIONS[3].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[3].z), white_pieces[1], PieceType::White);
    m_pieces[2] = PieceObj(2 + NODES, glm::vec3(NODE_POSITIONS[6].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[6].z), white_pieces[2], PieceType::White);
    m_pieces[3] = PieceObj(3 + NODES, glm::vec3(NODE_POSITIONS[17].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[17].z), white_pieces[3], PieceType::White);
    m_pieces[4] = PieceObj(4 + NODES, glm::vec3(NODE_POSITIONS[20].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[20].z), white_pieces[4], PieceType::White);
    m_pieces[5] = PieceObj(5 + NODES, glm::vec3(NODE_POSITIONS[23].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[23].z), white_pieces[5], PieceType::White);

    m_pieces[6] = PieceObj(6 + NODES, glm::vec3(NODE_POSITIONS[2].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[2].z), black_pieces[6 - PIECES / 2], PieceType::Black);
    m_pieces[7] = PieceObj(7 + NODES, glm::vec3(NODE_POSITIONS[5].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[5].z), black_pieces[7 - PIECES / 2], PieceType::Black);
    m_pieces[8] = PieceObj(8 + NODES, glm::vec3(NODE_POSITIONS[8].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[8].z), black_pieces[8 - PIECES / 2], PieceType::Black);
    m_pieces[9] = PieceObj(9 + NODES, glm::vec3(NODE_POSITIONS[15].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[15].z), black_pieces[9 - PIECES / 2], PieceType::Black);
    m_pieces[10] = PieceObj(10 + NODES, glm::vec3(NODE_POSITIONS[18].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[18].z), black_pieces[10 - PIECES / 2], PieceType::Black);
    m_pieces[11] = PieceObj(11 + NODES, glm::vec3(NODE_POSITIONS[21].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[21].z), black_pieces[11 - PIECES / 2], PieceType::Black);

    m_pieces[0].node_id = 0;
    m_nodes[0].piece_id = m_pieces[0].get_id();

    m_pieces[1].node_id = 3;
    m_nodes[3].piece_id = m_pieces[1].get_id();

    m_pieces[2].node_id = 6;
    m_nodes[6].piece_id = m_pieces[2].get_id();

    m_pieces[3].node_id = 17;
    m_nodes[17].piece_id = m_pieces[3].get_id();

    m_pieces[4].node_id = 20;
    m_nodes[20].piece_id = m_pieces[4].get_id();

    m_pieces[5].node_id = 23;
    m_nodes[23].piece_id = m_pieces[5].get_id();


    m_pieces[6].node_id = 2;
    m_nodes[2].piece_id = m_pieces[6].get_id();

    m_pieces[7].node_id = 5;
    m_nodes[5].piece_id = m_pieces[7].get_id();

    m_pieces[8].node_id = 8;
    m_nodes[8].piece_id = m_pieces[8].get_id();

    m_pieces[9].node_id = 15;
    m_nodes[15].piece_id = m_pieces[9].get_id();

    m_pieces[10].node_id = 18;
    m_nodes[18].piece_id = m_pieces[10].get_id();

    m_pieces[11].node_id = 21;
    m_nodes[21].piece_id = m_pieces[11].get_id();

    m_board[0] = Piece::White;
    m_board[3] = Piece::White;
    m_board[6] = Piece::White;
    m_board[17] = Piece::White;
    m_board[20] = Piece::White;
    m_board[23] = Piece::White;

    m_board[2] = Piece::Black;
    m_board[5] = Piece::Black;
    m_board[8] = Piece::Black;
    m_board[15] = Piece::Black;
    m_board[18] = Piece::Black;
    m_board[21] = Piece::Black;

    m_legal_moves = generate_moves();
}

void JumpPlusVariantBoard::update(sm::Ctx& ctx, glm::vec3 ray, glm::vec3 camera) {
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

    update_nodes_highlight(m_nodes, m_game_over, [this]() {
        return m_selected_index != -1;
    });

    update_pieces_highlight<NODES>(m_pieces, m_nodes, m_game_over, m_selected_index, [this](const PieceObj& piece) {
        return static_cast<Player>(piece.get_type()) == m_turn;
    });

    ctx.add_renderable(m_renderable);
    ctx.add_renderable(m_paint_renderable);

    update_nodes(ctx, m_nodes);
    update_pieces(ctx, m_pieces);
}

void JumpPlusVariantBoard::update_movement() {
    BoardObj::update_movement(m_pieces);
}

void JumpPlusVariantBoard::user_click_press() {
    BoardObj::user_click_press(m_game_over);
}

void JumpPlusVariantBoard::user_click_release() {
    BoardObj::user_click_release(m_game_over, [this]() {
        if (is_node_id(m_hovered_id)) {
            try_move(m_selected_index, m_nodes[m_hovered_id].get_id());
        }

        if (is_piece_id(m_hovered_id)) {
            select(m_pieces[PIECE(m_hovered_id)].node_id);
        }
    });
}

void JumpPlusVariantBoard::move_piece(int source_index, int destination_index) {
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

void JumpPlusVariantBoard::debug() {
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

void JumpPlusVariantBoard::select(int index) {
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

void JumpPlusVariantBoard::try_move(int source_index, int destination_index) {
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

void JumpPlusVariantBoard::user_move(int source_index, int destination_index) {
    move(source_index, destination_index);

    m_pieces[PIECE(m_nodes[source_index].piece_id)].node_id = destination_index;
    m_nodes[destination_index].piece_id = m_nodes[source_index].piece_id;

    const int move_piece_id {m_nodes[source_index].piece_id};

    m_nodes[source_index].piece_id = -1;

    do_move_animation(
        m_pieces[PIECE(move_piece_id)],
        m_nodes[destination_index],
        []() {},
        false
    );
}

void JumpPlusVariantBoard::move(int source_index, int destination_index) {
    assert(m_board[source_index] != Piece::None);
    assert(m_board[destination_index] == Piece::None);

    std::swap(m_board[source_index], m_board[destination_index]);

    finish_turn();
    check_winner();
    check_fifty_move_rule();
    check_threefold_repetition({m_board, m_turn});

    m_move_callback(create_move(source_index, destination_index));
}

void JumpPlusVariantBoard::finish_turn() {
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

void JumpPlusVariantBoard::check_winner() {
    if (m_game_over != GameOver::None) {
        return;
    }

    for (int i {0}; i < NODES; i++) {
        if (m_board[i] != static_cast<Piece>(opponent(m_turn))) {
            continue;
        }

        if (is_mill(m_board, opponent(m_turn), i)) {
            m_game_over = static_cast<GameOver>(opponent(m_turn));
        }
    }
}

void JumpPlusVariantBoard::check_fifty_move_rule() {
    if (m_game_over != GameOver::None) {
        return;
    }

    if (m_plies_without_advancement == 100) {
        m_game_over = GameOver::TieBetweenBothPlayers;
    }
}

void JumpPlusVariantBoard::check_threefold_repetition(const Position& position) {
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

bool JumpPlusVariantBoard::is_node_id(int id) {
    return id >= 0 && id <= NODES - 1;
}

bool JumpPlusVariantBoard::is_piece_id(int id) {
    return id >= NODES && id <= NODES + PIECES - 1;
}

std::vector<JumpPlusVariantBoard::Move> JumpPlusVariantBoard::generate_moves() const {
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

JumpPlusVariantBoard::Move JumpPlusVariantBoard::create_move(int source_index, int destination_index) {
    Move move;
    move.source_index = source_index;
    move.destination_index = destination_index;

    return move;
}
