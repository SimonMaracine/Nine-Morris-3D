#pragma once

#include "game/entities/serialization/node_serialized.h"
#include "game/entities/serialization/piece_serialized.h"
#include "game/entities/node.h"
#include "game/undo_redo_state.h"
#include "other/constants.h"

struct JumpBoardSerialized {
    JumpBoardSerialized() = default;
    ~JumpBoardSerialized() = default;

    JumpBoardSerialized(const JumpBoardSerialized&) = default;
    JumpBoardSerialized(JumpBoardSerialized&&) = delete;
    JumpBoardSerialized& operator=(const JumpBoardSerialized&) = default;
    JumpBoardSerialized& operator=(JumpBoardSerialized&&) = default;

    std::array<NodeSerialized, 24> nodes;  // TODO put these in a struct
    std::unordered_map<size_t, PieceSerialized> pieces;

    BoardPhase phase = BoardPhase::PlacePieces;
    BoardPlayer turn = BoardPlayer::White;
    BoardEnding ending;

    bool must_take_piece = false;

    std::array<bool, 2> can_jump = { false, false };

    ThreefoldRepetitionHistory repetition_history;

    bool is_players_turn = false;

    unsigned int white_pieces_count = 0;
    unsigned int black_pieces_count = 0;
    unsigned int not_placed_white_pieces_count = 9;
    unsigned int not_placed_black_pieces_count = 9;

    unsigned int turns_without_mills = 0;
};
