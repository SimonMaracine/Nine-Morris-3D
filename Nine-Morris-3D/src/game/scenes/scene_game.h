#pragma once

#include <engine/engine_application.h>
#include <engine/engine_graphics.h>
#include <engine/engine_other.h>
#include <engine/engine_scene.h>

#include "game/minimax/minimax_thread.h"
#include "game/assets_load.h"
#include "game/save_load.h"
#include "game/point_camera_controller.h"
#include "game/undo_redo_state.h"
#include "game/keyboard_controls.h"
#include "game/game_context.h"
#include "game/timer.h"

class SceneGame : public Scene {
public:
    SceneGame(const std::string& name)
        : Scene(name) {}
    virtual ~SceneGame() = default;

    SceneGame(const SceneGame&) = delete;
    SceneGame& operator=(const SceneGame&) = delete;
    SceneGame(SceneGame&&) = delete;
    SceneGame& operator=(SceneGame&&) = delete;

    virtual void setup_and_add_model_pieces() = 0;
    virtual void setup_entities() = 0;
    virtual void initialize_renderables() = 0;
    virtual void initialize_pieces() = 0;
    virtual void draw_debug_imgui() = 0;
    virtual void update_menubar() = 0;
    virtual void save_game() = 0;
    virtual void load_game() = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;
    virtual Board& get_board() = 0;
    virtual size_t get_undo_size() = 0;
    virtual size_t get_redo_size() = 0;

    void setup_and_add_model_board();
    void setup_and_add_model_board_paint();
    void setup_and_add_model_piece(Index index, const glm::vec3& position);
    void setup_and_add_model_nodes();
    void setup_and_add_model_node(Index index, const glm::vec3& position);
    void setup_piece_on_node(Index index, Index node_index);
    void setup_camera();
    void setup_and_add_turn_indicator();
    void setup_and_add_timer_text();
    void setup_wait_indicator();
    void setup_computer_thinking_indicator();
    void setup_light_bulb();

    void initialize_piece(Index index, std::shared_ptr<gl::Texture> diffuse_texture);
    void initialize_piece_no_normal(Index index, std::shared_ptr<gl::Texture> diffuse_texture);

    void release_piece_material_instances();
    void change_skybox();
    void change_board_paint_texture();

    void update_listener();
    void update_cursor();
    void update_turn_indicator();
    void update_wait_indicator();
    void update_computer_thinking_indicator();
    void update_timer_text();
    void update_after_human_move(bool did_action, bool switched_turn, bool must_take_or_took_piece);
    void update_after_computer_move(bool switched_turn);
    void update_game_state();
    void update_all_imgui();

    void set_skybox(Skybox skybox);
    void set_board_paint_texture();

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

    // ImGui-related
    void imgui_initialize();
    void imgui_reset();

    void imgui_draw_menu_bar();
    void imgui_draw_info();
    void imgui_draw_game_over();
    void imgui_draw_about();
    void imgui_draw_could_not_load_game();
    void imgui_draw_no_last_game();
    void imgui_draw_rules_standard_game();
    void imgui_draw_rules_jump_variant();
    void imgui_draw_rules_jump_plus_variant();
    void imgui_draw_debug();

    bool hovering_gui = false;
    bool can_undo = false;
    bool can_redo = false;
    bool show_info = false;

    WindowImGui window = WindowImGui::None;

    struct DummyFunc {
        constexpr void operator()() {}
    };

    void imgui_draw_game_over_message(std::string_view message1, std::string_view message2);
    void imgui_draw_window(const char* title, const std::function<void()>& contents,
            const std::function<void()>& ok_callback = DummyFunc {});
    void imgui_initialize_options();

    std::string last_save_game_date = save_load::NO_LAST_GAME;

    std::string info_file_path;
    std::string save_game_file_path;

    ImGuiWindowFlags window_flags = 0;

#ifdef NM3D_PLATFORM_DEBUG
    static constexpr size_t FRAMES_SIZE = 100;
    std::vector<float> frames = std::vector<float> {FRAMES_SIZE};
    size_t index = 0;
#endif
};
