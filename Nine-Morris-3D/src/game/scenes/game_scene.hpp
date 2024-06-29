#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <nine_morris_3d_engine/external/glm.h++>

#include "game/point_camera_controller.hpp"

struct GameScene : sm::Scene {
    explicit GameScene(sm::Ctx& ctx)
        : sm::Scene(ctx, "loading") {}

    void on_start() override;
    void on_update() override;
    void on_imgui_update() override;

    void on_window_resized(const sm::WindowResizedEvent& event);

    void load_models();
    void load_textures();
    void load_materials();
    void load_material_instances();

    // TODO move to engine
    // TODO also draw lights and other debug things from engine
    void draw_bounding_box(float left, float right, float bottom, float top, float near, float far, const glm::vec3& position, const glm::vec3& orientation);

    sm::Camera cam;
    sm::Camera2D cam_2d;
    PointCameraController cam_controller;
    sm::DirectionalLight directional_light;
    sm::PointLight point_light;

    float shadow_left {-30.0f};
    float shadow_right {30.0f};
    float shadow_bottom {-30.0f};
    float shadow_top {30.0f};
    float shadow_far {35.0f};

    glm::vec2 pos {};
    float scl {1.0f};
    bool sky {true};
    bool blur {false};
    bool outline {true};
};
