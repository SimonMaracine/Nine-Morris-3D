#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <nine_morris_3d_engine/external/glm.h++>

#include "game/point_camera_controller.hpp"

struct GameScene : sm::ApplicationScene {
    explicit GameScene(sm::Ctx& ctx)
        : sm::ApplicationScene(ctx, "loading") {}

    void on_start() override;
    void on_update() override;
    void on_imgui_update() override;

    void on_window_resized(const sm::WindowResizedEvent& event);

    void load_models();
    void load_textures();
    void load_materials();
    void load_material_instances();

    sm::Camera cam;
    sm::Camera2D cam_2d;
    PointCameraController cam_controller;
    sm::DirectionalLight directional_light;
    sm::PointLight point_light;
    sm::Shadows shadows;

    sm::Renderable ground;
    sm::Renderable dragon1;
    sm::Renderable dragon2;
    sm::Renderable teapot;
    sm::Renderable cube;
    sm::Renderable brick;
    sm::Renderable lamp_stand;
    sm::Renderable lamp_bulb;
    sm::Text text1;
    sm::Text text2;
    sm::Text text3;
    sm::Text text4;
    sm::Quad wait;
    sm::Quad white;

    bool sky {true};
    bool blur {false};
    bool outline {true};
};
