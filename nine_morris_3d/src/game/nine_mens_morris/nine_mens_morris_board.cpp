#include "game/nine_mens_morris/nine_mens_morris_board.hpp"

#include <algorithm>
#include <utility>
#include <regex>
#include <cstring>
#include <cassert>

#include <nine_morris_3d_engine/external/imgui.h++>

#define PIECE(index) (index - NineMensMorrisBoard::NODES)

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

static const glm::vec3 RED {0.8f, 0.16f, 0.3f};
static const glm::vec3 ORANGE {0.96f, 0.58f, 0.15f};

static constexpr float PIECE_Y_POSITION_AIR_INITIAL {0.5f};
static constexpr float PIECE_Y_POSITION_BOARD {0.135f};

static constexpr float PIECE_Y_POSITION_AIR_MOVE {0.75f};
static constexpr float PIECE_Y_POSITION_AIR_TAKE {2.0f};

static int index_from_string(const std::string& string) {
    if (string == "a7") return 0;
    else if (string == "d7") return 1;
    else if (string == "g7") return 2;
    else if (string == "b6") return 3;
    else if (string == "d6") return 4;
    else if (string == "f6") return 5;
    else if (string == "c5") return 6;
    else if (string == "d5") return 7;
    else if (string == "e5") return 8;
    else if (string == "a4") return 9;
    else if (string == "b4") return 10;
    else if (string == "c4") return 11;
    else if (string == "e4") return 12;
    else if (string == "f4") return 13;
    else if (string == "g4") return 14;
    else if (string == "c3") return 15;
    else if (string == "d3") return 16;
    else if (string == "e3") return 17;
    else if (string == "b2") return 18;
    else if (string == "d2") return 19;
    else if (string == "f2") return 20;
    else if (string == "a1") return 21;
    else if (string == "d1") return 22;
    else if (string == "g1") return 23;

    throw BoardError("Invalid string");
}

static const char* index_to_string(int index) {
    switch (index) {
        case 0: return "a7";
        case 1: return "d7";
        case 2: return "g7";
        case 3: return "b6";
        case 4: return "d6";
        case 5: return "f6";
        case 6: return "c5";
        case 7: return "d5";
        case 8: return "e5";
        case 9: return "a4";
        case 10: return "b4";
        case 11: return "c4";
        case 12: return "e4";
        case 13: return "f4";
        case 14: return "g4";
        case 15: return "c3";
        case 16: return "d3";
        case 17: return "e3";
        case 18: return "b2";
        case 19: return "d2";
        case 20: return "f2";
        case 21: return "a1";
        case 22: return "d1";
        case 23: return "g1";
    }

    throw BoardError("Invalid index");
}

static std::vector<std::string> split(const std::string& message, const char* separator) {
    std::vector<std::string> tokens;
    std::string buffer {message};

    char* token {std::strtok(buffer.data(), separator)};

    while (token != nullptr) {
        tokens.emplace_back(token);
        token = std::strtok(nullptr, separator);
    }

    return tokens;
}

static NineMensMorrisBoard::Player parse_player(const std::string& string) {
    if (string == "w") {
        return NineMensMorrisBoard::Player::White;
    } else if (string == "b") {
        return NineMensMorrisBoard::Player::Black;
    } else {
        throw BoardError("Invalid string");
    }
}

static std::pair<std::vector<int>, NineMensMorrisBoard::Player> parse_pieces(const std::string& string) {
    const auto player {parse_player(string.substr(0, 1))};

    const auto tokens {split(string.substr(1), ",")};
    std::vector<int> pieces;

    for (const auto& token : tokens) {
        if (token.empty()) {
            continue;
        }

        pieces.push_back(index_from_string(token));
    }

    return std::make_pair(pieces, player);
}

bool NineMensMorrisBoard::Move::operator==(const Move& other) const {
    if (type != other.type) {
        return false;
    }

    switch (type) {
        case MoveType::Place:
            return place.place_index == other.place.place_index;
        case MoveType::PlaceCapture:
            return (
                place_capture.place_index == other.place_capture.place_index &&
                place_capture.capture_index == other.place_capture.capture_index
            );
        case MoveType::Move:
            return (
                move.source_index == other.move.source_index &&
                move.destination_index == other.move.destination_index
            );
        case MoveType::MoveCapture:
            return (
                move_capture.source_index == other.move_capture.source_index &&
                move_capture.destination_index == other.move_capture.destination_index &&
                move_capture.capture_index == other.move_capture.capture_index
            );
    }

    return {};
}

NineMensMorrisBoard::Move NineMensMorrisBoard::Move::create_place(int place_index) {
    Move move;
    move.type = MoveType::Place;
    move.place.place_index = place_index;

    return move;
}

NineMensMorrisBoard::Move NineMensMorrisBoard::Move::create_place_capture(int place_index, int capture_index) {
    Move move;
    move.type = MoveType::PlaceCapture;
    move.place_capture.place_index = place_index;
    move.place_capture.capture_index = capture_index;

    return move;
}

NineMensMorrisBoard::Move NineMensMorrisBoard::Move::create_move(int source_index, int destination_index) {
    Move move;
    move.type = MoveType::Move;
    move.move.source_index = source_index;
    move.move.destination_index = destination_index;

    return move;
}

NineMensMorrisBoard::Move NineMensMorrisBoard::Move::create_move_capture(int source_index, int destination_index, int capture_index) {
    Move move;
    move.type = MoveType::MoveCapture;
    move.move_capture.source_index = source_index;
    move.move_capture.destination_index = destination_index;
    move.move_capture.capture_index = capture_index;

    return move;
}

NineMensMorrisBoard::NineMensMorrisBoard(
    const sm::Renderable& board,
    const sm::Renderable& paint,
    const NodeRenderables& nodes,
    const PieceRenderables& white_pieces,
    const PieceRenderables& black_pieces,
    std::function<void(const Move&)>&& move_callback
)
    : m_move_callback(std::move(move_callback)) {
    m_board_renderable = board;
    m_board_renderable.transform.scale = 20.0f;

    m_paint_renderable = paint;
    m_paint_renderable.transform.scale = 20.0f;
    m_paint_renderable.transform.position.y = 0.062f;

    // The number of pieces given decides the variant of the game
    initialize_objects(nodes, white_pieces, black_pieces);

    // Move generation depends on the number of pieces
    m_legal_moves = generate_moves();
}

void NineMensMorrisBoard::user_click_release_callback() {
    if (m_position.plies >= m_pieces.size()) {
        if (m_capture_piece) {
            if (is_piece_id(m_hover_id)) {
                try_capture(m_pieces[PIECE(m_hover_id)].node_id);
            }
        } else {
            if (is_node_id(m_hover_id)) {
                try_move(m_select_id, m_nodes[m_hover_id].get_id());
            }

            if (is_piece_id(m_hover_id)) {
                select(m_pieces[PIECE(m_hover_id)].node_id);
            }
        }
    } else {
        if (m_capture_piece) {
            if (is_piece_id(m_hover_id)) {
                try_capture(m_pieces[PIECE(m_hover_id)].node_id);
            }
        } else {
            if (is_node_id(m_hover_id)) {
                try_place(m_nodes[m_hover_id].get_id());
            }
        }
    }
}

const GameOver& NineMensMorrisBoard::get_game_over() const {
    return m_game_over;
}

PlayerColor NineMensMorrisBoard::get_player_color() const {
    if (m_position.player == Player::White) {
        return PlayerColorWhite;
    } else {
        return PlayerColorBlack;
    }
}

bool NineMensMorrisBoard::is_turn_finished() const {
    for (const PieceObj& piece : m_pieces) {
        if (piece.is_moving()) {
            return false;
        }
    }

    return true;
}

void NineMensMorrisBoard::update(sm::Ctx& ctx, glm::vec3 ray, glm::vec3 camera, bool user_input) {
    if (user_input) {
        update_hover_id(ray, camera, [this]() {
            std::vector<HoverableObj> hoverables;

            for (const NodeObj& node : m_nodes) {
                hoverables.push_back(node);
            }

            for (const PieceObj& piece : m_pieces) {
                if (!(piece.active && !piece.to_remove)) {
                    continue;
                }

                hoverables.push_back(piece);
            }

            return hoverables;
        });
    }

#ifdef __GNUG__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wparentheses"
#endif

    update_nodes_highlight(
        [this]() {
            return (
                m_position.plies < m_pieces.size() && !m_capture_piece ||
                m_position.plies >= m_pieces.size() && m_select_id != -1 && !m_capture_piece
            );
        },
        user_input
    );

    update_pieces_highlight(
        [this](const PieceObj& piece) {
            return (
                m_capture_piece && static_cast<Player>(piece.get_type()) != m_position.player && piece.node_id != -1 ||
                m_position.plies >= m_pieces.size() && static_cast<Player>(piece.get_type()) == m_position.player && !m_capture_piece
            );
        },
        user_input
    );

#ifdef __GNUG__
    #pragma GCC diagnostic pop
#endif

    ctx.add_renderable(m_board_renderable);
    ctx.add_renderable(m_paint_renderable);

    update_nodes(ctx);
    update_pieces(ctx);
}

void NineMensMorrisBoard::update_movement() {
    for (PieceObj& piece : m_pieces) {
        piece.update_movement();
    }
}

void NineMensMorrisBoard::reset(const Position& position) {
    m_position = position;
    m_plies_no_advancement = 0;
    m_positions.clear();

    m_capture_piece = false;
    m_select_id = -1;
    m_game_over = GameOver();
    m_setup_position = m_position;

    m_legal_moves = generate_moves();

    initialize_objects();
}

void NineMensMorrisBoard::play_move(const Move& move) {
    const auto iter {std::find(m_legal_moves.cbegin(), m_legal_moves.cend(), move)};

    if (iter == m_legal_moves.cend()) {
        throw BoardError("Illegal move");
    }

    switch (move.type) {
        case MoveType::Place:
            {
                const int id {new_piece_to_place(static_cast<PieceType>(m_position.player))};

                m_pieces[PIECE(id)].node_id = move.place.place_index;
                m_nodes[move.place.place_index].piece_id = id;

                do_place_animation(m_pieces[PIECE(id)], m_nodes[move.place.place_index], [](PieceObj&) {});
            }

            play_place_move(move);

            break;
        case MoveType::PlaceCapture: {
            int take_id {};
            {
                m_pieces[PIECE(m_nodes[move.place_capture.capture_index].piece_id)].node_id = -1;

                const int id {std::exchange(m_nodes[move.place_capture.capture_index].piece_id, -1)};

                m_pieces[PIECE(id)].to_remove = true;

                take_id = id;
            }
            {
                const int id {new_piece_to_place(static_cast<PieceType>(m_position.player))};

                m_pieces[PIECE(id)].node_id = move.place.place_index;
                m_nodes[move.place.place_index].piece_id = id;

                do_place_animation(m_pieces[PIECE(id)], m_nodes[move.place.place_index], [this, take_id](PieceObj&) {
                    do_take_animation(m_pieces[PIECE(take_id)], [](PieceObj& piece) {
                        piece.active = false;
                    });
                });
            }

            play_place_capture_move(move);

            break;
        }
        case MoveType::Move:
            {
                m_pieces[PIECE(m_nodes[move.move.source_index].piece_id)].node_id = move.move.destination_index;
                m_nodes[move.move.destination_index].piece_id = m_nodes[move.move.source_index].piece_id;

                const int id {std::exchange(m_nodes[move.move.source_index].piece_id, -1)};

                do_move_animation(
                    m_pieces[PIECE(id)],
                    m_nodes[move.move.destination_index],
                    [](PieceObj&) {},
                    !has_three_pieces(m_position.board, m_pieces[PIECE(id)])
                );
            }

            play_move_move(move);

            break;
        case MoveType::MoveCapture: {
            int take_id {};
            {
                m_pieces[PIECE(m_nodes[move.move_capture.capture_index].piece_id)].node_id = -1;

                const int id {std::exchange(m_nodes[move.move_capture.capture_index].piece_id, -1)};

                m_pieces[PIECE(id)].to_remove = true;

                take_id = id;
            }
            {
                m_pieces[PIECE(m_nodes[move.move_capture.source_index].piece_id)].node_id = move.move_capture.destination_index;
                m_nodes[move.move_capture.destination_index].piece_id = m_nodes[move.move_capture.source_index].piece_id;

                const int id {std::exchange(m_nodes[move.move_capture.source_index].piece_id, -1)};

                do_move_animation(
                    m_pieces[PIECE(id)],
                    m_nodes[move.move_capture.destination_index],
                    [this, take_id](PieceObj&) {
                        do_take_animation(m_pieces[PIECE(take_id)], [](PieceObj& piece) {
                            piece.active = false;
                        });
                    },
                    !has_three_pieces(m_position.board, m_pieces[PIECE(id)])
                );
            }

            play_move_capture_move(move);

            break;
        }
    }
}

void NineMensMorrisBoard::timeout(Player player) {
    switch (player) {
        case Player::White:
            m_game_over = GameOver(
                GameOver::WinnerBlack,
                "White player has ran out of time."
            );
            break;
        case Player::Black:
            m_game_over = GameOver(
                GameOver::WinnerWhite,
                "Black player has ran out of time."
            );
            break;
    }
}

void NineMensMorrisBoard::resign(Player player) {
    switch (player) {
        case Player::White:
            m_game_over = GameOver(
                GameOver::WinnerBlack,
                "White player has resigned."
            );
            break;
        case Player::Black:
            m_game_over = GameOver(
                GameOver::WinnerWhite,
                "Black player has resigned."
            );
            break;
    }
}

void NineMensMorrisBoard::accept_draw_offer() {
    m_game_over = GameOver(
        GameOver::Draw,
        "Draw has been offered and accepted."
    );
}

NineMensMorrisBoard::Move NineMensMorrisBoard::move_from_string(const std::string& string) {
    const auto tokens {split(string, "-x")};

    switch (tokens.size()) {
        case 1: {
            const auto place_index {index_from_string(tokens[0])};

            return Move::create_place(place_index);
        }
        case 2: {
            if (string.find('-') == string.npos) {
                const auto place_index {index_from_string(tokens[0])};
                const auto capture_index {index_from_string(tokens[1])};

                return Move::create_place_capture(place_index, capture_index);
            } else {
                const auto source_index {index_from_string(tokens[0])};
                const auto destination_index {index_from_string(tokens[1])};

                return Move::create_move(source_index, destination_index);
            }
        }
        case 3: {
            const auto source_index {index_from_string(tokens[0])};
            const auto destination_index {index_from_string(tokens[1])};
            const auto capture_index {index_from_string(tokens[2])};

            return Move::create_move_capture(source_index, destination_index, capture_index);
        }
    }

    throw BoardError("Invalid move string");
}

std::string NineMensMorrisBoard::move_to_string(const Move& move) {
    std::string result;

    switch (move.type) {
        case MoveType::Place:
            result += index_to_string(move.place.place_index);
            break;
        case MoveType::PlaceCapture:
            result += index_to_string(move.place_capture.place_index);
            result += 'x';
            result += index_to_string(move.place_capture.capture_index);
            break;
        case MoveType::Move:
            result += index_to_string(move.move.source_index);
            result += '-';
            result += index_to_string(move.move.destination_index);
            break;
        case MoveType::MoveCapture:
            result += index_to_string(move.move_capture.source_index);
            result += '-';
            result += index_to_string(move.move_capture.destination_index);
            result += 'x';
            result += index_to_string(move.move_capture.capture_index);
            break;
    }

    return result;
}

NineMensMorrisBoard::Position NineMensMorrisBoard::position_from_string(const std::string& string) {
    const std::regex re {R"(^(w|b):(w|b)([a-g][1-7])?(,[a-g][1-7])*:(w|b)([a-g][1-7])?(,[a-g][1-7])*:[0-9]{1,3}$)"};

    if (!std::regex_match(string, re)) {
        throw BoardError("Invalid position string");
    }

    const auto tokens {split(string, ":")};

    assert(tokens.size() == 4);

    const auto player {parse_player(tokens[0])};
    const auto pieces1 {parse_pieces(tokens[1])};
    const auto pieces2 {parse_pieces(tokens[2])};
    int turns {};

    try {
        turns = std::stoi(tokens[3]);
    } catch (...) {
        throw BoardError("Invalid position string");
    }

    if (pieces1.second == pieces2.second) {
        throw BoardError("Invalid position string");
    }

    if (turns < 1) {
        throw BoardError("Invalid position string");
    }

    Position position;

    position.player = player;

    for (const int index : pieces1.first) {
        assert(index >= 0 && index < 24);

        position.board[index] = static_cast<Node>(pieces1.second);
    }

    for (const int index : pieces2.first) {
        assert(index >= 0 && index < 24);

        position.board[index] = static_cast<Node>(pieces2.second);
    }

    position.plies = (turns - 1) * 2 + static_cast<int>(player == Player::Black);

    return position;
}

std::string NineMensMorrisBoard::position_to_string(const Position& position) {
    std::string result;

    switch (position.player) {
        case Player::White:
            result += 'w';
            break;
        case Player::Black:
            result += 'b';
            break;
    }

    result += ":w";
    for (int i {0}; i < 24; i++) {
        if (position.board[i] != Node::White) {
            continue;
        }

        result += index_to_string(i);
        result += ',';
    }

    if (result.back() == ',') {
        result.pop_back();
    }

    result += ":b";
    for (int i {0}; i < 24; i++) {
        if (position.board[i] != Node::Black) {
            continue;
        }

        result += index_to_string(i);
        result += ',';
    }

    if (result.back() == ',') {
        result.pop_back();
    }

    result += ':';
    result += std::to_string(position.plies / 2 + 1);

    return result;
}

void NineMensMorrisBoard::debug_window() {
#ifndef SM_BUILD_DISTRIBUTION
    if (ImGui::Begin("Debug Board")) {
        ImGui::Text("player: %s", if_player_white("white", "black"));
        ImGui::Text("game_over: %s", m_game_over.to_string());
        ImGui::Text("plies: %d", m_position.plies);
        ImGui::Text("plies_no_advancement: %d", m_plies_no_advancement);
        ImGui::Text("positions: %lu", m_positions.size());
        ImGui::Text("capture_piece: %s", m_capture_piece ? "true" : "false");
        ImGui::Text("select_id: %d", m_select_id);
        ImGui::Text("legal_moves: %lu", m_legal_moves.size());
        ImGui::Text("click_id: %d", m_click_id);
        ImGui::Text("hover_id: %d", m_hover_id);
    }

    ImGui::End();
#endif
}

void NineMensMorrisBoard::initialize_objects(const NodeRenderables& nodes, const PieceRenderables& white_pieces, const PieceRenderables& black_pieces) {
    assert(white_pieces.size() == black_pieces.size());
    assert(white_pieces.size() + black_pieces.size() == NINE || white_pieces.size() + black_pieces.size() == TWELVE);

    for (int i {0}; i < NODES; i++) {
        m_nodes[i] = NodeObj(i, nodes[i], NODE_POSITIONS[i]);
    }

    // Thus it always contains the correct number of pieces
    m_pieces.resize(white_pieces.size() + black_pieces.size());

    // Offset pieces' IDs, so that they are different from nodes' IDs

    for (int i {0}; i < m_pieces.size() / 2; i++) {
        m_pieces[i] = PieceObj(
            i + NODES,
            white_pieces[i],
            glm::vec3(
                -3.0f,
                PIECE_Y_POSITION_AIR_INITIAL,
                static_cast<float>(i) * 0.5f - (m_pieces.size() == NINE ? 2.0f : 2.75f)
            ),
            PieceType::White
        );
    }

    for (int i {m_pieces.size() / 2}; i < m_pieces.size(); i++) {
        m_pieces[i] = PieceObj(
            i + NODES,
            black_pieces[i - m_pieces.size() / 2],
            glm::vec3(
                3.0f,
                PIECE_Y_POSITION_AIR_INITIAL,
                static_cast<float>(i - m_pieces.size() / 2) * -0.5f + (m_pieces.size() == NINE ? 2.0f : 2.75f)
            ),
            PieceType::Black
        );
    }
}

void NineMensMorrisBoard::initialize_objects() {
    NodeRenderables nodes;
    PieceRenderables white_pieces;
    PieceRenderables black_pieces;

    for (int i {0}; i < NODES; i++) {
        nodes.push_back(m_nodes[i].get_renderable());
    }

    for (int i {0}; i < m_pieces.size() / 2; i++) {
        white_pieces.push_back(m_pieces[i].get_renderable());
    }

    for (int i {m_pieces.size() / 2}; i < m_pieces.size(); i++) {
        black_pieces.push_back(m_pieces[i].get_renderable());
    }

    initialize_objects(nodes, white_pieces, black_pieces);
}

void NineMensMorrisBoard::update_nodes_highlight(std::function<bool()>&& highlight, bool enabled) {
    if (!enabled) {
        for (NodeObj& node : m_nodes) {
            node.set_highlighted(false);
        }

        return;
    }

    if (!highlight()) {
        for (NodeObj& node : m_nodes) {
            node.set_highlighted(false);
        }

        return;
    }

    for (NodeObj& node : m_nodes) {
        node.set_highlighted(node.get_id() == m_hover_id);
    }
}

void NineMensMorrisBoard::update_pieces_highlight(std::function<bool(const PieceObj&)>&& highlight, bool enabled) {
    if (!enabled) {
        for (PieceObj& piece : m_pieces) {
            piece.get_renderable().get_material()->flags &= ~sm::Material::Outline;
        }

        return;
    }

    for (PieceObj& piece : m_pieces) {
        if (piece.get_id() == m_hover_id && highlight(piece)) {
            piece.get_renderable().get_material()->flags |= sm::Material::Outline;
            piece.get_renderable().outline.color = ORANGE;
        } else {
            piece.get_renderable().get_material()->flags &= ~sm::Material::Outline;
        }
    }

    // Override, if the piece is actually selected
    if (m_select_id != -1) {
        const int piece_id {m_nodes[m_select_id].piece_id};

        if (piece_id != -1) {
            m_pieces[piece_id - NODES].get_renderable().get_material()->flags |= sm::Material::Outline;
            m_pieces[piece_id - NODES].get_renderable().outline.color = RED;
        }
    }
}

void NineMensMorrisBoard::update_nodes(sm::Ctx& ctx) {
    for (NodeObj& node : m_nodes) {
        node.update(ctx);
    }
}

void NineMensMorrisBoard::update_pieces(sm::Ctx& ctx) {
    for (PieceObj& piece : m_pieces) {
        piece.update(ctx);
    }
}

void NineMensMorrisBoard::do_place_animation(PieceObj& piece, const NodeObj& node, PieceObj::OnFinish&& on_finish) {
    const glm::vec3 origin {piece.get_renderable().transform.position};
    const glm::vec3 target0 {piece.get_renderable().transform.position.x, PIECE_Y_POSITION_AIR_MOVE, piece.get_renderable().transform.position.z};
    const glm::vec3 target1 {node.get_renderable().transform.position.x, PIECE_Y_POSITION_AIR_MOVE, node.get_renderable().transform.position.z};
    const glm::vec3 target {node.get_renderable().transform.position.x, PIECE_Y_POSITION_BOARD, node.get_renderable().transform.position.z};

    piece.move_three_step(origin, target0, target1, target, std::move(on_finish));
}

void NineMensMorrisBoard::do_move_animation(PieceObj& piece, const NodeObj& node, PieceObj::OnFinish&& on_finish, bool direct) {
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

void NineMensMorrisBoard::do_take_animation(PieceObj& piece, PieceObj::OnFinish&& on_finish) {
    const glm::vec3 origin {piece.get_renderable().transform.position};
    const glm::vec3 target {piece.get_renderable().transform.position.x, PIECE_Y_POSITION_AIR_TAKE, piece.get_renderable().transform.position.z};

    piece.move_direct(origin, target, std::move(on_finish));
}

void NineMensMorrisBoard::select(int index) {
    if (m_select_id == -1) {
        if (m_position.board[index] == static_cast<Node>(m_position.player)) {
            m_select_id = index;
        }
    } else {
        if (index == m_select_id) {
            m_select_id = -1;
        } else if (m_position.board[index] == static_cast<Node>(m_position.player)) {
            m_select_id = index;
        }
    }
}

void NineMensMorrisBoard::try_place(int place_index) {
    {
        const auto iter {std::find_if(m_legal_moves.cbegin(), m_legal_moves.cend(), [=](const Move& move) {
            return move.type == MoveType::Place && move.place.place_index == place_index;
        })};

        if (iter != m_legal_moves.cend()) {
            const int id {new_piece_to_place(static_cast<PieceType>(m_position.player))};

            m_pieces[PIECE(id)].node_id = place_index;
            m_nodes[place_index].piece_id = id;

            do_place_animation(m_pieces[PIECE(id)], m_nodes[place_index], [](PieceObj&) {});

            const Move move {*iter};
            play_place_move(move);

            return;
        }
    }

    m_candidate_moves.clear();

    std::copy_if(m_legal_moves.cbegin(), m_legal_moves.cend(), std::back_inserter(m_candidate_moves), [=](const Move& move) {
        return move.type == MoveType::PlaceCapture && move.place_capture.place_index == place_index;
    });

    if (!m_candidate_moves.empty()) {
        const int id {new_piece_to_place(static_cast<PieceType>(m_position.player))};

        m_pieces[PIECE(id)].node_id = place_index;
        m_nodes[place_index].piece_id = id;

        do_place_animation(m_pieces[PIECE(id)], m_nodes[place_index], [](PieceObj&) {});

        m_capture_piece = true;
    }
}

void NineMensMorrisBoard::try_move(int source_index, int destination_index) {
    {
        const auto iter {std::find_if(m_legal_moves.cbegin(), m_legal_moves.cend(), [=](const Move& move) {
            return (
                move.type == MoveType::Move &&
                move.move.source_index == source_index &&
                move.move.destination_index == destination_index
            );
        })};

        if (iter != m_legal_moves.cend()) {
            m_pieces[PIECE(m_nodes[source_index].piece_id)].node_id = destination_index;
            m_nodes[destination_index].piece_id = m_nodes[source_index].piece_id;

            const int id {std::exchange(m_nodes[source_index].piece_id, -1)};

            do_move_animation(
                m_pieces[PIECE(id)],
                m_nodes[destination_index],
                [](PieceObj&) {},
                !has_three_pieces(m_position.board, m_pieces[PIECE(id)])
            );

            const Move move {*iter};
            play_move_move(move);

            return;
        }
    }

    m_candidate_moves.clear();

    std::copy_if(m_legal_moves.cbegin(), m_legal_moves.cend(), std::back_inserter(m_candidate_moves), [=](const Move& move) {
        return (
            move.type == MoveType::MoveCapture &&
            move.move_capture.source_index == source_index &&
            move.move_capture.destination_index == destination_index
        );
    });

    if (!m_candidate_moves.empty()) {
        m_pieces[PIECE(m_nodes[source_index].piece_id)].node_id = destination_index;
        m_nodes[destination_index].piece_id = m_nodes[source_index].piece_id;

        const int id {std::exchange(m_nodes[source_index].piece_id, -1)};

        do_move_animation(
            m_pieces[PIECE(id)],
            m_nodes[destination_index],
            [](PieceObj&) {},
            !has_three_pieces(m_position.board, m_pieces[PIECE(id)])
        );

        m_capture_piece = true;
    }
}

void NineMensMorrisBoard::try_capture(int capture_index) {
    const auto iter {std::find_if(m_candidate_moves.cbegin(), m_candidate_moves.cend(), [=](const Move& move) -> bool {
        switch (move.type) {
            case MoveType::PlaceCapture:
                return move.place_capture.capture_index == capture_index;
            case MoveType::MoveCapture:
                return move.move_capture.capture_index == capture_index;
            default:
                assert(false);
                break;
        }

        return {};
    })};

    if (iter == m_candidate_moves.cend()) {
        return;
    }

    switch (iter->type) {
        case MoveType::PlaceCapture: {
            m_pieces[PIECE(m_nodes[capture_index].piece_id)].node_id = -1;

            const int id {std::exchange(m_nodes[capture_index].piece_id, -1)};

            m_pieces[PIECE(id)].to_remove = true;

            do_take_animation(m_pieces[PIECE(id)], [](PieceObj& piece) {
                piece.active = false;
            });

            const Move move {*iter};
            play_place_capture_move(move);

            break;
        }
        case MoveType::MoveCapture: {
            m_pieces[PIECE(m_nodes[capture_index].piece_id)].node_id = -1;

            const int id {std::exchange(m_nodes[capture_index].piece_id, -1)};

            m_pieces[PIECE(id)].to_remove = true;

            do_take_animation(m_pieces[PIECE(id)], [](PieceObj& piece) {
                piece.active = false;
            });

            const Move move {*iter};
            play_move_capture_move(move);

            break;
        }
        default:
            assert(false);
            break;
    }
}

void NineMensMorrisBoard::play_place_move(const Move& move) {
    assert(move.type == MoveType::Place);
    assert(m_position.board[move.place.place_index] == Node::None);

    m_position.board[move.place.place_index] = static_cast<Node>(m_position.player);

    finish_turn();
    check_legal_moves();

    if (m_enable_move_callback) {
        m_move_callback(move);
    }
}

void NineMensMorrisBoard::play_place_capture_move(const Move& move) {
    assert(move.type == MoveType::PlaceCapture);
    assert(m_position.board[move.place_capture.place_index] == Node::None);
    assert(m_position.board[move.place_capture.capture_index] != Node::None);

    m_position.board[move.place_capture.place_index] = static_cast<Node>(m_position.player);
    m_position.board[move.place_capture.capture_index] = Node::None;

    finish_turn();
    check_material();
    check_legal_moves();

    if (m_enable_move_callback) {
        m_move_callback(move);
    }
}

void NineMensMorrisBoard::play_move_move(const Move& move) {
    assert(move.type == MoveType::Move);
    assert(m_position.board[move.move.source_index] != Node::None);
    assert(m_position.board[move.move.destination_index] == Node::None);

    std::swap(m_position.board[move.move.source_index], m_position.board[move.move.destination_index]);

    finish_turn(false);
    check_legal_moves();
    check_threefold_repetition();
    check_fifty_move_rule();

    if (m_enable_move_callback) {
        m_move_callback(move);
    }
}

void NineMensMorrisBoard::play_move_capture_move(const Move& move) {
    assert(move.type == MoveType::MoveCapture);
    assert(m_position.board[move.move_capture.source_index] != Node::None);
    assert(m_position.board[move.move_capture.destination_index] == Node::None);
    assert(m_position.board[move.move_capture.capture_index] != Node::None);

    std::swap(m_position.board[move.move_capture.source_index], m_position.board[move.move_capture.destination_index]);
    m_position.board[move.move_capture.capture_index] = Node::None;

    finish_turn();
    check_material();
    check_legal_moves();

    if (m_enable_move_callback) {
        m_move_callback(move);
    }
}

void NineMensMorrisBoard::finish_turn(bool advancement) {
    m_position.player = opponent(m_position.player);
    m_position.plies++;
    m_legal_moves = generate_moves();

    if (advancement) {
        m_plies_no_advancement = 0;
        m_positions.clear();
    } else {
        m_plies_no_advancement++;
    }

    // Store the current position anyway
    m_positions.push_back(m_position);

    m_capture_piece = false;
    m_select_id = -1;
}

void NineMensMorrisBoard::check_material() {
    if (m_game_over != GameOver::None) {
        return;
    }

    if (m_position.plies < m_pieces.size()) {
        return;
    }

    if (count_pieces(m_position.board, m_position.player) < 3) {
        m_game_over = GameOver(
            if_player_white(GameOver::WinnerBlack, GameOver::WinnerWhite),
            format("%s player cannot make any more mills.", if_player_white("White", "Black"))
        );
    }
}

void NineMensMorrisBoard::check_legal_moves() {
    if (m_game_over != GameOver::None) {
        return;
    }

    if (m_legal_moves.empty()) {
        m_game_over = GameOver(
            if_player_white(GameOver::WinnerBlack, GameOver::WinnerWhite),
            format("%s player has no more legal moves to play.", if_player_white("White", "Black"))
        );
    }
}

void NineMensMorrisBoard::check_fifty_move_rule() {
    if (m_game_over != GameOver::None) {
        return;
    }

    if (m_plies_no_advancement == 100) {
        m_game_over = GameOver(
            GameOver::Draw,
            "Fifty moves have been played without a mill."
        );
    }
}

void NineMensMorrisBoard::check_threefold_repetition() {
    if (m_game_over != GameOver::None) {
        return;
    }

    const auto count {std::count_if(m_positions.cbegin(), m_positions.cend(), [this](const auto& position) {
        return position.eq(m_position, m_pieces.size());
    })};

    assert(count >= 1);

    if (count == 3) {
        m_game_over = GameOver(
            GameOver::Draw,
            "The same position has happened three times."
        );
    }
}

int NineMensMorrisBoard::new_piece_to_place(PieceType type) {
    for (const PieceObj& piece : m_pieces) {
        if (piece.get_type() == type && piece.active && !piece.to_remove && piece.node_id == -1) {
            return piece.get_id();
        }
    }

    assert(false);
    return {};
}

bool NineMensMorrisBoard::is_node_id(int id) {
    return id >= 0 && id <= NODES - 1;
}

bool NineMensMorrisBoard::is_piece_id(int id) {
    return id >= NODES && id <= NODES + PIECES - 1;
}

bool NineMensMorrisBoard::has_three_pieces(const Board& board, const PieceObj& piece) {
    return count_pieces(board, static_cast<Player>(piece.get_type())) == 3;
}

std::vector<NineMensMorrisBoard::Move> NineMensMorrisBoard::generate_moves() const {
    Board local_board {m_position.board};

    if (m_position.plies < m_pieces.size()) {
        return generate_moves_phase1(local_board, m_position.player, m_pieces.size());
    } else {
        if (count_pieces(local_board, m_position.player) == 3) {
            return generate_moves_phase3(local_board, m_position.player, m_pieces.size());
        } else {
            return generate_moves_phase2(local_board, m_position.player, m_pieces.size());
        }
    }
}

std::vector<NineMensMorrisBoard::Move> NineMensMorrisBoard::generate_moves_phase1(Board& board, Player player, int p) {
    std::vector<Move> moves;

    for (int i {0}; i < 24; i++) {
        if (board[i] != Node::None) {
            continue;
        }

        make_place_move(board, player, i);

        if (is_mill(board, player, i, p)) {
            const Player opponent_player {opponent(player)};
            const bool all_in_mills {all_pieces_in_mills(board, opponent_player, p)};

            for (int j {0}; j < 24; j++) {
                if (board[j] != static_cast<Node>(opponent_player)) {
                    continue;
                }

                if (is_mill(board, opponent_player, j, p) && !all_in_mills) {
                    continue;
                }

                moves.push_back(Move::create_place_capture(i, j));
            }
        } else {
            moves.push_back(Move::create_place(i));
        }

        unmake_place_move(board, i);
    }

    return moves;
}

std::vector<NineMensMorrisBoard::Move> NineMensMorrisBoard::generate_moves_phase2(Board& board, Player player, int p) {
    std::vector<Move> moves;

    for (int i {0}; i < 24; i++) {
        if (board[i] != static_cast<Node>(player)) {
            continue;
        }

        const auto free_positions {neighbor_free_positions(board, i, p)};

        for (int j {0}; j < static_cast<int>(free_positions.size()); j++) {
            make_move_move(board, i, free_positions[j]);

            if (is_mill(board, player, free_positions[j], p)) {
                const Player opponent_player {opponent(player)};
                const bool all_in_mills {all_pieces_in_mills(board, opponent_player, p)};

                for (int k {0}; k < 24; k++) {
                    if (board[k] != static_cast<Node>(opponent_player)) {
                        continue;
                    }

                    if (is_mill(board, opponent_player, k, p) && !all_in_mills) {
                        continue;
                    }

                    moves.push_back(Move::create_move_capture(i, free_positions[j], k));
                }
            } else {
                moves.push_back(Move::create_move(i, free_positions[j]));
            }

            unmake_move_move(board, i, free_positions[j]);
        }
    }

    return moves;
}

std::vector<NineMensMorrisBoard::Move> NineMensMorrisBoard::generate_moves_phase3(Board& board, Player player, int p) {
    std::vector<Move> moves;

    for (int i {0}; i < 24; i++) {
        if (board[i] != static_cast<Node>(player)) {
            continue;
        }

        for (int j {0}; j < 24; j++) {
            if (board[j] != Node::None) {
                continue;
            }

            make_move_move(board, i, j);

            if (is_mill(board, player, j, p)) {
                const Player opponent_player {opponent(player)};
                const bool all_in_mills {all_pieces_in_mills(board, opponent_player, p)};

                for (int k {0}; k < 24; k++) {
                    if (board[k] != static_cast<Node>(opponent_player)) {
                        continue;
                    }

                    if (is_mill(board, opponent_player, k, p) && !all_in_mills) {
                        continue;
                    }

                    moves.push_back(Move::create_move_capture(i, j, k));
                }
            } else {
                moves.push_back(Move::create_move(i, j));
            }

            unmake_move_move(board, i, j);
        }
    }

    return moves;
}

void NineMensMorrisBoard::make_place_move(Board& board, Player player, int place_index) {
    assert(board[place_index] == Node::None);

    board[place_index] = static_cast<Node>(player);
}

void NineMensMorrisBoard::unmake_place_move(Board& board, int place_index) {
    assert(board[place_index] != Node::None);

    board[place_index] = Node::None;
}

void NineMensMorrisBoard::make_move_move(Board& board, int source_index, int destination_index) {
    assert(board[source_index] != Node::None);
    assert(board[destination_index] == Node::None);

    std::swap(board[source_index], board[destination_index]);
}

void NineMensMorrisBoard::unmake_move_move(Board& board, int source_index, int destination_index) {
    assert(board[source_index] == Node::None);
    assert(board[destination_index] != Node::None);

    std::swap(board[source_index], board[destination_index]);
}

static bool mill(const NineMensMorrisBoard::Board& board, NineMensMorrisBoard::Node node, int index1, int index2) {
    return board[index1] == node && board[index2] == node;
}

bool NineMensMorrisBoard::is_mill(const Board& board, Player player, int index, int p) {
    if (p == NINE) {
        return is_mill9(board, player, index);
    } else {
        return is_mill12(board, player, index);
    }
}

bool NineMensMorrisBoard::is_mill9(const Board& board, Player player, int index) {
    const Node node {static_cast<Node>(player)};

    assert(board[index] == node);

    switch (index) {
        case 0: return mill(board, node, 1, 2) || mill(board, node, 9, 21);
        case 1: return mill(board, node, 0, 2) || mill(board, node, 4, 7);
        case 2: return mill(board, node, 0, 1) || mill(board, node, 14, 23);
        case 3: return mill(board, node, 4, 5) || mill(board, node, 10, 18);
        case 4: return mill(board, node, 3, 5) || mill(board, node, 1, 7);
        case 5: return mill(board, node, 3, 4) || mill(board, node, 13, 20);
        case 6: return mill(board, node, 7, 8) || mill(board, node, 11, 15);
        case 7: return mill(board, node, 6, 8) || mill(board, node, 1, 4);
        case 8: return mill(board, node, 6, 7) || mill(board, node, 12, 17);
        case 9: return mill(board, node, 0, 21) || mill(board, node, 10, 11);
        case 10: return mill(board, node, 9, 11) || mill(board, node, 3, 18);
        case 11: return mill(board, node, 9, 10) || mill(board, node, 6, 15);
        case 12: return mill(board, node, 13, 14) || mill(board, node, 8, 17);
        case 13: return mill(board, node, 12, 14) || mill(board, node, 5, 20);
        case 14: return mill(board, node, 12, 13) || mill(board, node, 2, 23);
        case 15: return mill(board, node, 16, 17) || mill(board, node, 6, 11);
        case 16: return mill(board, node, 15, 17) || mill(board, node, 19, 22);
        case 17: return mill(board, node, 15, 16) || mill(board, node, 8, 12);
        case 18: return mill(board, node, 19, 20) || mill(board, node, 3, 10);
        case 19: return mill(board, node, 18, 20) || mill(board, node, 16, 22);
        case 20: return mill(board, node, 18, 19) || mill(board, node, 5, 13);
        case 21: return mill(board, node, 22, 23) || mill(board, node, 0, 9);
        case 22: return mill(board, node, 21, 23) || mill(board, node, 16, 19);
        case 23: return mill(board, node, 21, 22) || mill(board, node, 2, 14);
    }

    assert(false);
    return {};
}

bool NineMensMorrisBoard::is_mill12(const Board& board, Player player, int index) {
    const Node node {static_cast<Node>(player)};

    assert(board[index] == node);

    switch (index) {
        case 0: return mill(board, node, 1, 2) || mill(board, node, 9, 21) || mill(board, node, 3, 6);
        case 1: return mill(board, node, 0, 2) || mill(board, node, 4, 7);
        case 2: return mill(board, node, 0, 1) || mill(board, node, 14, 23) || mill(board, node, 5, 8);
        case 3: return mill(board, node, 4, 5) || mill(board, node, 10, 18) || mill(board, node, 0, 6);
        case 4: return mill(board, node, 3, 5) || mill(board, node, 1, 7);
        case 5: return mill(board, node, 3, 4) || mill(board, node, 13, 20) || mill(board, node, 2, 8);
        case 6: return mill(board, node, 7, 8) || mill(board, node, 11, 15) || mill(board, node, 0, 3);
        case 7: return mill(board, node, 6, 8) || mill(board, node, 1, 4);
        case 8: return mill(board, node, 6, 7) || mill(board, node, 12, 17) || mill(board, node, 2, 5);
        case 9: return mill(board, node, 0, 21) || mill(board, node, 10, 11);
        case 10: return mill(board, node, 9, 11) || mill(board, node, 3, 18);
        case 11: return mill(board, node, 9, 10) || mill(board, node, 6, 15);
        case 12: return mill(board, node, 13, 14) || mill(board, node, 8, 17);
        case 13: return mill(board, node, 12, 14) || mill(board, node, 5, 20);
        case 14: return mill(board, node, 12, 13) || mill(board, node, 2, 23);
        case 15: return mill(board, node, 16, 17) || mill(board, node, 6, 11) || mill(board, node, 18, 21);
        case 16: return mill(board, node, 15, 17) || mill(board, node, 19, 22);
        case 17: return mill(board, node, 15, 16) || mill(board, node, 8, 12) || mill(board, node, 20, 23);
        case 18: return mill(board, node, 19, 20) || mill(board, node, 3, 10) || mill(board, node, 15, 21);
        case 19: return mill(board, node, 18, 20) || mill(board, node, 16, 22);
        case 20: return mill(board, node, 18, 19) || mill(board, node, 5, 13) || mill(board, node, 17, 23);
        case 21: return mill(board, node, 22, 23) || mill(board, node, 0, 9) || mill(board, node, 15, 18);
        case 22: return mill(board, node, 21, 23) || mill(board, node, 16, 19);
        case 23: return mill(board, node, 21, 22) || mill(board, node, 2, 14) || mill(board, node, 17, 20);
    }

    assert(false);
    return {};
}

bool NineMensMorrisBoard::all_pieces_in_mills(const Board& board, Player player, int p) {
    for (int i {0}; i < 24; i++) {
        if (board[i] != static_cast<Node>(player)) {
            continue;
        }

        if (!is_mill(board, player, i, p)) {
            return false;
        }
    }

    return true;
}

static void neighbor(const NineMensMorrisBoard::Board& board, std::vector<int>& result, int index) {
    if (board[index] == NineMensMorrisBoard::Node::None) {
        result.push_back(index);
    }
}

std::vector<int> NineMensMorrisBoard::neighbor_free_positions(const Board& board, int index, int p) {
    if (p == NINE) {
        return neighbor_free_positions9(board, index);
    } else {
        return neighbor_free_positions12(board, index);
    }
}

std::vector<int> NineMensMorrisBoard::neighbor_free_positions9(const Board& board, int index) {
    std::vector<int> result;
    result.reserve(4);
    switch (index) {
        case 0:
            neighbor(board, result, 1);
            neighbor(board, result, 9);
            break;
        case 1:
            neighbor(board, result, 0);
            neighbor(board, result, 2);
            neighbor(board, result, 4);
            break;
        case 2:
            neighbor(board, result, 1);
            neighbor(board, result, 14);
            break;
        case 3:
            neighbor(board, result, 4);
            neighbor(board, result, 10);
            break;
        case 4:
            neighbor(board, result, 1);
            neighbor(board, result, 3);
            neighbor(board, result, 5);
            neighbor(board, result, 7);
            break;
        case 5:
            neighbor(board, result, 4);
            neighbor(board, result, 13);
            break;
        case 6:
            neighbor(board, result, 7);
            neighbor(board, result, 11);
            break;
        case 7:
            neighbor(board, result, 4);
            neighbor(board, result, 6);
            neighbor(board, result, 8);
            break;
        case 8:
            neighbor(board, result, 7);
            neighbor(board, result, 12);
            break;
        case 9:
            neighbor(board, result, 0);
            neighbor(board, result, 10);
            neighbor(board, result, 21);
            break;
        case 10:
            neighbor(board, result, 3);
            neighbor(board, result, 9);
            neighbor(board, result, 11);
            neighbor(board, result, 18);
            break;
        case 11:
            neighbor(board, result, 6);
            neighbor(board, result, 10);
            neighbor(board, result, 15);
            break;
        case 12:
            neighbor(board, result, 8);
            neighbor(board, result, 13);
            neighbor(board, result, 17);
            break;
        case 13:
            neighbor(board, result, 5);
            neighbor(board, result, 12);
            neighbor(board, result, 14);
            neighbor(board, result, 20);
            break;
        case 14:
            neighbor(board, result, 2);
            neighbor(board, result, 13);
            neighbor(board, result, 23);
            break;
        case 15:
            neighbor(board, result, 11);
            neighbor(board, result, 16);
            break;
        case 16:
            neighbor(board, result, 15);
            neighbor(board, result, 17);
            neighbor(board, result, 19);
            break;
        case 17:
            neighbor(board, result, 12);
            neighbor(board, result, 16);
            break;
        case 18:
            neighbor(board, result, 10);
            neighbor(board, result, 19);
            break;
        case 19:
            neighbor(board, result, 16);
            neighbor(board, result, 18);
            neighbor(board, result, 20);
            neighbor(board, result, 22);
            break;
        case 20:
            neighbor(board, result, 13);
            neighbor(board, result, 19);
            break;
        case 21:
            neighbor(board, result, 9);
            neighbor(board, result, 22);
            break;
        case 22:
            neighbor(board, result, 19);
            neighbor(board, result, 21);
            neighbor(board, result, 23);
            break;
        case 23:
            neighbor(board, result, 14);
            neighbor(board, result, 22);
            break;
    }
    return result;
}

std::vector<int> NineMensMorrisBoard::neighbor_free_positions12(const Board& board, int index) {
    std::vector<int> result;
    result.reserve(4);

    switch (index) {
        case 0:
            neighbor(board, result, 1);
            neighbor(board, result, 9);
            neighbor(board, result, 3);
            break;
        case 1:
            neighbor(board, result, 0);
            neighbor(board, result, 2);
            neighbor(board, result, 4);
            break;
        case 2:
            neighbor(board, result, 1);
            neighbor(board, result, 14);
            neighbor(board, result, 5);
            break;
        case 3:
            neighbor(board, result, 4);
            neighbor(board, result, 10);
            neighbor(board, result, 0);
            neighbor(board, result, 6);
            break;
        case 4:
            neighbor(board, result, 1);
            neighbor(board, result, 3);
            neighbor(board, result, 5);
            neighbor(board, result, 7);
            break;
        case 5:
            neighbor(board, result, 4);
            neighbor(board, result, 13);
            neighbor(board, result, 2);
            neighbor(board, result, 8);
            break;
        case 6:
            neighbor(board, result, 7);
            neighbor(board, result, 11);
            neighbor(board, result, 3);
            break;
        case 7:
            neighbor(board, result, 4);
            neighbor(board, result, 6);
            neighbor(board, result, 8);
            break;
        case 8:
            neighbor(board, result, 7);
            neighbor(board, result, 12);
            neighbor(board, result, 5);
            break;
        case 9:
            neighbor(board, result, 0);
            neighbor(board, result, 10);
            neighbor(board, result, 21);
            break;
        case 10:
            neighbor(board, result, 3);
            neighbor(board, result, 9);
            neighbor(board, result, 11);
            neighbor(board, result, 18);
            break;
        case 11:
            neighbor(board, result, 6);
            neighbor(board, result, 10);
            neighbor(board, result, 15);
            break;
        case 12:
            neighbor(board, result, 8);
            neighbor(board, result, 13);
            neighbor(board, result, 17);
            break;
        case 13:
            neighbor(board, result, 5);
            neighbor(board, result, 12);
            neighbor(board, result, 14);
            neighbor(board, result, 20);
            break;
        case 14:
            neighbor(board, result, 2);
            neighbor(board, result, 13);
            neighbor(board, result, 23);
            break;
        case 15:
            neighbor(board, result, 11);
            neighbor(board, result, 16);
            neighbor(board, result, 18);
            break;
        case 16:
            neighbor(board, result, 15);
            neighbor(board, result, 17);
            neighbor(board, result, 19);
            break;
        case 17:
            neighbor(board, result, 12);
            neighbor(board, result, 16);
            neighbor(board, result, 20);
            break;
        case 18:
            neighbor(board, result, 10);
            neighbor(board, result, 19);
            neighbor(board, result, 15);
            neighbor(board, result, 21);
            break;
        case 19:
            neighbor(board, result, 16);
            neighbor(board, result, 18);
            neighbor(board, result, 20);
            neighbor(board, result, 22);
            break;
        case 20:
            neighbor(board, result, 13);
            neighbor(board, result, 19);
            neighbor(board, result, 17);
            neighbor(board, result, 23);
            break;
        case 21:
            neighbor(board, result, 9);
            neighbor(board, result, 22);
            neighbor(board, result, 18);
            break;
        case 22:
            neighbor(board, result, 19);
            neighbor(board, result, 21);
            neighbor(board, result, 23);
            break;
        case 23:
            neighbor(board, result, 14);
            neighbor(board, result, 22);
            neighbor(board, result, 20);
            break;
    }

    return result;
}

int NineMensMorrisBoard::count_pieces(const Board& board, Player player) {
    int result {0};

    for (const Node node : board) {
        result += static_cast<int>(node == static_cast<Node>(player));
    }

    return result;
}

NineMensMorrisBoard::Player NineMensMorrisBoard::opponent(Player player) {
    if (player == Player::White) {
        return Player::Black;
    } else {
        return Player::White;
    }
}
