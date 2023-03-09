#pragma once

#include "game/minimax/common.h"
#include "game/game_position.h"
#include "other/constants.h"

class MinimaxStandardGame : public MinimaxAlgorithm {
public:
    MinimaxStandardGame() = default;
    ~MinimaxStandardGame() = default;

    MinimaxStandardGame(const MinimaxStandardGame&) = default;
    MinimaxStandardGame& operator=(const MinimaxStandardGame&) = default;
    MinimaxStandardGame(MinimaxStandardGame&&) noexcept = default;
    MinimaxStandardGame& operator=(MinimaxStandardGame&&) noexcept = default;

    virtual void start(GamePosition position, PieceType piece, Move& result, std::atomic<bool>& running) override;

    struct {
        int PIECE = 7;
        int FREEDOM = 1;
    } parameters;
private:
    int minimax(size_t depth, size_t turns_from_root, int alpha, int beta, PieceType type);
    void random_move(PieceType piece);  // TODO temporary

    std::vector<Move> get_all_moves(PieceType piece);
    void get_moves_phase1(PieceType piece, std::vector<Move>& moves);
    void get_moves_phase2(PieceType piece, std::vector<Move>& moves);
    void get_moves_phase3(PieceType piece, std::vector<Move>& moves);

    void make_move(const Move& move);
    void unmake_move(const Move& move);

    void make_place_move(PieceType piece, size_t place_node_index);
    void unmake_place_move(size_t place_node_index);
    void make_move_move(PieceType piece, size_t move_source_node_index, size_t move_destination_node_index);
    void unmake_move_move(PieceType piece, size_t move_source_node_index, size_t move_destination_node_index);

    int evaluate_position();
    unsigned int calculate_material(PieceType piece);
    unsigned int calculate_freedom(PieceType piece);
    unsigned int calculate_piece_freedom(size_t index);

    bool all_pieces_in_mills(PieceType piece);
    std::array<size_t, 5> neighbor_free_positions(size_t index);
    PieceType opponent_piece(PieceType type);
    bool is_mill(PieceType piece, size_t index);
    bool is_game_over();
    unsigned int number_of_pieces_outside(PieceType type);
    unsigned int total_number_of_pieces(PieceType type);

    GamePosition position;
    Move best_move;
    int evaluation = 0;
    unsigned int positions_calculated = 0;
    unsigned int depth = 5;
};
