#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/scenes/imgui_layer.h"
#include "game/boards/standard_board.h"
#include "game/undo_redo_state.h"
#include "game/keyboard_controls.h"
#include "game/game_context.h"
#include "game/constants.h"
#include "game/timer.h"
#include "game/minimax/minimax_thread.h"
#include "game/assets_load.h"

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

    std::shared_ptr<Buffer> create_id_buffer(size_t vertices_size, hover::Id id, hs hash);

    void initialize_rendering_board();
    void initialize_rendering_board_paint();
    void initialize_rendering_pieces();
    void initialize_rendering_piece(
        size_t index,
        std::shared_ptr<Mesh<PTNT>> mesh,
        std::shared_ptr<Texture> diffuse_texture,
        std::shared_ptr<Buffer> vertices,
        std::shared_ptr<IndexBuffer> indices);
    void initialize_rendering_nodes();
    void initialize_rendering_node(size_t index, std::shared_ptr<Buffer> vertices, std::shared_ptr<IndexBuffer> indices);

    void setup_and_add_model_board();
    void setup_and_add_model_board_paint();
    void setup_and_add_model_pieces();
    void setup_and_add_model_piece(size_t index, std::shared_ptr<Mesh<PTNT>> mesh);
    void setup_and_add_model_nodes();
    void setup_and_add_model_node(size_t index, const glm::vec3& position);

    void setup_entities();
    void setup_entity_ids();
    void setup_skybox();
    void setup_light();
    void setup_camera();

    void save_game();

    // ImGui
    ImGuiLayer<StandardGameScene> imgui_layer;

    // Game-related
    Camera camera;
    StandardBoard board;
    UndoRedoState undo_redo_state;
    KeyboardControls keyboard;
    GameContext game;
    MinimaxThread minimax_thread;

    bool made_first_move = false;
    bool show_keyboard_controls = false;

    glm::vec3 default_camera_position = glm::vec3(0.0f);

    std::unique_ptr<assets_load::CustomLoader> loader;

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

#ifdef PLATFORM_GAME_DEBUG
    std::shared_ptr<Texture> light_bulb_texture;
    std::shared_ptr<Renderer::Quad> light_bulb_quad;
#endif
};
