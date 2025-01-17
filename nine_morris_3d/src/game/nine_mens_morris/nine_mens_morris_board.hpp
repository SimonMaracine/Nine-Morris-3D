#pragma once

#include <array>
#include <vector>
#include <functional>
#include <string>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/board.hpp"
#include "game/nine_mens_morris/node.hpp"
#include "game/nine_mens_morris/piece.hpp"

class NineMensMorrisBoard : public BoardObj {
public:
    static constexpr int NODES {24};
    static constexpr int PIECES {18};

    enum class Player {
        White = 1,
        Black = 2
    };

    enum class MoveType {
        Place,
        PlaceCapture,
        Move,
        MoveCapture
    };

    enum class Node {
        None = 0,
        White = 1,
        Black = 2
    };

    struct Move {
        union {
            struct {
                int place_index;
            } place;

            struct {
                int place_index;
                int capture_index;
            } place_capture;

            struct {
                int source_index;
                int destination_index;
            } move;

            struct {
                int source_index;
                int destination_index;
                int capture_index;
            } move_capture;
        };

        MoveType type {};

        bool operator==(const Move& other) const;

        static Move create_place(int place_index);
        static Move create_place_capture(int place_index, int capture_index);
        static Move create_move(int source_index, int destination_index);
        static Move create_move_capture(int source_index, int destination_index, int capture_index);
    };

    using Board = std::array<Node, NODES>;

    struct Position {
        Board board {};
        Player player {Player::White};
        unsigned int plies {0};

        bool operator==(const Position& other) const {
            return board == other.board && player == other.player && plies >= 18 && other.plies >= 18;
        }
    };

    NineMensMorrisBoard() = default;
    NineMensMorrisBoard(
        const sm::Renderable& board,
        const sm::Renderable& board_paint,
        const std::vector<sm::Renderable>& nodes,
        const std::vector<sm::Renderable>& white_pieces,
        const std::vector<sm::Renderable>& black_pieces,
        std::function<void(const Move&)>&& move_callback
    );

    const GameOver& get_game_over() const override;
    PlayerColor get_player_color() const override;
    Player get_player() const { return m_position.player; }
    const Position& get_setup_position() const { return m_setup_position; }

    sm::Renderable& get_renderable() { return m_renderable; }
    sm::Renderable& get_paint_renderable() { return m_paint_renderable; }
    std::array<NodeObj, NODES>& get_nodes() { return m_nodes; }
    std::vector<PieceObj>& get_pieces() { return m_pieces; }

    void update(sm::Ctx& ctx, glm::vec3 ray, glm::vec3 camera, bool user_input);
    void update_movement();
    void user_click_press();
    void user_click_release();

    void reset(const Position& position);
    void play_move(const Move& move);
    void timeout(Player player);
    void resign(Player player);
    void offer_draw();

    static Move move_from_string(const std::string& string);
    static std::string move_to_string(const Move& move);
    static Position position_from_string(const std::string& string);
    static std::string position_to_string(const Position& position);

    void debug();

    void set_renderables(
        const sm::Renderable& board,
        const sm::Renderable& board_paint,
        const std::vector<sm::Renderable>& nodes,
        const std::vector<sm::Renderable>& white_pieces,
        const std::vector<sm::Renderable>& black_pieces
    );
private:
    void initialize_objects(
        const std::vector<sm::Renderable>& nodes,
        const std::vector<sm::Renderable>& white_pieces,
        const std::vector<sm::Renderable>& black_pieces
    );
    void initialize_objects();

    void update_nodes_highlight(std::function<bool()>&& highlight);
    void update_pieces_highlight(std::function<bool(const PieceObj&)>&& highlight);
    void update_nodes(sm::Ctx& ctx);
    void update_pieces(sm::Ctx& ctx);

    static void do_place_animation(PieceObj& piece, const NodeObj& node, std::function<void()>&& on_finish);
    static void do_move_animation(PieceObj& piece, const NodeObj& node, std::function<void()>&& on_finish, bool direct);
    static void do_take_animation(PieceObj& piece, std::function<void()>&& on_finish);

    void select(int id);
    void try_place(int place_index);
    void try_move(int source_index, int destination_index);
    void try_capture(int capture_index);

    // These just change the state
    void play_place_move(const Move& move);
    void play_place_capture_move(const Move& move);
    void play_move_move(const Move& move);
    void play_move_capture_move(const Move& move);

    void finish_turn(bool advancement = true);
    void check_material();
    void check_legal_moves();
    void check_fifty_move_rule();
    void check_threefold_repetition();

    int new_piece_to_place(PieceType type);
    static bool is_node_id(int id);
    static bool is_piece_id(int id);
    static bool has_three_pieces(const Board& board, const PieceObj& piece);

    // Move generation
    std::vector<Move> generate_moves() const;
    static std::vector<Move> generate_moves_phase1(Board& board, Player player);
    static std::vector<Move> generate_moves_phase2(Board& board, Player player);
    static std::vector<Move> generate_moves_phase3(Board& board, Player player);
    static void make_place_move(Board& board, Player player, int place_index);
    static void unmake_place_move(Board& board, int place_index);
    static void make_move_move(Board& board, int source_index, int destination_index);
    static void unmake_move_move(Board& board, int source_index, int destination_index);
    static bool is_mill(const Board& board, Player player, int index);
    static bool all_pieces_in_mills(const Board& board, Player player);
    static std::vector<int> neighbor_free_positions(const Board& board, int index);
    static int count_pieces(const Board& board, Player player);
    static Player opponent(Player player);

    template<typename T>
    static T if_player_white(Player player, T&& value_if_white, T&& value_if_black) {
        return player == Player::White ? value_if_white : value_if_black;
    }

    // Game data
    Position m_position;
    unsigned int m_plies_no_advancement {};
    std::vector<Position> m_positions;

    // Management data
    bool m_capture_piece {false};
    int m_select_id {-1};
    Position m_setup_position;
    std::vector<Move> m_legal_moves;
    std::vector<Move> m_candidate_moves;
    std::function<void(const Move&)> m_move_callback;

    // Objects
    sm::Renderable m_renderable;
    sm::Renderable m_paint_renderable;
    std::array<NodeObj, NODES> m_nodes;
    std::vector<PieceObj> m_pieces;
};
