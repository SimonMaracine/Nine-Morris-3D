#pragma once

#include <engine/nine_morris_3d.hpp>

#include "game/point_camera_controller.hpp"

struct GameScene : sm::Scene {
    explicit GameScene(sm::Ctx& ctx)
        : sm::Scene(ctx, "loading") {}

    void on_start() override;
    void on_update() override;
    void on_imgui_update() override;

    void on_window_resized(const sm::WindowResizedEvent& event);

    sm::Camera cam;
    sm::Camera2D cam_2d;
    PointCameraController cam_controller;
    sm::DirectionalLight directional_light;
    sm::PointLight point_light;

    glm::vec2 pos {};
    float scl {1.0f};
    bool blur {true};
    bool outline {true};

    sm::Renderable teapot;
};
