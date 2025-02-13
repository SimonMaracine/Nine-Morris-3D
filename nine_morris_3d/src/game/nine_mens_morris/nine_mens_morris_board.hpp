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
    static constexpr int PIECES {24};
    static constexpr int NINE {18};
    static constexpr int TWELVE {24};

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
        int plies {0};

        bool eq(const Position& other, int p) const {
            return board == other.board && player == other.player && plies >= p && other.plies >= p;
        }
    };

    using NodeModels = sm::utils::Array<std::shared_ptr<sm::ModelNode>, int, NODES>;
    using PieceModels = sm::utils::Array<std::shared_ptr<sm::OutlinedModelNode>, int, PIECES / 2>;

    using Nodes = std::array<NodeObj, NODES>;
    using Pieces = sm::utils::Array<PieceObj, int, PIECES>;

    NineMensMorrisBoard() = default;
    NineMensMorrisBoard(
        std::shared_ptr<sm::ModelNode> board,
        std::shared_ptr<sm::ModelNode> paint,
        const NodeModels& nodes,
        const PieceModels& white_pieces,
        const PieceModels& black_pieces,
        std::shared_ptr<sm::SoundData> sound_piece_place1,
        std::shared_ptr<sm::SoundData> sound_piece_place2,
        std::shared_ptr<sm::SoundData> sound_piece_place3,
        std::shared_ptr<sm::SoundData> sound_piece_move1,
        std::shared_ptr<sm::SoundData> sound_piece_move2,
        std::shared_ptr<sm::SoundData> sound_piece_move3,
        std::shared_ptr<sm::SoundData> sound_piece_capture1,
        std::shared_ptr<sm::SoundData> sound_piece_capture2,
        std::function<void(const Move&)>&& move_callback
    );

    void user_click_release_callback() override;
    const GameOver& get_game_over() const override;
    PlayerColor get_player_color() const override;
    bool is_turn_finished() const override;
    void setup_pieces() override;

    Player get_player() const { return m_position.player; }
    const Position& get_setup_position() const { return m_setup_position; }

    std::shared_ptr<sm::ModelNode> get_board_model() const { return m_board_model; }
    std::shared_ptr<sm::ModelNode> get_paint_model() const { return m_paint_model; }
    Nodes& get_nodes() { return m_nodes; }
    Pieces& get_pieces() { return m_pieces; }

    void update(sm::Ctx& ctx, glm::vec3 ray, glm::vec3 camera, bool user_input);
    void update_movement();

    void reset(const Position& position);
    void play_move(const Move& move);
    void timeout(Player player);
    void resign(Player player);
    void accept_draw();

    static Move move_from_string(const std::string& string);
    static std::string move_to_string(const Move& move);
    static Position position_from_string(const std::string& string);
    static std::string position_to_string(const Position& position);

    void debug_window();

    template<typename T>
    T if_player_white(T value_if_white, T value_if_black) const {
        return m_position.player == Player::White ? value_if_white : value_if_black;
    }
private:
    void initialize_objects(const NodeModels& nodes, const PieceModels& white_pieces, const PieceModels& black_pieces);
    void initialize_objects();

    void update_nodes_highlight(std::function<bool()>&& highlight, bool enabled);
    void update_pieces_highlight(std::function<bool(const PieceObj&)>&& highlight, bool enabled);
    void update_nodes(sm::Ctx& ctx);
    void update_pieces(sm::Ctx& ctx);

    void do_place_animation(PieceObj& piece, const NodeObj& node, PieceObj::OnFinish&& on_finish) const;
    void do_move_animation(PieceObj& piece, const NodeObj& node, PieceObj::OnFinish&& on_finish, bool direct) const;
    void do_take_animation(PieceObj& piece, PieceObj::OnFinish&& on_finish) const;

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
    static std::vector<Move> generate_moves_phase1(Board& board, Player player, int p);
    static std::vector<Move> generate_moves_phase2(Board& board, Player player, int p);
    static std::vector<Move> generate_moves_phase3(Board& board, Player player, int p);
    static void make_place_move(Board& board, Player player, int place_index);
    static void unmake_place_move(Board& board, int place_index);
    static void make_move_move(Board& board, int source_index, int destination_index);
    static void unmake_move_move(Board& board, int source_index, int destination_index);
    static bool is_mill(const Board& board, Player player, int index, int p);
    static bool is_mill9(const Board& board, Player player, int index);
    static bool is_mill12(const Board& board, Player player, int index);
    static bool all_pieces_in_mills(const Board& board, Player player, int p);
    static std::vector<int> neighbor_free_positions(const Board& board, int index, int p);
    static std::vector<int> neighbor_free_positions9(const Board& board, int index);
    static std::vector<int> neighbor_free_positions12(const Board& board, int index);
    static int count_pieces(const Board& board, Player player);
    static Player opponent(Player player);

    // Game data
    Position m_position;
    int m_plies_no_advancement {};
    std::vector<Position> m_positions;

    // Management data
    bool m_capture_piece {false};
    int m_select_id {-1};
    Position m_setup_position;
    std::vector<Move> m_legal_moves;
    std::vector<Move> m_candidate_moves;
    std::function<void(const Move&)> m_move_callback;  // Called after the turn has finished

    // Objects
    std::shared_ptr<sm::ModelNode> m_board_model;
    std::shared_ptr<sm::ModelNode> m_paint_model;
    Nodes m_nodes;
    Pieces m_pieces;

    // Sounds
    std::shared_ptr<sm::SoundData> m_sound_piece_place1;
    std::shared_ptr<sm::SoundData> m_sound_piece_place2;
    std::shared_ptr<sm::SoundData> m_sound_piece_place3;
    std::shared_ptr<sm::SoundData> m_sound_piece_move1;
    std::shared_ptr<sm::SoundData> m_sound_piece_move2;
    std::shared_ptr<sm::SoundData> m_sound_piece_move3;
    std::shared_ptr<sm::SoundData> m_sound_piece_capture1;
    std::shared_ptr<sm::SoundData> m_sound_piece_capture2;
};
