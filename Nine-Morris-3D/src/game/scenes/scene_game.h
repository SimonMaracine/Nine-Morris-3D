#pragma once

#include <engine/engine_graphics.h>

#include "game/minimax/minimax_thread.h"
#include "game/assets_load.h"
#include "game/point_camera_controller.h"
#include "game/undo_redo_state.h"
#include "game/keyboard_controls.h"
#include "game/game_context.h"
#include "game/timer.h"

struct SceneGame {
    SceneGame() = default;
    virtual ~SceneGame() = default;

    SceneGame(const SceneGame&) = delete;
    SceneGame& operator=(const SceneGame&) = delete;
    SceneGame(SceneGame&&) = delete;
    SceneGame& operator=(SceneGame&&) = delete;

    virtual void setup_and_add_model_pieces() = 0;
    virtual void initialize_pieces() = 0;
    virtual void setup_entities() = 0;
    virtual void save_game() = 0;
    virtual void load_game() = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;
    virtual void imgui_draw_debug() = 0;

    std::unique_ptr<assets_load::SkyboxLoader> skybox_loader;
    std::unique_ptr<assets_load::BoardPaintTextureLoader> board_paint_texture_loader;

    // Game-related
    Camera camera;
    PointCameraController camera_controller {&camera};
    KeyboardControls keyboard;
    GameContext game;
    MinimaxThread minimax_thread;
    Timer timer;

    bool made_first_move = false;
    bool show_keyboard_controls = false;

    glm::vec3 default_camera_position = glm::vec3(0.0f);
    std::string save_game_file_name;

    // GUI-related
    bool show_wait_indicator = false;
    bool show_computer_thinking_indicator = false;
};
