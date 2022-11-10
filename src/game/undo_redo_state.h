#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

template<typename B>
struct UndoRedoState {
    struct State {
        B board_serialized;
        Camera camera;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(board_serialized, camera);
        }
    };

    std::vector<State> undo;
    std::vector<State> redo;

    template<typename Archive>
    void serialize(Archive& archive) {
        archive(undo, redo);
    }
};
