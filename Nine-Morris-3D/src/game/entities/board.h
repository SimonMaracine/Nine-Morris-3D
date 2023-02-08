#pragma once

#include <engine/engine_application.h>
#include <engine/engine_scene.h>

#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "game/threefold_repetition_history.h"
#include "game/point_camera_controller.h"
#include "other/constants.h"

class KeyboardControls;
class Scene;

struct Board {
    Board() = default;
    virtual ~Board() = default;

    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;
    Board(Board&&) = default;
    Board& operator=(Board&&) = default;

    struct Flags;

    virtual void click(identifier::Id) = 0;
    virtual Flags release(identifier::Id) = 0;
    virtual void computer_place_piece(Index) = 0;
    virtual void computer_move_piece(Index, Index) = 0;
    virtual void computer_take_piece(Index) = 0;

    GamePosition get_position();
    void update_nodes(identifier::Id hovered_id);
    void update_pieces(identifier::Id hovered_id);
    void move_pieces();
    void finalize_pieces_state();

    Index new_piece_to_place(PieceType type, float x_pos, float z_pos, Index node_index);
    void take_and_raise_piece(Index piece_index);
    void select_piece(Index piece_index);
    void set_pieces_show_outline(PieceType type, bool show);
    void set_pieces_to_take(PieceType type, bool take);
    void game_over(const BoardEnding& ending);
    bool is_mill_made(Index node_index, PieceType type, const size_t mills[][3], size_t mills_count);
    size_t number_of_pieces_in_mills(PieceType type, const size_t mills[][3], size_t mills_count);
    void unselect_other_pieces(Index currently_selected_piece_index_index);
    void update_piece_outlines();
    void play_piece_place_sound(Index piece_index);
    void play_piece_move_sound(Index piece_index);
    void play_piece_take_sound(Index piece_index);
    void remember_position_and_check_repetition(Index piece_index, Index node_index);
    void piece_arrive_at_node(Index piece_index);
    void prepare_piece_for_linear_move(Index piece_index, const glm::vec3& target, const glm::vec3& velocity);
    void prepare_piece_for_three_step_move(Index piece_index, const glm::vec3& target, const glm::vec3& velocity,
        const glm::vec3& target0, const glm::vec3& target1);

    object::Model* model = nullptr;
    object::Model* paint_model = nullptr;

    std::array<Node, MAX_NODES> nodes;  // 24 ordered nodes
    std::unordered_map<Index, Piece> pieces;  // Any number of pieces

    BoardPhase phase = BoardPhase::PlacePieces;
    BoardPlayer turn = BoardPlayer::White;
    BoardEnding ending;

    Index clicked_node_index = NULL_INDEX;
    Index clicked_piece_index = NULL_INDEX;
    Index selected_piece_index = NULL_INDEX;

    ThreefoldRepetitionHistory repetition_history;

    bool must_take_piece = false;
    bool next_move = true;  // It is false when any piece is in the air, true otherwise
    bool is_players_turn = false;

    unsigned int turn_count = 0;

    struct Flags {
        bool did_action = false;
        bool switched_turn = false;
        bool must_take_or_took_piece = false;
    } flags;

    Application* app = nullptr;
    Scene* scene = nullptr;
    KeyboardControls* keyboard = nullptr;
    PointCameraController* camera_controller = nullptr;
};
