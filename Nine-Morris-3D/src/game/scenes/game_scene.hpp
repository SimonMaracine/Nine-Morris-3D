#pragma once

#include <engine/nine_morris_3d.hpp>

#include "game/point_camera_controller.hpp"

struct GameScene : public sm::Scene {
    GameScene()
        : sm::Scene("loading") {}

    void on_start() override;
    void on_update() override;
    void on_imgui_update() override;

    void on_window_resized(const sm::WindowResizedEvent& event);

    sm::Camera cam;
    PointCameraController cam_controller;
    sm::DirectionalLight directional_light;
    sm::PointLight point_light;

    sm::Renderable teapot;
};
