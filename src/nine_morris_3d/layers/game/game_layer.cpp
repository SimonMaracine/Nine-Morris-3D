#include <functional>
#include <vector>
#include <algorithm>
#include <array>
#include <iterator>
#include <stdlib.h>
#include <time.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "application/application.h"
#include "application/layer.h"
#include "application/window.h"
#include "application/events.h"
#include "application/input.h"
#include "opengl/debug_opengl.h"
#include "opengl/renderer/renderer.h"
#include "opengl/renderer/texture.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/buffer.h"
#include "other/model.h"
#include "other/loader.h"
#include "other/logging.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
#include "nine_morris_3d/piece.h"
#include "nine_morris_3d/options.h"
#include "nine_morris_3d/save_load.h"

void GameLayer::on_attach() {
    options::load_options_from_file(scene->options);
    app->window->set_vsync(scene->options.vsync);

    app->storage->scene_framebuffer = Framebuffer::create(Framebuffer::Type::Scene,
            app->data.width, app->data.height, scene->options.samples, 2);

    setup_light();
    setup_board();
    setup_board_paint();
    setup_pieces();

    SPDLOG_DEBUG("Finished initializing game");
    STOP_ALLOCATION_LOG();
}

void GameLayer::on_detach() {
    SPDLOG_INFO("Closing game");

}

void GameLayer::on_bind_layers() {

}

void GameLayer::on_update(float dt) {
    scene->camera.update(mouse_wheel, dx, dy, dt);
    scene->board.move_pieces(dt);

    mouse_wheel = 0.0f;
    dx = 0.0f;
    dy = 0.0f;
}

void GameLayer::on_draw() {
    setup_shadows();

    app->storage->depth_map_framebuffer->bind();

    renderer::clear(renderer::Depth);
    renderer::set_viewport(2048, 2048);

    render_to_depth();

    app->storage->scene_framebuffer->bind();

    renderer::clear(renderer::Color | renderer::Depth | renderer::Stencil);
    renderer::set_viewport(app->data.width, app->data.height);
    renderer::set_stencil_mask_zero();

    renderer::bind_texture(app->storage->depth_map_framebuffer->get_depth_attachment(), 1);

    renderer::load_projection_view(scene->camera.projection_view_matrix);
    setup_camera();
    render_skybox();
    renderer::draw_board(scene->board);
    renderer::disable_output_to_red(1);
    renderer::draw_board_paint(scene->board.paint);
#ifndef NDEBUG
    renderer::draw_origin();
#endif
    renderer::enable_output_to_red(1);
    render_nodes();
    render_pieces();

    Framebuffer::resolve_framebuffer(app->storage->scene_framebuffer->get_id(),
            app->storage->intermediate_framebuffer->get_id(), app->data.width, app->data.height);

    app->storage->intermediate_framebuffer->bind();

    const int x = input::get_mouse_x();
    const int y = app->data.height - input::get_mouse_y();
    scene->hovered_id = app->storage->intermediate_framebuffer->read_pixel(1, x, y);

    Framebuffer::bind_default();

    renderer::clear(renderer::Color);
    renderer::draw_screen_quad(app->storage->intermediate_framebuffer->get_color_attachment(0));
}

void GameLayer::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher = Dispatcher(event);

    dispatcher.dispatch<MouseScrolledEvent>(MouseScrolled, BIND(GameLayer::on_mouse_scrolled));
    dispatcher.dispatch<MouseMovedEvent>(MouseMoved, BIND(GameLayer::on_mouse_moved));
    dispatcher.dispatch<MouseButtonPressedEvent>(MouseButtonPressed, BIND(GameLayer::on_mouse_button_pressed));
    dispatcher.dispatch<MouseButtonReleasedEvent>(MouseButtonReleased, BIND(GameLayer::on_mouse_button_released));
    dispatcher.dispatch<WindowResizedEvent>(WindowResized, BIND(GameLayer::on_window_resized));
}

bool GameLayer::on_mouse_scrolled(events::MouseScrolledEvent& event) {
    mouse_wheel = event.scroll;

    return true;
}

bool GameLayer::on_mouse_moved(events::MouseMovedEvent& event) {
    dx = last_mouse_x - event.mouse_x;
    dy = last_mouse_y - event.mouse_y;
    last_mouse_x = event.mouse_x;
    last_mouse_y = event.mouse_y;

    return true;
}

bool GameLayer::on_mouse_button_pressed(events::MouseButtonPressedEvent& event) {
    scene->board.press(scene->hovered_id);

    return false;
}

bool GameLayer::on_mouse_button_released(events::MouseButtonReleasedEvent& event) {
    if (event.button == MOUSE_BUTTON_LEFT) {
        if (scene->board.phase == Board::Phase::PlacePieces) {
            if (scene->board.should_take_piece) {
                scene->board.take_piece(scene->hovered_id);
            } else {
                scene->board.place_piece(scene->hovered_id);
            }
        } else if (scene->board.phase == Board::Phase::MovePieces) {
            if (scene->board.should_take_piece) {
                scene->board.take_piece(scene->hovered_id);
            } else {
                scene->board.select_piece(scene->hovered_id);
                scene->board.put_piece(scene->hovered_id);
            }
        }

        scene->board.release(scene->hovered_id);
    }

    return false;
}

bool GameLayer::on_window_resized(events::WindowResizedEvent& event) {
    app->storage->scene_framebuffer->resize(event.width, event.height);
    app->storage->intermediate_framebuffer->resize(event.width, event.height);
    scene->camera.update_projection((float) event.width, (float) event.height);
    app->storage->orthographic_projection_matrix = glm::ortho(0.0f, (float) event.width, 0.0f, (float) event.height);

    return false;
}

void GameLayer::render_skybox() {
    const glm::mat4& projection_matrix = scene->camera.projection_matrix;
    const glm::mat4 view_matrix = glm::mat4(glm::mat3(scene->camera.view_matrix));

    renderer::draw_skybox(projection_matrix * view_matrix);
}

void GameLayer::setup_light() {
    app->storage->board_shader->bind();
    app->storage->board_shader->set_uniform_vec3("u_light.position", scene->light.position);
    app->storage->board_shader->set_uniform_vec3("u_light.ambient", scene->light.ambient_color);
    app->storage->board_shader->set_uniform_vec3("u_light.diffuse", scene->light.diffuse_color);
    app->storage->board_shader->set_uniform_vec3("u_light.specular", scene->light.specular_color);
    app->storage->board_shader->set_uniform_vec3("u_view_position", scene->camera.position);

    app->storage->board_paint_shader->bind();
    app->storage->board_paint_shader->set_uniform_vec3("u_light.position", scene->light.position);
    app->storage->board_paint_shader->set_uniform_vec3("u_light.ambient", scene->light.ambient_color);
    app->storage->board_paint_shader->set_uniform_vec3("u_light.diffuse", scene->light.diffuse_color);
    app->storage->board_paint_shader->set_uniform_vec3("u_light.specular", scene->light.specular_color);
    app->storage->board_paint_shader->set_uniform_vec3("u_view_position", scene->camera.position);

    app->storage->piece_shader->bind();
    app->storage->piece_shader->set_uniform_vec3("u_light.position", scene->light.position);
    app->storage->piece_shader->set_uniform_vec3("u_light.ambient", scene->light.ambient_color);
    app->storage->piece_shader->set_uniform_vec3("u_light.diffuse", scene->light.diffuse_color);
    app->storage->piece_shader->set_uniform_vec3("u_light.specular", scene->light.specular_color);
    app->storage->piece_shader->set_uniform_vec3("u_view_position", scene->camera.position);
}

void GameLayer::setup_camera() {
    app->storage->board_shader->bind();
    app->storage->board_shader->set_uniform_vec3("u_view_position", scene->camera.position);

    app->storage->board_paint_shader->bind();
    app->storage->board_paint_shader->set_uniform_vec3("u_view_position", scene->camera.position);

    app->storage->piece_shader->bind();
    app->storage->piece_shader->set_uniform_vec3("u_view_position", scene->camera.position);
}

void GameLayer::setup_board() {
    app->storage->board_shader->bind();
    app->storage->board_shader->set_uniform_int("u_material.diffuse", 0);
    app->storage->board_shader->set_uniform_vec3("u_material.specular", scene->board.specular_color);
    app->storage->board_shader->set_uniform_float("u_material.shininess", scene->board.shininess);
}

void GameLayer::setup_board_paint() {
    app->storage->board_paint_shader->bind();
    app->storage->board_paint_shader->set_uniform_int("u_material.diffuse", 0);
    app->storage->board_paint_shader->set_uniform_vec3("u_material.specular", scene->board.paint.specular_color);
    app->storage->board_paint_shader->set_uniform_float("u_material.shininess", scene->board.paint.shininess);
}

void GameLayer::setup_pieces() {
    app->storage->piece_shader->bind();
    app->storage->piece_shader->set_uniform_int("u_material.diffuse", 0);
    app->storage->piece_shader->set_uniform_vec3("u_material.specular", scene->board.pieces[0].specular_color);  // TODO think about a better way
    app->storage->piece_shader->set_uniform_float("u_material.shininess", scene->board.pieces[0].shininess);
}

void GameLayer::render_pieces() {
    constexpr auto copy = [](Piece* piece) {
        return piece->active;
    };
    const auto sort = [this](Piece* lhs, Piece* rhs) {
        float distance1 = glm::length(scene->camera.position - lhs->position);
        float distance2 = glm::length(scene->camera.position - rhs->position);
        return distance1 > distance2;
    };

    std::array<Piece*, 18> pointer_pieces;
    for (unsigned int i = 0; i < 18; i++) {
        pointer_pieces[i] = &scene->board.pieces[i];
    }
    std::vector<Piece*> active_pieces;
    std::copy_if(pointer_pieces.begin(), pointer_pieces.end(), std::back_inserter(active_pieces), copy);
    std::sort(active_pieces.begin(), active_pieces.end(), sort);

    for (Piece* piece : active_pieces) {
        if (piece->selected) {
            renderer::draw_piece_with_outline(piece, piece->select_color);
        } else if (piece->show_outline && piece->id == scene->hovered_id && piece->in_use && !piece->pending_remove) {
            renderer::draw_piece_with_outline(piece, piece->hover_color);
        } else if (piece->to_take && piece->id == scene->hovered_id && piece->in_use) {
            renderer::draw_piece(piece, glm::vec3(1.0f, 0.2f, 0.2f));
        } else {
            renderer::draw_piece(piece, glm::vec3(1.0f, 1.0f, 1.0f));
        }
    }
}

void GameLayer::render_nodes() {
    for (Node& node : scene->board.nodes) {
        if (node.id == scene->hovered_id && scene->board.phase != Board::Phase::None &&
                scene->board.phase != Board::Phase::GameOver) {
            renderer::draw_node(node, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
        } else {
            renderer::draw_node(node, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        }
    }
}

void GameLayer::render_to_depth() {
    renderer::draw_to_depth(glm::vec3(0.0f), glm::vec3(0.0f), scene->board.scale, scene->board.vertex_array,
            scene->board.index_count);

    constexpr auto copy = [](Piece* piece) {
        return piece->active;
    };

    std::array<Piece*, 18> pointer_pieces;
    for (unsigned int i = 0; i < 18; i++) {
        pointer_pieces[i] = &scene->board.pieces[i];
    }
    std::vector<Piece*> active_pieces;
    std::copy_if(pointer_pieces.begin(), pointer_pieces.end(), std::back_inserter(active_pieces), copy);

    for (Piece* piece : active_pieces) {
        renderer::draw_to_depth(piece->position, piece->rotation, piece->scale, piece->vertex_array,
                piece->index_count);
    }
}

void GameLayer::setup_shadows() {
    const glm::mat4 projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 9.0f);
    const glm::mat4 view = glm::lookAt(scene->light.position / 4.0f,
            glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 light_space_matrix = projection * view;
    app->storage->shadow_shader->bind();
    app->storage->shadow_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);
    app->storage->board_shader->bind();
    app->storage->board_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);
    app->storage->board_shader->set_uniform_int("u_shadow_map", 1);
    app->storage->board_paint_shader->bind();
    app->storage->board_paint_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);
    app->storage->board_paint_shader->set_uniform_int("u_shadow_map", 1);
    app->storage->piece_shader->bind();
    app->storage->piece_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);
    app->storage->piece_shader->set_uniform_int("u_shadow_map", 1);
}

void GameLayer::restart() {


    SPDLOG_INFO("Restarted game");
}

void GameLayer::set_scene_framebuffer(int samples) {
    const int width = app->data.width;
    const int height = app->data.height;
    app->storage->scene_framebuffer = Framebuffer::create(Framebuffer::Type::Scene, width, height, samples, 2);
}

// void GameLayer::set_textures_quality(int quality) {

// }

void GameLayer::load_game() {

    SPDLOG_INFO("Loaded game");
}
