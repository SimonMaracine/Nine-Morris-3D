#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/constants.h"
#include "game/piece.h"
#include "game/node.h"

struct UndoRedoState;
class KeyboardControls;
class GameContext;

struct ThreefoldRepetitionHistory {
    struct PositionPlusInfo {
        bool operator==(const PositionPlusInfo& other) const {
            return (
                position == other.position &&
                piece_index == other.piece_index &&
                node_index == other.node_index
            );
        }

        std::array<PieceType, 24> position;
        size_t piece_index;
        size_t node_index;
    };

    std::vector<PositionPlusInfo> ones;
    std::vector<PositionPlusInfo> twos;
};

class GenericBoard {
public:
    GenericBoard() = default;
    GenericBoard(Application* app)
        : app(app) {}
    virtual ~GenericBoard() = default;

    virtual bool player_must_take_piece() { return false; };

    GamePosition get_position();
protected:
    Piece* new_piece_to_place(PieceType type, float x_pos, float z_pos, Node* node);
    void take_and_raise_piece(Piece* piece);
    void set_pieces_show_outline(PieceType type, bool show);
    void game_over(const BoardEnding& ending, PieceType type_to_hide);
    bool is_windmill_made(Node* node, PieceType type, size_t** windmills, size_t mills_count);
    void set_pieces_to_take(PieceType type, bool take);
    size_t number_of_pieces_in_windmills(PieceType type, size_t** windmills, size_t mills_count);
    void unselect_other_pieces(Piece* currently_selected_piece);
    void update_piece_outlines();
    void remember_position_and_check_repetition(Piece* piece, Node* node);
    void remember_state(const Camera& camera);
    void piece_arrive_at_node(Piece* piece);
    void prepare_piece_for_linear_move(Piece* piece, const glm::vec3& target, const glm::vec3& velocity);
    void prepare_piece_for_three_step_move(Piece* piece, const glm::vec3& target, const glm::vec3& velocity,
        const glm::vec3& target0, const glm::vec3& target1);

    Application* app = nullptr;

    std::shared_ptr<Renderer::Model> model;
    std::shared_ptr<Renderer::Model> paint_model;

    std::array<Node, 24> nodes;
    std::vector<std::optional<Piece>> pieces;

    BoardPhase phase = BoardPhase::PlacePieces;
    BoardPlayer turn = BoardPlayer::White;
    BoardEnding ending;

    // unsigned int white_pieces_count = 0;  // Number of pieces on the board
    // unsigned int black_pieces_count = 0;
    // unsigned int not_placed_pieces_count = 18;  // Number of pieces floating

    // bool player_must_take_piece() = false;

    Node* hovered_node = nullptr;
    Piece* hovered_piece = nullptr;
    Piece* selected_piece = nullptr;

    std::array<bool, 2> can_jump = { false, false };  // White first and black second

    // unsigned int turns_without_mills = 0;

    ThreefoldRepetitionHistory repetition_history;

    UndoRedoState* undo_redo_state = nullptr;
    KeyboardControls* keyboard = nullptr;
    GameContext* game_context = nullptr;

    bool next_move = true;  // It is false when any piece is in the air, true otherwise
    bool is_players_turn = true;
    bool switched_turn = false;

    friend class GameContext;
    friend class KeyboardControls;
    
    template<typename Archive>
    friend void serialize(Archive& archive, GenericBoard& board);
};
