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
    virtual void load_and_set_skybox() = 0;
    virtual void load_and_set_board_paint_texture() = 0;
};
