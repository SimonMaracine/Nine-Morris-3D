#pragma once

#include "graphics/renderer/camera.h"
#include "nine_morris_3d/board.h"
#include "nine_morris_3d/game_context.h"

struct StateHistory {
    struct Page {
        Board board;
        Camera camera;
        GameState game_state;
    };

    std::vector<Page> undo;
    std::vector<Page> redo;
};
