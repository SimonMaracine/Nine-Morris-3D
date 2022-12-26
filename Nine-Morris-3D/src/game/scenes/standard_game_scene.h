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
        : Scene("game") {}

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

    std::shared_ptr<gl::Buffer> create_id_buffer(size_t vertices_size, identifier::Id id, hs hash);

    void initialize_board();
    void initialize_board_paint();
    void initialize_pieces();
    void initialize_piece(
        size_t index,
        std::shared_ptr<Mesh<PTNT>> mesh,
        std::shared_ptr<gl::Texture> diffuse_texture,
        std::shared_ptr<gl::Buffer> vertex_buffer,
        std::shared_ptr<gl::IndexBuffer> index_buffer);
    void initialize_nodes();
    void initialize_node(size_t index, std::shared_ptr<gl::Buffer> vertex_buffer, std::shared_ptr<gl::IndexBuffer> index_buffer);

    void initialize_board_no_normal();
    void initialize_board_paint_no_normal();
    void initialize_pieces_no_normal();
    void initialize_piece_no_normal(
        size_t index,
        std::shared_ptr<Mesh<PTN>> mesh,
        std::shared_ptr<gl::Texture> diffuse_texture,
        std::shared_ptr<gl::Buffer> vertex_buffer,
        std::shared_ptr<gl::IndexBuffer> index_buffer);

    void initialize_keyboard_controls();
    void initialize_light_bulb();

    void setup_and_add_model_board();
    void setup_and_add_model_board_paint();
    void setup_and_add_model_pieces();
    void setup_and_add_model_piece(size_t index, const glm::vec3& position, std::shared_ptr<gl::IndexBuffer> index_buffer);
    void setup_and_add_model_nodes();
    void setup_and_add_model_node(size_t index, const glm::vec3& position);

    void setup_entities();
    void initialize_skybox();
    void initialize_light();
    void setup_camera();
    void initialize_indicators_textures();
    void setup_widgets();

    void update_game_state();
    void update_timer_text();
    void update_turn_indicator();
    void update_wait_indicator();
    void update_computer_thinking_indicator();
    void update_cursor();

    void update_after_human_move(bool did_action, bool switched_turn, bool must_take_piece_or_took_piece);
    void update_after_computer_move(bool switched_turn);

    void set_skybox(Skybox skybox);  // TODO maybe move this out
    void change_skybox();

    void set_board_paint_texture();  // TODO maybe move this out
    void change_board_paint_texture();

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

    bool made_first_move = false;
    bool show_keyboard_controls = false;

    glm::vec3 default_camera_position = glm::vec3(0.0f);
    std::shared_ptr<music::MusicTrack> current_music_track;

    // GUI-related
    Timer timer;
    bool show_wait_indicator = false;
    bool show_computer_thinking_indicator = false;
};
