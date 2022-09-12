#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/boards/generic_board.h"
#include "game/game_context.h"

struct UndoRedoState {
    struct State {
        GenericBoard board;
        Camera camera;
        GameState game_state;
    };

    std::vector<State> undo;
    std::vector<State> redo;
};
