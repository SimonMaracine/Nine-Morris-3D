#pragma once

#include <vector>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/board.hpp"
#include "point_camera_controller.hpp"
#include "ui.hpp"

class GameScene : public sm::ApplicationScene {
public:
    explicit GameScene(sm::Ctx& ctx)
        : sm::ApplicationScene(ctx) {}

    // Call these in the child scene class
    void on_start() override;
    void on_stop() override;
    void on_update() override;
    void on_fixed_update() override;
    void on_imgui_update() override;

    PointCameraController& get_camera_controller() { return m_cam_controller; }

    void load_and_set_skybox();
    void load_and_set_board_paint_texture();

    virtual BoardObj& get_board() = 0;
protected:
    void on_window_resized(const sm::WindowResizedEvent& event);

    void setup_camera();
    void setup_skybox();
    void setup_lights();

    sm::Renderable setup_board() const;
    sm::Renderable setup_board_paint() const;
    std::vector<sm::Renderable> setup_nodes(unsigned int count) const;
    std::vector<sm::Renderable> setup_white_pieces(unsigned int count) const;
    std::vector<sm::Renderable> setup_black_pieces(unsigned int count) const;

    void load_skybox() const;
    void load_board_paint_texture() const;

    std::shared_ptr<sm::GlTextureCubemap> get_skybox_texture_cubemap() const;
    std::shared_ptr<sm::GlTexture> get_board_paint_texture(const sm::TextureSpecification& specification) const;

    bool m_game_started {false};
    glm::vec3 m_default_camera_position {};

    PointCameraController m_cam_controller;
    Ui m_ui;

    sm::Camera m_cam;
    sm::Camera2D m_cam_2d;
    sm::DirectionalLight m_directional_light;
    sm::ShadowBox m_shadow_box;

    std::shared_ptr<sm::GlTextureCubemap> m_skybox;
};
