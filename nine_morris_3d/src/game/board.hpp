#pragma once

#include <functional>
#include <vector>
#include <utility>
#include <string>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/node.hpp"
#include "game/piece.hpp"

inline constexpr float NODE_Y_POSITION {0.063f};
inline const glm::vec3 NODE_POSITIONS[24] {
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

inline const glm::vec3 RED {0.8f, 0.16f, 0.3f};
inline const glm::vec3 ORANGE {0.96f, 0.58f, 0.15f};

inline constexpr float PIECE_Y_POSITION_AIR_INITIAL {0.5f};
inline constexpr float PIECE_Y_POSITION_BOARD {0.135f};

enum class Player {
    White = 1,
    Black = 2
};

enum class Piece {
    None,
    White,
    Black
};

class GameOver {
public:
    enum Type {
        None,
        WinnerWhite,
        WinnerBlack,
        TieBetweenBothPlayers
    };

    GameOver() = default;
    GameOver(Type type, const std::string& reason)
        : m_type(type), m_reason(reason) {}
    GameOver(Player player, const std::string& reason)
        : m_type(static_cast<Type>(player)), m_reason(reason) {}

    operator int() const { return m_type; }
    operator std::string() const { return m_reason; }
private:
    Type m_type {None};
    std::string m_reason;
};

template<typename Board>
struct Position {
    Board board {};
    Player turn {};

    bool operator==(const Position& other) const {
        return board == other.board && turn == other.turn;
    }
};

class BoardObj {
public:
    BoardObj() = default;
    BoardObj(const sm::Renderable& board, const sm::Renderable& board_paint);
    virtual ~BoardObj() = default;

    BoardObj(const BoardObj&) = default;
    BoardObj& operator=(const BoardObj&) = default;
    BoardObj(BoardObj&&) = default;
    BoardObj& operator=(BoardObj&&) = default;

    virtual const GameOver& get_game_over() const = 0;
    virtual Player get_turn() const = 0;
    virtual int node_count() = 0;
    virtual int piece_count() = 0;

    sm::Renderable& get_renderable() { return m_renderable; }
    sm::Renderable& get_paint_renderable() { return m_paint_renderable; }
    std::vector<NodeObj>& get_nodes() { return m_nodes; }
    std::vector<PieceObj>& get_pieces() { return m_pieces; }

    void set_board_paint_renderable(const sm::Renderable& renderable);
    void set_renderables(
        const sm::Renderable& board,
        const sm::Renderable& board_paint,
        const std::vector<sm::Renderable>& nodes,
        const std::vector<sm::Renderable>& white_pieces,
        const std::vector<sm::Renderable>& black_pieces
    );
protected:
    void user_click_press(GameOver game_over);
    void user_click_release(GameOver game_over, std::function<void()>&& callback);
    void update_hovered_id(glm::vec3 ray, glm::vec3 camera, std::function<std::vector<std::pair<int, sm::Renderable>>()>&& get_renderables);

    void initialize_nodes(const std::vector<sm::Renderable>& renderables);
    void initialize_piece_in_air(
        const std::vector<sm::Renderable>& renderables,
        int index,
        int renderable_index,
        float x,
        float y,
        PieceType piece
    );

    void update_movement();
    void update_nodes_highlight(GameOver game_over, std::function<bool()>&& highlight);
    void update_pieces_highlight(GameOver game_over, int user_selected_index, std::function<bool(const PieceObj&)>&& highlight);
    void update_nodes(sm::Ctx& ctx);
    void update_pieces(sm::Ctx& ctx);

    static const char* turn_string(Player turn);
    static const char* game_over_string(GameOver game_over);
    static Player opponent(Player player);
    static std::string format(const char* format, ...);

    static void do_place_animation(PieceObj& piece, const NodeObj& node, std::function<void()>&& on_finish);
    static void do_move_animation(PieceObj& piece, const NodeObj& node, std::function<void()>&& on_finish, bool direct);
    static void do_take_animation(PieceObj& piece, std::function<void()>&& on_finish);

    template<typename T>
    T if_player_white(Player player, T&& value_if_white, T&& value_if_black) {
        return player == Player::White ? value_if_white : value_if_black;
    }

    template<typename Board>
    void initialize_piece_on_board(
        Board& board,
        const std::vector<sm::Renderable>& renderables,
        int index,
        int node_index,
        int renderable_index,
        PieceType piece
    ) {
        // Offset pieces' IDs, so that they are different from nodes' IDs

        m_pieces[index] = PieceObj(
            index + static_cast<int>(m_nodes.size()),
            glm::vec3(NODE_POSITIONS[node_index].x, PIECE_Y_POSITION_BOARD, NODE_POSITIONS[node_index].z),
            renderables[renderable_index],
            piece
        );

        m_pieces[index].node_id = node_index;
        m_nodes[node_index].piece_id = m_pieces[index].get_id();

        board[node_index] = static_cast<Piece>(piece);
    }

    template<typename Board>
    static unsigned int count_pieces(const Board& board, Player player) {
        unsigned int result {0};

        for (const Piece piece : board) {
            result += static_cast<unsigned int>(piece == static_cast<Piece>(player));
        }

        return result;
    }

#ifdef __GNUG__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wparentheses"
#endif

    template<typename Board>
    static bool is_mill(const Board& board, Player player, int index) {
        const Piece piece {static_cast<Piece>(player)};

        assert(board[index] == piece);

        switch (index) {
            case 0: return board[1] == piece && board[2] == piece || board[9] == piece && board[21] == piece;
            case 1: return board[0] == piece && board[2] == piece || board[4] == piece && board[7] == piece;
            case 2: return board[0] == piece && board[1] == piece || board[14] == piece && board[23] == piece;
            case 3: return board[4] == piece && board[5] == piece || board[10] == piece && board[18] == piece;
            case 4: return board[3] == piece && board[5] == piece || board[1] == piece && board[7] == piece;
            case 5: return board[3] == piece && board[4] == piece || board[13] == piece && board[20] == piece;
            case 6: return board[7] == piece && board[8] == piece || board[11] == piece && board[15] == piece;
            case 7: return board[6] == piece && board[8] == piece || board[1] == piece && board[4] == piece;
            case 8: return board[6] == piece && board[7] == piece || board[12] == piece && board[17] == piece;
            case 9: return board[0] == piece && board[21] == piece || board[10] == piece && board[11] == piece;
            case 10: return board[9] == piece && board[11] == piece || board[3] == piece && board[18] == piece;
            case 11: return board[9] == piece && board[10] == piece || board[6] == piece && board[15] == piece;
            case 12: return board[13] == piece && board[14] == piece || board[8] == piece && board[17] == piece;
            case 13: return board[12] == piece && board[14] == piece || board[5] == piece && board[20] == piece;
            case 14: return board[12] == piece && board[13] == piece || board[2] == piece && board[23] == piece;
            case 15: return board[16] == piece && board[17] == piece || board[6] == piece && board[11] == piece;
            case 16: return board[15] == piece && board[17] == piece || board[19] == piece && board[22] == piece;
            case 17: return board[15] == piece && board[16] == piece || board[8] == piece && board[12] == piece;
            case 18: return board[19] == piece && board[20] == piece || board[3] == piece && board[10] == piece;
            case 19: return board[18] == piece && board[20] == piece || board[16] == piece && board[22] == piece;
            case 20: return board[18] == piece && board[19] == piece || board[5] == piece && board[13] == piece;
            case 21: return board[22] == piece && board[23] == piece || board[0] == piece && board[9] == piece;
            case 22: return board[21] == piece && board[23] == piece || board[16] == piece && board[19] == piece;
            case 23: return board[21] == piece && board[22] == piece || board[2] == piece && board[14] == piece;
        }

        return {};
    }

#ifdef __GNUG__
    #pragma GCC diagnostic pop
#endif

    int m_clicked_id {-1};
    int m_hovered_id {-1};

    sm::Renderable m_renderable;
    sm::Renderable m_paint_renderable;

    std::vector<NodeObj> m_nodes;
    std::vector<PieceObj> m_pieces;
};
