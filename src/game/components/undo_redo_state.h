#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

// #include "ecs/internal_components.h"
#include "game/components/board.h"
#include "game/constants.h"

struct UndoRedoStateComponent {
    struct Page {
        BoardComponent board_c;
        CameraComponent camera_c;
        GameState game_state;
    };

    std::vector<Page> undo;
    std::vector<Page> redo;
};
