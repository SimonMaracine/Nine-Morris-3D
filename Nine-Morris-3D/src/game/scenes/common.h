#pragma once

#include <engine/engine_application.h>
#include <engine/engine_graphics.h>

#include "game/point_camera_controller.h"
#include "other/data.h"
#include "other/constants.h"

void initialize_board(Application* app);
void initialize_board_paint(Application* app);
void initialize_pieces(Application* app);
void initialize_piece(
    Application* app,
    size_t index,
    std::shared_ptr<gl::Texture> diffuse_texture,
    std::shared_ptr<gl::Buffer> vertex_buffer,
    std::shared_ptr<gl::IndexBuffer> index_buffer
);
void initialize_nodes(Application* app);
void initialize_node(
    Application* app,
    size_t index,
    std::shared_ptr<gl::Buffer> vertex_buffer,
    std::shared_ptr<gl::IndexBuffer> index_buffer
);

void initialize_board_no_normal(Application* app);
void initialize_board_paint_no_normal(Application* app);
void initialize_pieces_no_normal(Application* app);
void initialize_piece_no_normal(
    Application* app,
    size_t index,
    std::shared_ptr<gl::Texture> diffuse_texture,
    std::shared_ptr<gl::Buffer> vertex_buffer,
    std::shared_ptr<gl::IndexBuffer> index_buffer
);

void initialize_keyboard_controls(Application* app);
void initialize_light_bulb(Application* app);

void initialize_skybox(Application* app);
void initialize_light(Application* app);
void initialize_indicators_textures(Application* app);

void change_skybox(Application* app);
void change_board_paint_texture(Application* app);

template<typename S>
void setup_camera(Application* app, S& scene) {
    auto& data = app->user_data<Data>();

    constexpr float PITCH = 47.0f;
    constexpr float DISTANCE_TO_POINT = 8.0f;

    scene.camera = Camera {};

    scene.camera_controller = PointCameraController {
        &scene.camera,
        app->data().width,
        app->data().height,
        LENS_FOV,
        LENS_NEAR,
        LENS_FAR,
        glm::vec3(0.0f),
        DISTANCE_TO_POINT,
        PITCH,
        data.options.sensitivity
    };

    scene.default_camera_position = scene.camera_controller.get_position();

    scene.camera_controller = PointCameraController {
        &scene.camera,
        app->data().width,
        app->data().height,
        LENS_FOV,
        LENS_NEAR,
        LENS_FAR,
        glm::vec3(0.0f),
        DISTANCE_TO_POINT + 0.7f,
        PITCH,
        data.options.sensitivity
    };

    app->renderer->set_camera_controller(&scene.camera_controller);
    app->openal->get_listener().set_position(scene.camera_controller.get_position());
    app->openal->get_listener().set_look_at_and_up(
        scene.camera_controller.get_point() - scene.camera_controller.get_position(),
        glm::rotate(UP_VECTOR, scene.camera_controller.get_rotation().y, UP_VECTOR)
    );

    DEB_DEBUG("Setup camera");
}

template<typename S>
void update_cursor(Application* app, S& scene) {
    auto& data = app->user_data<Data>();

    if (data.options.custom_cursor) {
        if (scene.board.must_take_piece) {
            app->window->set_cursor(data.cross_cursor);

            app->res.quad["keyboard_controls"_h]->texture = app->res.texture["keyboard_controls_cross"_h];
        } else {
            app->window->set_cursor(data.arrow_cursor);

            app->res.quad["keyboard_controls"_h]->texture = app->res.texture["keyboard_controls_default"_h];
        }
    }
}

template<typename S>
void set_skybox(Application* app, S& scene, Skybox skybox) {
    if (skybox == Skybox::None) {
        app->renderer->set_skybox(nullptr);
        return;
    }

    auto& data = app->user_data<Data>();

    scene.skybox_loader->start_loading_thread(data.launcher_options.texture_quality, data.options.skybox);
}

template<typename S>
void set_board_paint_texture(Application* app, S& scene) {
    auto& data = app->user_data<Data>();

    scene.board_paint_texture_loader->start_loading_thread(
        data.launcher_options.texture_quality,
        data.options.labeled_board
    );
}
