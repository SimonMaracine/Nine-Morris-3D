#pragma once

#include <functional>
#include <vector>
#include <utility>

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

enum class GameOver {
    None,
    WinnerWhite,
    WinnerBlack,
    TieBetweenBothPlayers
};

template<typename Board>
struct Position {
    Board board {};
    Player turn {};

    bool operator==(const Position& other) const {
        return board == other.board && turn == other.turn;
    }
};

Player opponent(Player player);

template<typename Board>
unsigned int count_pieces(const Board& board, Player player) {
    unsigned int result {0};

    for (const Piece piece : board) {
        result += static_cast<unsigned int>(piece == static_cast<Piece>(player));
    }

    return result;
}

class BoardObj {
public:
    BoardObj() = default;
    BoardObj(const sm::Renderable& board, const sm::Renderable& board_paint);
    virtual ~BoardObj() = default;

    BoardObj(const BoardObj&) = default;
    BoardObj& operator=(const BoardObj&) = default;
    BoardObj(BoardObj&&) = default;
    BoardObj& operator=(BoardObj&&) = default;

    void set_board_paint_renderable(const sm::Renderable& board_paint);
protected:
    void update_hovered_id(glm::vec3 ray, glm::vec3 camera, std::function<std::vector<std::pair<int, sm::Renderable>>()>&& get_renderables);

    static void do_place_animation(PieceObj& piece, const NodeObj& node, std::function<void()>&& on_finish);
    static void do_move_animation(PieceObj& piece, const NodeObj& node, std::function<void()>&& on_finish, bool direct);
    static void do_take_animation(PieceObj& piece, std::function<void()>&& on_finish);

    int m_clicked_id {-1};
    int m_hovered_id {-1};

    sm::Renderable m_renderable;
    sm::Renderable m_paint_renderable;
};
