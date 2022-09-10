#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/constants.h"
#include "game/piece.h"
#include "game/node.h"
#include "game/undo_redo_state.h"
#include "game/keyboard_controls.h"

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

struct Board {
    Board() = default;
    ~Board() = default;

    std::shared_ptr<Renderer::Model> model;
    std::shared_ptr<Renderer::Model> paint_model;

    std::array<Node, 24> nodes;
    std::array<Piece, 18> pieces;

    BoardPhase phase = BoardPhase::PlacePieces;
    BoardPlayer turn = BoardPlayer::White;
    BoardEnding ending = BoardEnding::None;

    std::string ending_message;

    unsigned int white_pieces_count = 0;  // Number of pieces on the board
    unsigned int black_pieces_count = 0;
    unsigned int not_placed_pieces_count = 18;  // Number of pieces floating
    bool should_take_piece = false;

    Node* hovered_node = nullptr;
    Piece* hovered_piece = nullptr;
    Piece* selected_piece = nullptr;

    std::array<bool, 2> can_jump = { false, false };  // White first and black second

    unsigned int turns_without_mills = 0;

    ThreefoldRepetitionHistory repetition_history;

    UndoRedoState* undo_redo_state = nullptr;
    KeyboardControls* keyboard = nullptr;
    GameContext* game_context = nullptr;

    bool next_move = true;  // It is false when any piece is in the air, true otherwise
    bool is_players_turn = true;
    bool switched_turn = false;
};