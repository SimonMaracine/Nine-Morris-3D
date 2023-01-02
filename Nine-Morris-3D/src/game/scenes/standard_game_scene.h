#pragma once

#include <engine/engine_application.h>
#include <engine/engine_graphics.h>
#include <engine/engine_audio.h>

#include "game/entities/boards/standard_board.h"
#include "game/entities/serialization/standard_board_serialized.h"
#include "game/minimax/minimax_thread.h"
#include "game/scenes/imgui_layer.h"
#include "game/undo_redo_state.h"
#include "game/keyboard_controls.h"
#include "game/game_context.h"
#include "game/timer.h"
#include "game/assets_load.h"
#include "game/point_camera_controller.h"
#include "other/constants.h"

struct StandardGameScene : public Scene {
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

    void setup_and_add_model_pieces();

    void initialize_pieces();
    void setup_entities();

    void save_game();
    void load_game();
    void undo();
    void redo();

    std::unique_ptr<assets_load::SkyboxLoader> skybox_loader;
    std::unique_ptr<assets_load::BoardPaintTextureLoader> board_paint_texture_loader;

    // ImGui
    ImGuiLayer<StandardGameScene, StandardBoardSerialized> imgui_layer;

    // Game-related
    Camera camera;
    PointCameraController camera_controller {&camera};
    StandardBoard board;
    UndoRedoState<StandardBoardSerialized> undo_redo_state;
    KeyboardControls keyboard;
    GameContext game;
    MinimaxThread minimax_thread;
    Timer timer;

    bool made_first_move = false;
    bool show_keyboard_controls = false;

    glm::vec3 default_camera_position = glm::vec3(0.0f);

    // GUI-related
    bool show_wait_indicator = false;
    bool show_computer_thinking_indicator = false;
};
