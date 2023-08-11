#pragma once

#include <engine/nine_morris_3d.hpp>

#include <game/point_camera_controller.hpp>

struct GameScene : public sm::Scene {
    GameScene()
        : sm::Scene("loading") {}

    virtual void on_start() override;
    virtual void on_update() override;

    sm::Camera cam;
    PointCameraController cam_controller;
};
