#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "other/constants.h"

template<typename B>
struct UndoRedoState {
    struct State {
        B board;
        Camera camera;
        GameState game_state;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(board, camera, game_state);
        }
    };

    std::vector<State> undo;
    std::vector<State> redo;

    template<typename Archive>
    void serialize(Archive& archive) {
        archive(undo, redo);
    }
};
