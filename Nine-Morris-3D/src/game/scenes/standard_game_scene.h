#pragma once

#include <engine/engine_application.h>

#include "game/entities/boards/standard_board.h"
#include "game/entities/serialization/standard_board_serialized.h"
#include "game/entities/board.h"
#include "game/scenes/scene_game.h"

struct StandardGameScene : public SceneGame {
    StandardGameScene()
        : SceneGame("standard_game") {}

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
    virtual void initialize_renderables() override;
    virtual void initialize_pieces() override;
    virtual void draw_debug_imgui() override;
    virtual void update_menubar() override;
    virtual void save_game() override;
    virtual void load_game() override;
    virtual void undo() override;
    virtual void redo() override;
    virtual Board& get_board() override;
    virtual size_t get_undo_size() override;
    virtual size_t get_redo_size() override;

    // ImGuiLayer<StandardGameScene, StandardBoardSerialized> imgui_layer;

    StandardBoard board;
    UndoRedoState<StandardBoardSerialized> undo_redo_state;
};
