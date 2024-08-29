#pragma once

#include "point_camera_controller.hpp"

struct GameScene {
    GameScene() = default;
    virtual ~GameScene() = default;

    GameScene(const GameScene&) = default;
    GameScene& operator=(const GameScene&) = default;
    GameScene(GameScene&&) = default;
    GameScene& operator=(GameScene&&) = default;

    virtual PointCameraController& get_camera_controller() = 0;
};
