#pragma once

#include "game/entities/serialization/node_serialized.h"
#include "game/entities/serialization/piece_serialized.h"
#include "game/entities/node.h"
#include "game/undo_redo_state.h"
#include "game/threefold_repetition_history.h"
#include "other/constants.h"

struct BoardSerialized {
    std::array<NodeSerialized, MAX_NODES> nodes;
    std::unordered_map<size_t, PieceSerialized> pieces;

    BoardPhase phase = BoardPhase::PlacePieces;
    BoardPlayer turn = BoardPlayer::White;
    BoardEnding ending;

    ThreefoldRepetitionHistory repetition_history;

    bool must_take_piece = false;
    bool is_players_turn = false;

    unsigned int turn_count = 0;
};
