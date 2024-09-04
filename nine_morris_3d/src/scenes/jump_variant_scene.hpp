// #pragma once

// #include <vector>

// #include <nine_morris_3d_engine/nine_morris_3d.hpp>

// // #include "game/standard_board.hpp"
// #include "scenes/game_scene.hpp"
// #include "point_camera_controller.hpp"
// #include "turn_indicator.hpp"
// #include "timer.hpp"
// #include "ui.hpp"

// class JumpVariantScene : public sm::ApplicationScene, public GameScene {
// public:
//     explicit JumpVariantScene(sm::Ctx& ctx)
//         : sm::ApplicationScene(ctx) {}

//     SM_SCENE_NAME("jump_variant")

//     void on_start() override;
//     void on_stop() override;
//     void on_update() override;
//     void on_fixed_update() override;
//     void on_imgui_update() override;

//     PointCameraController& get_camera_controller() override;
//     void load_and_set_skybox() override;
//     void load_and_set_board_paint_texture() override;
// private:
//     void on_window_resized(const sm::WindowResizedEvent& event);
//     void on_key_released(const sm::KeyReleasedEvent& event);
//     void on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event);
//     void on_mouse_button_released(const sm::MouseButtonReleasedEvent& event);

//     void setup_camera();
//     void setup_skybox();
//     void setup_lights();
//     void setup_turn_indicator();
//     void setup_timer();
//     void setup_renderables();
//     sm::Renderable setup_board() const;
//     sm::Renderable setup_board_paint() const;
//     std::vector<sm::Renderable> setup_nodes() const;
//     std::vector<sm::Renderable> setup_white_pieces() const;
//     std::vector<sm::Renderable> setup_black_pieces() const;

//     void load_skybox() const;
//     void load_board_paint_texture() const;

//     std::shared_ptr<sm::GlTextureCubemap> get_skybox_texture_cubemap() const;
//     std::shared_ptr<sm::GlTexture> get_board_paint_texture(const sm::TextureSpecification& specification) const;

//     bool m_game_started {false};
//     glm::vec3 m_default_camera_position {};

//     sm::Camera m_cam;
//     sm::Camera2D m_cam_2d;
//     PointCameraController m_cam_controller;
//     sm::DirectionalLight m_directional_light;
//     sm::ShadowBox m_shadow_box;

//     // StandardBoard m_board;
//     TurnIndicator m_turn_indicator;
//     Timer m_timer;

//     std::shared_ptr<sm::GlTextureCubemap> m_skybox;

//     Ui m_ui;
// };
