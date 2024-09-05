#pragma once

#include <array>
#include <vector>
#include <functional>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/board.hpp"
#include "game/node.hpp"
#include "game/piece.hpp"

class JumpVariantBoard : public BoardObj {
public:
    static constexpr int NODES {24};
    static constexpr int PIECES {6};

    struct Move {
        int source_index;
        int destination_index;
    };

    using Board = std::array<Piece, NODES>;
    using Position = ::Position<Board>;

    JumpVariantBoard() = default;
    JumpVariantBoard(
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

    void move_piece(int source_index, int destination_index);

    void debug();
private:
    void update_nodes();
    void update_pieces();

    void select(int index);
    void try_move(int source_index, int destination_index);

    void user_move(int source_index, int destination_index);

    // This just updates the game data
    void move(int source_index, int destination_index);

    void finish_turn();
    void check_winner();
    void check_fifty_move_rule();
    void check_threefold_repetition(const Position& position);

    static bool is_node_id(int id);
    static bool is_piece_id(int id);

    // Move generation
    std::vector<Move> generate_moves() const;
    static bool is_mill(const Board& board, Player player, int index);
    static Move create_move(int source_index, int destination_index);

    // Game data
    Board m_board {};
    Player m_turn {Player::White};
    GameOver m_game_over {GameOver::None};
    unsigned int m_plies_without_advancement {};
    std::vector<Position> m_positions;

    // Management data
    std::function<void(const Move&)> m_move_callback;
    std::vector<Move> m_legal_moves;
    int m_user_stored_index1 {-1};

    // Scene data
    std::array<NodeObj, NODES> m_nodes {};
    std::array<PieceObj, PIECES> m_pieces {};
};
