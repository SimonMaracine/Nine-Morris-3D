#pragma once

#include <engine/engine_application.h>

#include "game/entities/boards/standard_board.h"
#include "game/entities/serialization/standard_board_serialized.h"
#include "game/scenes/imgui_layer.h"
#include "game/scenes/scene_game.h"

struct StandardGameScene : public Scene, public SceneGame<StandardGameScene, StandardBoardSerialized> {
    StandardGameScene()
        : Scene("standard_game") {}

    virtual void on_start() override;
    virtual void on_stop() override;
    virtual void on_awake() override;
    virtual void on_update() override;
    virtual void on_fixed_update() override;
    virtual void on_imgui_update() override;

    void on_mouse_button_pressed(const MouseButtonPressedEvent& event);
    void on_mouse_button_released(const MouseButtonReleasedEvent& event);
    void on_key_pressed(const KeyPressedEvent& event);
    void on_key_released(const KeyReleasedEvent& event);
    void on_window_resized(const WindowResizedEvent& event);

    virtual void setup_and_add_model_pieces() override;
    virtual void setup_entities() override;
    virtual void initialize_pieces() override;
    virtual void imgui_draw_debug() override;

    ImGuiLayer<StandardGameScene, StandardBoardSerialized> imgui_layer;

    StandardBoard board;
    UndoRedoState<StandardBoardSerialized> undo_redo_state;
};
