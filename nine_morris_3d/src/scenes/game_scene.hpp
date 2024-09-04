#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "point_camera_controller.hpp"
#include "ui.hpp"

class GameScene : public sm::ApplicationScene {
public:
    explicit GameScene(sm::Ctx& ctx)
        : sm::ApplicationScene(ctx) {}

    PointCameraController& get_camera_controller() { return m_cam_controller; }

    // Call these in the child scene class
    void on_start() override;
    void on_stop() override;

    virtual void load_and_set_skybox() = 0;
    virtual void load_and_set_board_paint_texture() = 0;
protected:
    void on_window_resized(const sm::WindowResizedEvent& event);

    void setup_camera();
    void setup_skybox();
    void setup_lights();

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
