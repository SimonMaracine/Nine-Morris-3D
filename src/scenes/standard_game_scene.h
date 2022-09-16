#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "scenes/imgui_layer.h"
#include "game/boards/standard_board.h"
#include "game/undo_redo_state.h"
#include "game/keyboard_controls.h"
#include "game/game_context.h"
#include "game/options.h"
#include "game/timer.h"
#include "minimax/minimax_thread.h"

struct StandardGameScene : public Scene {
    StandardGameScene()
        : Scene("game") {}

    virtual void on_start() override;
    virtual void on_stop() override;
    virtual void on_awake() override;
    virtual void on_update() override;
    virtual void on_fixed_update() override;
    virtual void on_imgui_update() override;

    // ImGui
    ImGuiLayer<StandardGameScene> imgui_layer {app, this};

    // Game-related
    Camera camera;
    StandardBoard board;
    UndoRedoState undo_redo_state;
    KeyboardControls keyboard;
    GameContext game;
    MinimaxThread minimax_thread;

    bool first_move = false;
    bool show_keyboard_controls = false;

    glm::vec3 default_camera_position = glm::vec3(0.0f);

    std::unique_ptr<ConcurrentLoader<options::Options>> loader;

    // GUI-related
    Timer timer;
    bool show_wait_indicator = false;
    bool show_computer_thinking_indicator = false;

    std::shared_ptr<gui::Image> turn_indicator;
    std::shared_ptr<gui::Text> timer_text;
    std::shared_ptr<gui::Image> wait_indicator;
    std::shared_ptr<gui::Image> computer_thinking_indicator;

    // bool changed_skybox = false;
    // bool changed_texture_quality = false;
    // bool changed_labeled_board_texture = false;
    // bool changed_normal_mapping = false;

// #ifdef PLATFORM_GAME_DEBUG
//     std::shared_ptr<Texture> light_bulb_texture;
//     Renderer::Quad light_bulb_quad;
// #endif
};
