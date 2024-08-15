#pragma once

#include <array>
#include <vector>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/node.hpp"
#include "game/piece.hpp"

enum class Player {
    White = 1,
    Black = 2
};

enum class Piece {
    None,
    White,
    Black
};

enum class MoveType {
    Place,
    PlaceTake,
    Move,
    MoveTake
};

enum class GameOver {
    None,
    WinnerWhite,
    WinnerBlack,
    TieBetweenBothPlayers
};

struct Move {
    union {
        struct {
            int place_index;
        } place;

        struct {
            int place_index;
            int take_index;
        } place_take;

        struct {
            int source_index;
            int destination_index;
        } move;

        struct {
            int source_index;
            int destination_index;
            int take_index;
        } move_take;
    };

    MoveType type {};
};

using Board = std::array<Piece, 24>;

struct Position {
    Board board {};
    Player turn {};

    bool operator==(const Position& other) const {
        return board == other.board && turn == other.turn;
    }
};

class StandardBoard {
public:
    StandardBoard() = default;
    StandardBoard(
        const sm::Renderable& board,
        const sm::Renderable& board_paint,
        const std::vector<sm::Renderable>& nodes,
        const std::vector<sm::Renderable>& white_pieces,
        const std::vector<sm::Renderable>& black_pieces
    );

    void update(sm::Ctx& ctx, glm::vec3 ray, glm::vec3 camera);
private:
    void update_hovered_index(glm::vec3 ray, glm::vec3 camera);
    bool select_piece(int index);
    void try_place(int place_index);
    void try_place_take(int place_index, int take_index);
    void try_move(int source_index, int destination_index);
    void try_move_take(int source_index, int destination_index, int take_index);
    void finish_turn(bool advancement = true);
    void check_winner_material();
    void check_winner_blocking();
    void check_fifty_move_rule();
    void check_threefold_repetition(const Position& position);

    // Move generation
    std::vector<Move> generate_moves() const;
    static void generate_moves_phase1(Board& board, std::vector<Move>& moves, Player player);
    static void generate_moves_phase2(Board& board, std::vector<Move>& moves, Player player);
    static void generate_moves_phase3(Board& board, std::vector<Move>& moves, Player player);
    static void make_place_move(Board& board, Player player, int place_index);
    static void unmake_place_move(Board& board, int place_index);
    static void make_move_move(Board& board, int source_index, int destination_index);
    static void unmake_move_move(Board& board, int source_index, int destination_index);
    static bool is_mill(const Board& board, Player player, int index);
    static bool all_pieces_in_mills(const Board& board, Player player);
    static std::vector<int> neighbor_free_positions(const Board& board, int index);
    static Move create_place(int place_index);
    static Move create_place_take(int place_index, int take_index);
    static Move create_move(int source_index, int destination_index);
    static Move create_move_take(int source_index, int destination_index, int take_index);
    static unsigned int count_pieces(const Board& board, Player player);
    static Player opponent(Player player);

    int m_hovered_node_index {-1};
    int m_hovered_piece_index {-1};

    // Scene data
    std::array<NodeObj, 24> m_nodes {};
    std::array<PieceObj, 18> m_pieces {};

    // Game data
    Board m_board {};
    Player m_turn {Player::White};
    GameOver m_game_over {GameOver::None};
    unsigned int m_plies {};
    unsigned int m_plies_without_advancement {};
    std::vector<Position> m_positions;

    sm::Renderable m_renderable;
    sm::Renderable m_paint_renderable;
};
