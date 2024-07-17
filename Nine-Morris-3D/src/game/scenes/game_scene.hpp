#pragma once

#include <vector>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/point_camera_controller.hpp"

struct GameScene : sm::ApplicationScene {
    explicit GameScene(sm::Ctx& ctx)
        : sm::ApplicationScene(ctx, "game") {}

    void on_start() override;
    void on_stop() override;
    void on_update() override;
    void on_fixed_update() override;
    void on_imgui_update() override;

    void on_window_resized(const sm::WindowResizedEvent& event);

    void setup_ground();
    void setup_dragon();
    void setup_teapot();
    void setup_cube();
    void setup_brick();
    void setup_lamp();
    void setup_barrel();
    void setup_textured_bricks();
    void setup_texts();
    void setup_quads();
    void setup_skybox();
    void setup_lights();

    sm::Camera cam;
    sm::Camera2D cam_2d;
    PointCameraController cam_controller;
    sm::DirectionalLight directional_light;
    sm::PointLight point_light;
    sm::ShadowBox shadow_box;

    sm::Renderable ground;
    sm::Renderable dragon1;
    sm::Renderable dragon2;
    sm::Renderable teapot;
    sm::Renderable cube;
    sm::Renderable brick;
    sm::Renderable lamp_stand;
    sm::Renderable lamp_bulb;
    sm::Renderable barrel;

    std::vector<sm::Renderable> textured_bricks;

    sm::Text text1;
    sm::Text text2;
    sm::Text text3;
    sm::Text text4;
    sm::Quad wait;
    sm::Quad white;

    std::shared_ptr<sm::GlTextureCubemap> field;
    std::shared_ptr<sm::Font> sans;

    bool sky {true};
    bool blur {false};
    bool outline {true};
};
