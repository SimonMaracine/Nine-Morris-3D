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

    for (int i {0}; i < NODES; i++) {
        m_nodes[i] = NodeObj(i, NODE_POSITIONS[i], nodes[i]);
    }

    // Offset pieces' IDs, so that they are different from nodes' IDs

    m_pieces[0] = PieceObj(0 + NODES, glm::vec3(NODE_POSITIONS[4].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[4].z), white_pieces[0], PieceType::White);
    m_pieces[1] = PieceObj(1 + NODES, glm::vec3(NODE_POSITIONS[13].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[13].z), white_pieces[1], PieceType::White);
    m_pieces[2] = PieceObj(2 + NODES, glm::vec3(NODE_POSITIONS[15].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[15].z), white_pieces[2], PieceType::White);

    m_pieces[3] = PieceObj(3 + NODES, glm::vec3(NODE_POSITIONS[5].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[5].z), black_pieces[3 - PIECES / 2], PieceType::Black);
    m_pieces[4] = PieceObj(4 + NODES, glm::vec3(NODE_POSITIONS[11].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[11].z), black_pieces[4 - PIECES / 2], PieceType::Black);
    m_pieces[5] = PieceObj(5 + NODES, glm::vec3(NODE_POSITIONS[16].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[16].z), black_pieces[5 - PIECES / 2], PieceType::Black);

    m_pieces[0].node_id = 4;
    m_nodes[4].piece_id = m_pieces[0].get_id();

    m_pieces[1].node_id = 13;
    m_nodes[13].piece_id = m_pieces[1].get_id();

    m_pieces[2].node_id = 15;
    m_nodes[15].piece_id = m_pieces[2].get_id();

    m_pieces[3].node_id = 5;
    m_nodes[5].piece_id = m_pieces[3].get_id();

    m_pieces[4].node_id = 11;
    m_nodes[11].piece_id = m_pieces[4].get_id();

    m_pieces[5].node_id = 16;
    m_nodes[16].piece_id = m_pieces[5].get_id();

    m_board[4] = Piece::White;
    m_board[13] = Piece::White;
    m_board[15] = Piece::White;

    m_board[5] = Piece::Black;
    m_board[11] = Piece::Black;
    m_board[16] = Piece::Black;

    m_legal_moves = generate_moves();
}

void JumpVariantBoard::update(sm::Ctx& ctx, glm::vec3 ray, glm::vec3 camera) {  // TODO dry
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

    update_nodes();
    update_pieces();

    ctx.add_renderable(m_renderable);
    ctx.add_renderable(m_paint_renderable);

    for (NodeObj& node : m_nodes) {
        node.update(ctx);
    }

    for (PieceObj& piece : m_pieces) {
        piece.update(ctx);
    }
}

void JumpVariantBoard::update_movement() {  // TODO dry
    for (PieceObj& piece : m_pieces) {
        piece.update_movement();
    }
}

void JumpVariantBoard::user_click_press() {  // TODO dry
    if (m_game_over != GameOver::None) {
        return;
    }

    m_clicked_id = m_hovered_id;
}

void JumpVariantBoard::user_click_release() {  // TODO dry
    if (m_game_over != GameOver::None) {
        m_clicked_id = -1;
        return;
    }

    if (m_hovered_id == -1 || m_hovered_id != m_clicked_id) {
        m_clicked_id = -1;
        return;
    }

    m_clicked_id = -1;

    if (is_node_id(m_hovered_id)) {
        try_move(m_user_selected_index, m_nodes[m_hovered_id].get_id());
    }

    if (is_piece_id(m_hovered_id)) {
        select(m_pieces[PIECE(m_hovered_id)].node_id);
    }
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

void JumpVariantBoard::debug() {  // TODO dry
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
        ImGui::Text("plies_without_advancement %u", m_plies_without_advancement);
        ImGui::Text("positions %lu", m_positions.size());
        ImGui::Text("legal_moves %lu", m_legal_moves.size());
        ImGui::Text("clicked_id %d", m_clicked_id);
        ImGui::Text("hovered_id %d", m_hovered_id);
        ImGui::Text("user_selected_index %d", m_user_selected_index);
    }

    ImGui::End();
}

void JumpVariantBoard::update_nodes() {  // TODO dry
    if (m_game_over != GameOver::None) {
        std::for_each(m_nodes.begin(), m_nodes.end(), [](NodeObj& node) {
            node.set_highlighted(false);
        });

        return;
    }

    if (m_user_selected_index == -1) {
        std::for_each(m_nodes.begin(), m_nodes.end(), [](NodeObj& node) {
            node.set_highlighted(false);
        });

        return;
    }

    for (NodeObj& node : m_nodes) {
        node.set_highlighted(node.get_id() == m_hovered_id);
    }
}

void JumpVariantBoard::update_pieces() {  // TODO dry
    if (m_game_over != GameOver::None) {
        std::for_each(m_pieces.begin(), m_pieces.end(), [](PieceObj& piece) {
            piece.get_renderable().get_material()->flags &= ~sm::Material::Outline;
        });

        return;
    }

    for (PieceObj& piece : m_pieces) {
        const bool highlight {
            static_cast<Player>(piece.get_type()) == m_turn
        };

        if (piece.get_id() == m_hovered_id && highlight) {
            piece.get_renderable().get_material()->flags |= sm::Material::Outline;
            piece.get_renderable().outline.color = ORANGE;
        } else {
            piece.get_renderable().get_material()->flags &= ~sm::Material::Outline;
        }
    }

    // Override, if the piece is actually selected
    if (m_user_selected_index != -1) {
        const int piece_id {m_nodes[m_user_selected_index].piece_id};

        if (piece_id != -1) {
            m_pieces[PIECE(piece_id)].get_renderable().get_material()->flags |= sm::Material::Outline;
            m_pieces[PIECE(piece_id)].get_renderable().outline.color = RED;
        }
    }
}

void JumpVariantBoard::select(int index) {
    if (m_user_selected_index == -1) {
        if (m_board[index] == static_cast<Piece>(m_turn)) {
            m_user_selected_index = index;
        }
    } else {
        if (index == m_user_selected_index) {
            m_user_selected_index = -1;
        } else if (m_board[index] == static_cast<Piece>(m_turn)) {
            m_user_selected_index = index;
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

    const int move_piece_id {m_nodes[source_index].piece_id};

    m_nodes[source_index].piece_id = -1;

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

    m_user_selected_index = -1;
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
            m_game_over = static_cast<GameOver>(opponent(m_turn));
        }
    }
}

void JumpVariantBoard::check_fifty_move_rule() {
    if (m_game_over != GameOver::None) {
        return;
    }

    if (m_plies_without_advancement == 100) {
        m_game_over = GameOver::TieBetweenBothPlayers;
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
                m_game_over = GameOver::TieBetweenBothPlayers;
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

#ifdef __GNUG__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wparentheses"
#endif

#define IS_PC(const_index) (board[const_index] == piece)

bool JumpVariantBoard::is_mill(const Board& board, Player player, int index) {
    const Piece piece {static_cast<Piece>(player)};

    assert(board[index] == piece);

    switch (index) {
        case 0:
            return IS_PC(1) && IS_PC(2) || IS_PC(9) && IS_PC(21);
        case 1:
            return IS_PC(0) && IS_PC(2) || IS_PC(4) && IS_PC(7);
        case 2:
            return IS_PC(0) && IS_PC(1) || IS_PC(14) && IS_PC(23);
        case 3:
            return IS_PC(4) && IS_PC(5) || IS_PC(10) && IS_PC(18);
        case 4:
            return IS_PC(3) && IS_PC(5) || IS_PC(1) && IS_PC(7);
        case 5:
            return IS_PC(3) && IS_PC(4) || IS_PC(13) && IS_PC(20);
        case 6:
            return IS_PC(7) && IS_PC(8) || IS_PC(11) && IS_PC(15);
        case 7:
            return IS_PC(6) && IS_PC(8) || IS_PC(1) && IS_PC(4);
        case 8:
            return IS_PC(6) && IS_PC(7) || IS_PC(12) && IS_PC(17);
        case 9:
            return IS_PC(0) && IS_PC(21) || IS_PC(10) && IS_PC(11);
        case 10:
            return IS_PC(9) && IS_PC(11) || IS_PC(3) && IS_PC(18);
        case 11:
            return IS_PC(9) && IS_PC(10) || IS_PC(6) && IS_PC(15);
        case 12:
            return IS_PC(13) && IS_PC(14) || IS_PC(8) && IS_PC(17);
        case 13:
            return IS_PC(12) && IS_PC(14) || IS_PC(5) && IS_PC(20);
        case 14:
            return IS_PC(12) && IS_PC(13) || IS_PC(2) && IS_PC(23);
        case 15:
            return IS_PC(16) && IS_PC(17) || IS_PC(6) && IS_PC(11);
        case 16:
            return IS_PC(15) && IS_PC(17) || IS_PC(19) && IS_PC(22);
        case 17:
            return IS_PC(15) && IS_PC(16) || IS_PC(8) && IS_PC(12);
        case 18:
            return IS_PC(19) && IS_PC(20) || IS_PC(3) && IS_PC(10);
        case 19:
            return IS_PC(18) && IS_PC(20) || IS_PC(16) && IS_PC(22);
        case 20:
            return IS_PC(18) && IS_PC(19) || IS_PC(5) && IS_PC(13);
        case 21:
            return IS_PC(22) && IS_PC(23) || IS_PC(0) && IS_PC(9);
        case 22:
            return IS_PC(21) && IS_PC(23) || IS_PC(16) && IS_PC(19);
        case 23:
            return IS_PC(21) && IS_PC(22) || IS_PC(2) && IS_PC(14);
    }

    return {};
}

#ifdef __GNUG__
    #pragma GCC diagnostic pop
#endif

JumpVariantBoard::Move JumpVariantBoard::create_move(int source_index, int destination_index) {
    Move move;
    move.source_index = source_index;
    move.destination_index = destination_index;

    return move;
}
