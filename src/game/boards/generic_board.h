#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/piece.h"
#include "game/node.h"
#include "game/threefold_repetition_history.h"
#include "other/constants.h"

struct UndoRedoState;
class KeyboardControls;
class GameContext;

struct GenericBoard {
    GenericBoard() = default;
    GenericBoard(Application* app)
        : app(app) {}
    virtual ~GenericBoard() = default;

    virtual void click(hover::Id) {}
    virtual std::tuple<bool, bool, bool> release() { return {}; }

    GamePosition get_position();
    void update_nodes(hover::Id hovered_id);
    void update_pieces(hover::Id hovered_id);
    void move_pieces();
    void finalize_pieces_state();

    size_t new_piece_to_place(PieceType type, float x_pos, float z_pos, size_t node_index);
    void take_and_raise_piece(size_t piece_index);
    void select_piece(size_t piece_index);
    void set_pieces_show_outline(PieceType type, bool show);
    void set_pieces_to_take(PieceType type, bool take);
    void game_over(const BoardEnding& ending, PieceType type_to_hide);
    bool is_windmill_made(size_t node_index, PieceType type, const size_t windmills[][3], size_t mills_count);
    size_t number_of_pieces_in_windmills(PieceType type, const size_t windmills[][3], size_t mills_count);
    void unselect_other_pieces(size_t currently_selected_piece_index_index);
    void update_piece_outlines();
    void remember_position_and_check_repetition(size_t piece_index, size_t node_index);
    void remember_state(const Camera& camera);
    void piece_arrive_at_node(size_t piece_index);
    void prepare_piece_for_linear_move(size_t piece_index, const glm::vec3& target, const glm::vec3& velocity);
    void prepare_piece_for_three_step_move(size_t piece_index, const glm::vec3& target, const glm::vec3& velocity,
        const glm::vec3& target0, const glm::vec3& target1);

    Application* app = nullptr;

    std::shared_ptr<Renderer::Model> model;
    std::shared_ptr<Renderer::Model> paint_model;

    std::array<Node, 24> nodes;  // 24 ordered nodes
    std::unordered_map<size_t, Piece> pieces;  // Any number of pieces

    BoardPhase phase = BoardPhase::PlacePieces;
    BoardPlayer turn = BoardPlayer::White;
    BoardEnding ending;

    // unsigned int white_pieces_count = 0;  // Number of pieces on the board
    // unsigned int black_pieces_count = 0;
    // unsigned int not_placed_pieces_count = 18;  // Number of pieces floating

    bool must_take_piece = false;

    size_t clicked_node_index = NULL_INDEX;
    size_t clicked_piece_index = NULL_INDEX;
    size_t selected_piece_index = NULL_INDEX;

    std::array<bool, 2> can_jump = { false, false };  // White first and black second

    // unsigned int turns_without_mills = 0;

    ThreefoldRepetitionHistory repetition_history;

    UndoRedoState* undo_redo_state = nullptr;
    KeyboardControls* keyboard = nullptr;
    GameContext* game_context = nullptr;

    bool next_move = true;  // It is false when any piece is in the air, true otherwise
    bool is_players_turn = true;

    bool did_action = false;
    bool switched_turn = false;
    bool must_take_piece_or_took_piece = false;

    friend class GameContext;  // TODO see if needed
    friend class KeyboardControls;

    template<typename Archive>
    friend void serialize(Archive& archive, GenericBoard& board);
};
