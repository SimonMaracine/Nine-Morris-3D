#pragma once

#include <glm/glm.hpp>

#include "graphics/renderer/renderer.h"
#include "graphics/renderer/hoverable.h"
#include "nine_morris_3d/node.h"
#include "nine_morris_3d/piece.h"
#include "nine_morris_3d/constants.h"

struct StateHistory;
class KeyboardControls;

using PosValue = Piece::Type;
using GamePosition = std::array<Piece::Type, 24>;

class Board {
public:
    enum class Phase {
        None,
        PlacePieces = 1,
        MovePieces = 2,
        GameOver
    };

    enum class Player {
        White = 0,
        Black = 1
    };

    enum class Ending {
        None,
        WinnerWhite,
        WinnerBlack,
        TieBetweenBothPlayers
    };

    Board() = default;
    Board(StateHistory& state_history);
    ~Board() = default;

    static void copy_smart(Board& to, const Board& from, StateHistory* state_history);

    bool place_piece(hoverable::Id hovered_id);
    bool take_piece(hoverable::Id hovered_id);
    bool put_down_piece(hoverable::Id hovered_id);
    void computer_place_piece(size_t node_index);
    void computer_take_piece(size_t node_index);
    void computer_put_down_piece(size_t source_node_index, size_t destination_node_index);
    void move_pieces(float dt);
    void select_piece(hoverable::Id hovered_id);
    void press(hoverable::Id hovered_id);
    void release();
    bool undo();
    bool redo();
    unsigned int not_placed_pieces_count();
    void finalize_pieces_state();
    void update_cursor();
    void update_nodes(hoverable::Id hovered_id);
    void update_pieces(hoverable::Id hovered_id);
    std::string_view get_ending_message();
    GamePosition get_position();

    Renderer::Model model;
    Renderer::Model paint_model;

    std::array<Node, 24> nodes;
    std::array<Piece, 18> pieces;

    Phase phase = Phase::PlacePieces;
    Player turn = Player::White;
    Ending ending = Ending::None;

    std::string ending_message;

    unsigned int white_pieces_count = 0;  // Number of pieces on the board
    unsigned int black_pieces_count = 0;
    unsigned int not_placed_white_pieces_count = 9;  // Number of pieces floating
    unsigned int not_placed_black_pieces_count = 9;
    bool should_take_piece = false;

    Node* hovered_node = nullptr;
    Piece* hovered_piece = nullptr;
    Piece* selected_piece = nullptr;

    std::array<bool, 2> can_jump = { false, false };  // White first and black second

    unsigned int turns_without_mills = 0;

    struct ThreefoldRepetitionHistory {
        struct PositionPlusInfo {
            bool operator==(const PositionPlusInfo& other) const {
                return (
                    position == other.position &&
                    piece_index == other.piece_index &&
                    node_index == other.node_index
                );
            }

            GamePosition position;
            size_t piece_index;
            size_t node_index;
        };

        std::vector<PositionPlusInfo> ones;
        std::vector<PositionPlusInfo> twos;
    } repetition_history;

    std::vector<Board>* undo_state_history = nullptr;
    std::vector<Board>* redo_state_history = nullptr;
    KeyboardControls* keyboard = nullptr;
    bool next_move = true;  // It is false when any piece is in air and true otherwise
    bool is_players_turn = true;
private:
    Piece* new_piece_to_place(Piece::Type type, float x_pos, float z_pos, Node* node);
    void take_and_raise_piece(Piece* piece);
    void set_pieces_show_outline(Piece::Type type, bool show);
    void game_over(Ending ending, Piece::Type type_to_hide, std::string_view ending_message);
    void switch_turn();
    bool is_windmill_made(Node* node, Piece::Type type);
    void set_pieces_to_take(Piece::Type type, bool take);
    unsigned int number_of_pieces_in_windmills(Piece::Type type);
    void unselect_other_pieces(Piece* currently_selected_piece);
    void update_outlines();
    bool can_go(Node* source_node, Node* destination_node);
    void check_player_number_of_pieces(Player player);
    bool is_player_blocked(Player player);
    void remember_position_and_check_repetition(Piece* piece, Node* node);
    void remember_state();
    void arrive_at_node(Piece* piece);
    void prepare_piece_for_linear_move(Piece* piece, const glm::vec3& target, const glm::vec3& velocity);
    void prepare_piece_for_three_step_move(Piece* piece, const glm::vec3& target, const glm::vec3& velocity,
            const glm::vec3& target0, const glm::vec3& target1);
};

struct StateHistory {
    std::vector<Board> undo_state_history;
    std::vector<Board> redo_state_history;
};
