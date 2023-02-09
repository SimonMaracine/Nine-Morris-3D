#pragma once

#include "game/point_camera_controller.h"

template<typename B>
struct UndoRedoState {
    struct State {
        B board_serialized;
        PointCameraController camera_controller;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(board_serialized, camera_controller);
        }
    };

    UndoRedoState() = default;
    ~UndoRedoState() = default;

    UndoRedoState(const UndoRedoState&) = delete;
    UndoRedoState& operator=(const UndoRedoState&) = default;
    UndoRedoState(UndoRedoState&&) = delete;
    UndoRedoState& operator=(UndoRedoState&& other) noexcept;

    std::vector<State> undo;
    std::vector<State> redo;

    template<typename Archive>
    void serialize(Archive& archive) {
        archive(undo, redo);
    }
};

template<typename B>
UndoRedoState<B>& UndoRedoState<B>::operator=(UndoRedoState<B>&& other) noexcept {
    undo = std::move(other.undo);
    redo = std::move(other.redo);

    return *this;
}
