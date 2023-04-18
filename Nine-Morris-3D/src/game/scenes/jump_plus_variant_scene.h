#pragma once

#include <engine/public/application_base.h>

#include "game/entities/boards/jump_board.h"
#include "game/entities/serialization/jump_board_serialized.h"
#include "game/entities/board.h"
#include "game/scenes/scene_game.h"

struct JumpPlusVariantScene : public SceneGame {
    JumpPlusVariantScene()
        : SceneGame("jump_plus_variant") {}

    virtual void on_start() override;
    virtual void on_stop() override;
    virtual void on_awake() override;
    virtual void on_update() override;
    virtual void on_fixed_update() override;
    virtual void on_imgui_update() override;

    void on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event);
    void on_mouse_button_released(const sm::MouseButtonReleasedEvent& event);
    void on_key_pressed(const sm::KeyPressedEvent& event);
    void on_key_released(const sm::KeyReleasedEvent& event);
    void on_window_resized(const sm::WindowResizedEvent& event);

    virtual void setup_and_add_model_pieces() override;
    virtual void initialize_pieces() override;
    virtual void setup_entities() override;
    virtual void initialize_renderables() override;
    virtual void draw_debug_imgui() override;
    virtual void draw_ai_configuration_imgui() override;
    virtual void update_menubar() override;
    virtual void save_game() override;
    virtual void load_game() override;
    virtual void undo() override;
    virtual void redo() override;
    virtual Board& get_board() override;
    virtual size_t get_undo_size() override;
    virtual size_t get_redo_size() override;

    JumpBoard board;
    UndoRedoState<JumpBoardSerialized> undo_redo_state;
};
