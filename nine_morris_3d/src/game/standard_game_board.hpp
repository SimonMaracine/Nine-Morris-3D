#pragma once

#include <array>
#include <vector>
#include <functional>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/board.hpp"
#include "game/node.hpp"
#include "game/piece.hpp"

class StandardGameBoard : public BoardObj {
public:
    static constexpr int NODES {24};
    static constexpr int PIECES {18};

    enum class MoveType {
        Place,
        PlaceTake,
        Move,
        MoveTake
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

    using Board = std::array<Piece, NODES>;
    using Position = ::Position<Board>;

    StandardGameBoard() = default;
    StandardGameBoard(
        const sm::Renderable& board,
        const sm::Renderable& board_paint,
        const std::vector<sm::Renderable>& nodes,
        const std::vector<sm::Renderable>& white_pieces,
        const std::vector<sm::Renderable>& black_pieces,
        std::function<void(const Move&)>&& move_callback
    );

    Player get_turn() const { return m_turn; }
    GameOver get_game_over() const { return m_game_over; }

    void update(sm::Ctx& ctx, glm::vec3 ray, glm::vec3 camera);
    void update_movement();
    void user_click_press();
    void user_click_release();

    void place_piece(int place_index);
    void place_take_piece(int place_index, int take_index);
    void move_piece(int source_index, int destination_index);
    void move_take_piece(int source_index, int destination_index, int take_index);

    void debug();
private:
    void select(int index);
    void try_place(int place_index);
    void try_place_take(int place_index, int take_index);
    void try_move(int source_index, int destination_index);
    void try_move_take(int source_index, int destination_index, int take_index);

    void user_place(int place_index);
    void user_place_take_just_place(int place_index);
    void user_place_take(int place_index, int take_index);
    void user_move(int source_index, int destination_index);
    void user_move_take_just_move(int source_index, int destination_index);
    void user_move_take(int source_index, int destination_index, int take_index);

    // These just update the game data
    void place(int place_index);
    void place_take(int place_index, int take_index);
    void move(int source_index, int destination_index);
    void move_take(int source_index, int destination_index, int take_index);

    void finish_turn(bool advancement = true);
    void check_winner_material();
    void check_winner_blocking();
    void check_fifty_move_rule();
    void check_threefold_repetition(const Position& position);

    int new_piece_to_place(PieceType type);
    static bool is_node_id(int id);
    static bool is_piece_id(int id);
    static bool has_three_pieces(const Board& board, const PieceObj& piece);

    // Move generation
    std::vector<Move> generate_moves() const;
    static void generate_moves_phase1(Board& board, std::vector<Move>& moves, Player player);
    static void generate_moves_phase2(Board& board, std::vector<Move>& moves, Player player);
    static void generate_moves_phase3(Board& board, std::vector<Move>& moves, Player player);
    static void make_place_move(Board& board, Player player, int place_index);
    static void unmake_place_move(Board& board, int place_index);
    static void make_move_move(Board& board, int source_index, int destination_index);
    static void unmake_move_move(Board& board, int source_index, int destination_index);
    static bool all_pieces_in_mills(const Board& board, Player player);
    static std::vector<int> neighbor_free_positions(const Board& board, int index);
    static Move create_place(int place_index);
    static Move create_place_take(int place_index, int take_index);
    static Move create_move(int source_index, int destination_index);
    static Move create_move_take(int source_index, int destination_index, int take_index);

    // Game data
    Board m_board {};
    Player m_turn {Player::White};
    GameOver m_game_over {GameOver::None};
    unsigned int m_plies {};
    unsigned int m_plies_without_advancement {};
    std::vector<Position> m_positions;

    // Management data
    std::function<void(const Move&)> m_move_callback;
    std::vector<Move> m_legal_moves;
    int m_selected_index {-1};
    int m_take_action_index {-1};

    // Scene data
    std::array<NodeObj, NODES> m_nodes {};
    std::array<PieceObj, PIECES> m_pieces {};
};
