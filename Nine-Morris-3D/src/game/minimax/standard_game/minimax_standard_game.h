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
    MinimaxStandardGame(MinimaxStandardGame&&) = default;
    MinimaxStandardGame& operator=(MinimaxStandardGame&&) = default;

    virtual void start(GamePosition position, PieceType piece, Move& result, std::atomic<bool>& running) override;
private:
    int minimax(GamePosition& position, size_t depth, size_t turns_from_root, int alpha, int beta, PieceType type);
    void random_move(GamePosition& position, PieceType piece);  // TODO temporary

    std::vector<Move> get_all_moves(GamePosition& position, PieceType piece);
    void get_moves_phase1(GamePosition& position, PieceType piece, std::vector<Move>& moves);
    void get_moves_phase2(GamePosition& position, PieceType piece, std::vector<Move>& moves);
    void get_moves_phase3(GamePosition& position, PieceType piece, std::vector<Move>& moves);

    void make_move(GamePosition& position, const Move& move);
    void unmake_move(GamePosition& position, const Move& move);

    int evaluate_position(GamePosition& position);
    unsigned int calculate_material(GamePosition& position, PieceType piece);
    unsigned int calculate_freedom(GamePosition& position, PieceType piece);
    unsigned int calculate_piece_freedom(GamePosition& position, size_t index);

    bool all_pieces_in_mills(GamePosition& position, PieceType piece);
    std::array<size_t, 5> neighbor_free_positions(GamePosition& position, PieceType piece, size_t index);
    PieceType opponent_piece(PieceType type);
    bool is_mill(GamePosition& position, PieceType piece, size_t index);
    bool is_game_over(GamePosition& position);
    unsigned int number_of_pieces_outside(GamePosition& position, PieceType type);
    unsigned int total_number_of_pieces(GamePosition& position, PieceType type);

    // Data for a full minimax search
    Move best_move;
    int evaluation = 0;
    unsigned int positions_calculated = 0;
    unsigned int depth = 5;
};
