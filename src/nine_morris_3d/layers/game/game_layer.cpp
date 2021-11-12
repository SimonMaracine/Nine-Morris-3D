#include <functional>
#include <memory>
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
// #include "ecs/components.h"
// #include "ecs/systems.h"
#include "other/model.h"
#include "other/loader.h"
#include "other/logging.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
// #include "nine_morris_3d/game.h"
#include "nine_morris_3d/piece.h"
#include "nine_morris_3d/options.h"
#include "nine_morris_3d/save_load.h"

void GameLayer::on_attach() {
    options::load_options_from_file(scene->options);
    app->window->set_vsync(scene->options.vsync);

    app->storage->scene_framebuffer = Framebuffer::create(Framebuffer::Type::Scene,
            app->data.width, app->data.height, scene->options.samples, 2);
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
    const glm::mat4 projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 9.0f);  // TODO put these in a function
    const glm::mat4 view = glm::lookAt(scene->light.position / 4.0f,
            glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 light_space_matrix = projection * view;
    app->storage->shadow_shader->bind();
    app->storage->shadow_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);

    app->storage->depth_map_framebuffer->bind();

    renderer::clear(renderer::Depth);
    renderer::set_viewport(2048, 2048);

    render_to_depth();

    app->storage->scene_framebuffer->bind();

    renderer::clear(renderer::Color | renderer::Depth | renderer::Stencil);
    renderer::set_viewport(app->data.width, app->data.height);
    renderer::set_stencil_mask_zero();

    app->storage->board_shader->bind();  // TODO put these in a function
    app->storage->board_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);
    app->storage->board_shader->set_uniform_int("u_shadow_map", 1);
    app->storage->board_paint_shader->bind();
    app->storage->board_paint_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);
    app->storage->board_paint_shader->set_uniform_int("u_shadow_map", 1);
    app->storage->piece_shader->bind();
    app->storage->piece_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);
    app->storage->piece_shader->set_uniform_int("u_shadow_map", 1);
    renderer::bind_texture(app->storage->depth_map_framebuffer->get_depth_attachment(), 1);

    renderer::load_projection_view(scene->camera.projection_view_matrix);
    render_skybox();
    setup_light();  // TODO should be once in setup
    setup_board();  // TODO should be once in setup
    renderer::draw_board(scene->board);
    setup_board_paint();  // TODO should be once in setup
    setup_pieces();  // TODO should be once in setup
    render_pieces();
    renderer::disable_output_to_red(1);
    renderer::draw_board_paint(scene->board.paint);
#ifndef NDEBUG
    renderer::draw_origin();
#endif
    renderer::enable_output_to_red(1);
    render_nodes();

    Framebuffer::resolve_framebuffer(app->storage->scene_framebuffer->get_id(),
            app->storage->intermediate_framebuffer->get_id(), app->data.width, app->data.height);

    app->storage->intermediate_framebuffer->bind();

    const int x = input::get_mouse_x();
    const int y = app->data.height - input::get_mouse_y();
    scene->hovered_id = app->storage->intermediate_framebuffer->read_pixel(1, x, y);
    SPDLOG_DEBUG("Hovered ID: {}", scene->hovered_id);

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
    // systems::press(scene->registry, scene->board, scene->hovered_entity);
    scene->board.press(scene->hovered_id);

    return false;
}

bool GameLayer::on_mouse_button_released(events::MouseButtonReleasedEvent& event) {
    // auto& state = scene->registry.get<GameStateComponent>(scene->board);

    if (event.button == MOUSE_BUTTON_LEFT) {
        if (scene->board.phase == Board::Phase::PlacePieces) {
            if (scene->board.should_take_piece) {
                // systems::take_piece(scene->registry, scene->board, scene->hovered_entity);
                scene->board.take_piece(scene->hovered_id);
            } else {
                // systems::place_piece(scene->registry, scene->board, scene->hovered_entity);
                scene->board.place_piece(scene->hovered_id);
            }
        } else if (scene->board.phase == Board::Phase::MovePieces) {
            if (scene->board.should_take_piece) {
                // systems::take_piece(scene->registry, scene->board, scene->hovered_entity);
                scene->board.take_piece(scene->hovered_id);
            } else {
                // systems::select_piece(scene->registry, scene->board, scene->hovered_entity);
                // systems::put_piece(scene->registry, scene->board, scene->hovered_entity);
                scene->board.select_piece(scene->hovered_id);
                scene->board.put_piece(scene->hovered_id);
            }
        }

        // systems::release(scene->registry, scene->board);
        scene->board.release(scene->hovered_id);
    }

    return false;
}

bool GameLayer::on_window_resized(events::WindowResizedEvent& event) {
    app->storage->scene_framebuffer->resize(event.width, event.height);
    app->storage->intermediate_framebuffer->resize(event.width, event.height);
    // systems::projection_matrix(scene->registry, (float) event.width, (float) event.height);  // TODO this
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
    app->storage->piece_shader->set_uniform_vec3("u_material.specular", scene->board.pieces[0]->specular_color);  // TODO think about a better way
    app->storage->piece_shader->set_uniform_float("u_material.shininess", scene->board.pieces[0]->shininess);
}

void GameLayer::render_pieces() {
    constexpr auto copy = [](std::shared_ptr<Piece> piece) {
        return piece->active;
    };
    const auto sort = [this](std::shared_ptr<Piece> lhs, std::shared_ptr<Piece> rhs) {
        float distance1 = glm::length(scene->camera.position - lhs->position);
        float distance2 = glm::length(scene->camera.position - rhs->position);
        return distance1 > distance2;
    };

    std::vector<std::shared_ptr<Piece>> active_pieces;
    std::copy_if(scene->board.pieces.begin(), scene->board.pieces.end(), std::back_inserter(active_pieces), copy);
    std::sort(active_pieces.begin(), active_pieces.end(), sort);

    for (auto piece : active_pieces) {
        if (piece->selected) {
            renderer::draw_piece_with_outline(piece, piece->select_color);
        } else if (piece->show_outline && piece->id == scene->hovered_id && piece->in_use && !piece->pending_remove) {
            renderer::draw_piece_with_outline(piece, piece->hover_color);
        } else if (piece->to_take && piece->id == scene->hovered_id && piece->in_use) {
            renderer::draw_piece(piece, glm::vec3(0.9f, 0.1f, 0.1f));
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

    constexpr auto copy = [](std::shared_ptr<Piece> piece) {
        return piece->active;
    };

    std::vector<std::shared_ptr<Piece>> active_pieces;
    std::copy_if(scene->board.pieces.begin(), scene->board.pieces.end(), std::back_inserter(active_pieces), copy);

    for (auto piece : active_pieces) {
        renderer::draw_to_depth(piece->position, piece->rotation, piece->scale, piece->vertex_array,
                piece->index_count);
    }
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

// Rc<Buffer> GameLayer::create_ids_buffer(unsigned int vertices_size, entt::entity entity) {
//     std::vector<int> array;
//     array.resize(vertices_size);
//     for (unsigned int i = 0; i < array.size(); i++) {
//         array[i] = (int) entt::to_integral(entity);
//     }
//     Rc<Buffer> buffer = Buffer::create(array.data(), array.size() * sizeof(int));

//     return buffer;
// }

// Rc<VertexArray> GameLayer::create_entity_vertex_array(Rc<model::Mesh<Vertex>> mesh, entt::entity entity) {
//     Rc<Buffer> vertices = Buffer::create(mesh->vertices.data(), mesh->vertices.size() * sizeof(model::Vertex));

//     Rc<Buffer> ids = create_ids_buffer(mesh->vertices.size(), entity);

//     BufferLayout layout;
//     layout.add(0, BufferLayout::Type::Float, 3);
//     layout.add(1, BufferLayout::Type::Float, 2);
//     layout.add(2, BufferLayout::Type::Float, 3);

//     BufferLayout layout2;
//     layout2.add(3, BufferLayout::Type::Int, 1);

//     Rc<Buffer> index_buffer = Buffer::create_index(mesh->indices.data(), mesh->indices.size() * sizeof(unsigned int));

//     Rc<VertexArray> vertex_array = VertexArray::create();
//     index_buffer->bind();
//     vertex_array->add_buffer(vertices, layout);
//     vertex_array->add_buffer(ids, layout2);
//     vertex_array->hold_index_buffer(index_buffer);

//     VertexArray::unbind();

//     return vertex_array;
// }

// void GameLayer::build_board() {;
//     scene->board = scene->registry.create();

//     Rc<VertexArray> vertex_array = create_entity_vertex_array(app->asset_manager.get_mesh(0), scene->board);

//     if (!app->storage->board_diffuse_texture) {
//         if (scene->options.texture_quality == 0) {
//             app->storage->board_diffuse_texture = Texture::create(app->asset_manager.get_texture_flipped(5), true, -2.0f);
//         } else {
//             app->storage->board_diffuse_texture = Texture::create(app->asset_manager.get_texture_flipped(6), true, -2.0f);
//         }
//     }

//     auto& transform = scene->registry.emplace<TransformComponent>(scene->board);
//     transform.scale = 20.0f;

//     scene->registry.emplace<MeshComponent>(scene->board, vertex_array, app->asset_manager.get_mesh(0)->indices.size());
//     scene->registry.emplace<MaterialComponent>(scene->board, glm::vec3(0.25f), 8.0f);
//     scene->registry.emplace<ShadowComponent>(scene->board);

//     scene->registry.emplace<GameStateComponent>(scene->board, scene->nodes);
//     scene->registry.emplace<MovesHistoryComponent>(scene->board);

//     SPDLOG_DEBUG("Built board entity {}", scene->board);
// }

// void GameLayer::build_board_paint() {
//     Rc<Buffer> vertices = Buffer::create(app->asset_manager.get_mesh(1)->vertices.data(),
//                                          app->asset_manager.get_mesh(1)->vertices.size() * sizeof(model::Vertex));
//     Rc<VertexArray> vertex_array = VertexArray::create();

//     BufferLayout layout;
//     layout.add(0, BufferLayout::Type::Float, 3);
//     layout.add(1, BufferLayout::Type::Float, 2);
//     layout.add(2, BufferLayout::Type::Float, 3);

//     Rc<Buffer> index_buffer = Buffer::create_index(app->asset_manager.get_mesh(1)->indices.data(),
//                                                    app->asset_manager.get_mesh(1)->indices.size() * sizeof(unsigned int));

//     index_buffer->bind();
//     vertex_array->add_buffer(vertices, layout);
//     vertex_array->hold_index_buffer(index_buffer);

//     VertexArray::unbind();

//     if (!app->storage->board_paint_texture) {
//         if (scene->options.texture_quality == 0) {
//             app->storage->board_paint_texture = Texture::create(app->asset_manager.get_texture_flipped(7), true, -1.0f);
//         } else {
//             app->storage->board_paint_texture = Texture::create(app->asset_manager.get_texture_flipped(8), true, -1.0f);
//         }
//     }

//     entt::entity board_paint = scene->registry.create();

//     auto& transform = scene->registry.emplace<TransformComponent>(board_paint);
//     transform.position = glm::vec3(0.0f, 0.062f, 0.0f);
//     transform.scale = 20.0f;

//     scene->registry.emplace<MeshComponent>(board_paint, vertex_array, app->asset_manager.get_mesh(1)->indices.size());
//     scene->registry.emplace<MaterialComponent>(board_paint, glm::vec3(0.25f), 8.0f);
//     scene->registry.emplace<BoardPaintComponent>(board_paint);

//     SPDLOG_DEBUG("Built board paint entity {}", board_paint);
// }

// void GameLayer::build_camera() {
//     scene->camera = scene->registry.create();

//     auto& transform = scene->registry.emplace<TransformComponent>(scene->camera);
//     transform.rotation = glm::vec3(47.0f, 0.0f, 0.0f);

//     scene->registry.emplace<CameraComponent>(scene->camera,
//         glm::perspective(glm::radians(45.0f), (float) app->data.width / (float) app->data.height, 0.1f, 70.0f),
//         glm::vec3(0.0f), 8.0f);
//     scene->registry.emplace<CameraMoveComponent>(scene->camera);

//     SPDLOG_DEBUG("Built camera entity {}", scene->camera);
// }

// void GameLayer::build_skybox() {
//     Rc<Buffer> positions = Buffer::create(skybox_points, sizeof(skybox_points));

//     BufferLayout layout;
//     layout.add(0, BufferLayout::Type::Float, 3);

//     app->storage->skybox_vertex_array = VertexArray::create();
//     app->storage->skybox_vertex_array->add_buffer(positions, layout);
//     VertexArray::unbind();

//     if (!app->storage->skybox_texture) {
//         std::array<Rc<TextureData>, 6> data = {
//             app->asset_manager.get_texture(16),
//             app->asset_manager.get_texture(17),
//             app->asset_manager.get_texture(18),
//             app->asset_manager.get_texture(19),
//             app->asset_manager.get_texture(20),
//             app->asset_manager.get_texture(21)
//         };
//         app->storage->skybox_texture = Texture3D::create(data);
//     }

//     entt::entity skybox = scene->registry.create();

//     scene->registry.emplace<SkyboxComponent>(skybox);

//     SPDLOG_DEBUG("Built skybox entity {}", skybox);
// }

// void GameLayer::build_piece(int id, Piece type, Rc<model::Mesh<Vertex>> mesh,
//                             Rc<Texture> diffuse_texture, const glm::vec3& position) {
//     entt::entity piece = scene->registry.create();
//     scene->pieces[id] = piece;

//     Rc<VertexArray> vertex_array = create_entity_vertex_array(mesh, piece);

//     int random_rotation = rand() % 360;

//     auto& transform = scene->registry.emplace<TransformComponent>(piece);
//     transform.position = position;
//     transform.rotation = glm::vec3(0.0f, glm::radians((float) random_rotation), 0.0f);
//     transform.scale = 20.0f;

//     scene->registry.emplace<MeshComponent>(piece, vertex_array, mesh->indices.size());
//     scene->registry.emplace<MaterialComponent>(piece, glm::vec3(0.25f), 8.0f);
//     scene->registry.emplace<PieceTextureComponent>(piece, diffuse_texture);
//     scene->registry.emplace<OutlineComponent>(piece, app->storage->outline_shader, glm::vec3(1.0f, 0.0f, 0.0f));
//     scene->registry.emplace<ShadowComponent>(piece);

//     scene->registry.emplace<PieceComponent>(piece, id, type);
//     scene->registry.emplace<MoveComponent>(piece);

//     SPDLOG_DEBUG("Built piece entity {}", piece);
// }

// void GameLayer::build_directional_light() {
//     entt::entity directional_light = scene->registry.create();

//     auto& transform = scene->registry.emplace<TransformComponent>(directional_light);
//     transform.position = LIGHT_POSITION;
//     transform.scale = 0.3f;

//     scene->registry.emplace<LightComponent>(directional_light, glm::vec3(0.15f), glm::vec3(0.8f), glm::vec3(1.0f));
// #ifndef NDEBUG
//     scene->registry.emplace<QuadTextureComponent>(directional_light, app->storage->light_texture);
// #endif

//     SPDLOG_DEBUG("Built directional light entity {}", directional_light);
// }

// #ifndef NDEBUG
// void GameLayer::build_origin() {
//     entt::entity origin = scene->registry.create();

//     scene->registry.emplace<OriginComponent>(origin);

//     SPDLOG_DEBUG("Built origin entity {}", origin);
// }
// #endif

// void GameLayer::build_node(int index, const glm::vec3& position) {
//     entt::entity node = scene->registry.create();
//     scene->nodes[index] = node;

//     Rc<Buffer> vertices = Buffer::create(app->asset_manager.get_mesh_p(2)->vertices.data(),
//                                          app->asset_manager.get_mesh_p(2)->vertices.size() * sizeof(glm::vec3));

//     Rc<Buffer> ids = create_ids_buffer(app->asset_manager.get_mesh_p(2)->vertices.size(), node);

//     BufferLayout layout;
//     layout.add(0, BufferLayout::Type::Float, 3);
//     BufferLayout layout2;
//     layout2.add(1, BufferLayout::Type::Int, 1);

//     Rc<Buffer> index_buffer = Buffer::create_index(app->asset_manager.get_mesh_p(2)->indices.data(),
//                                                    app->asset_manager.get_mesh_p(2)->indices.size() * sizeof(unsigned int));

//     Rc<VertexArray> vertex_array = VertexArray::create();
//     index_buffer->bind();
//     vertex_array->add_buffer(vertices, layout);
//     vertex_array->add_buffer(ids, layout2);
//     vertex_array->hold_index_buffer(index_buffer);

//     VertexArray::unbind();

//     auto& transform = scene->registry.emplace<TransformComponent>(node);
//     transform.position = position;
//     transform.scale = 20.0f;

//     scene->registry.emplace<MeshComponent>(node, vertex_array, app->asset_manager.get_mesh_p(2)->indices.size());

//     scene->registry.emplace<NodeComponent>(node, index);

//     SPDLOG_DEBUG("Built node entity {}", node);
// }

// void GameLayer::build_turn_indicator() {
//     entt::entity turn_indicator = scene->registry.create();

//     if (!app->storage->white_indicator_texture) {
//         app->storage->white_indicator_texture = Texture::create(app->asset_manager.get_texture_flipped(9), false);
//         app->storage->black_indicator_texture = Texture::create(app->asset_manager.get_texture_flipped(10), false);
//     }

//     auto& transform = scene->registry.emplace<TransformComponent>(turn_indicator);
//     transform.position = glm::vec3(app->data.width - 90, app->data.height - 115, 0.0f);

//     scene->registry.emplace<TurnIndicatorComponent>(turn_indicator);

//     SPDLOG_DEBUG("Built turn indicator entity {}", turn_indicator);
// }

// void GameLayer::rebuild_board_after_load() {
//     Rc<VertexArray> vertex_array = create_entity_vertex_array(app->asset_manager.get_mesh(0), scene->board);

//     scene->registry.emplace<MeshComponent>(scene->board, vertex_array, app->asset_manager.get_mesh(0)->indices.size());
//     scene->registry.emplace<MaterialComponent>(scene->board, glm::vec3(0.25f), 8.0f);
//     scene->registry.emplace<ShadowComponent>(scene->board);
// }

// void GameLayer::rebuild_camera_after_load() {
//     scene->registry.emplace<CameraMoveComponent>(scene->camera);
// }

// void GameLayer::rebuild_piece_after_load(entt::entity piece, Rc<model::Mesh<Vertex>> mesh,
//                                          Rc<Texture> diffuse_texture) {
//     Rc<VertexArray> vertex_array = create_entity_vertex_array(mesh, piece);

//     scene->registry.emplace<MeshComponent>(piece, vertex_array, mesh->indices.size());
//     scene->registry.emplace<MaterialComponent>(piece, glm::vec3(0.25f), 8.0f);
//     scene->registry.emplace<PieceTextureComponent>(piece, diffuse_texture);
//     scene->registry.emplace<OutlineComponent>(piece, app->storage->outline_shader, glm::vec3(1.0f, 0.0f, 0.0f));
//     scene->registry.emplace<ShadowComponent>(piece);

//     scene->registry.emplace<MoveComponent>(piece);
// }

// void GameLayer::rebuild_node_after_load(entt::entity node) {
//     Rc<Buffer> vertices = Buffer::create(app->asset_manager.get_mesh_p(2)->vertices.data(),
//                                          app->asset_manager.get_mesh_p(2)->vertices.size() * sizeof(glm::vec3));

//     Rc<Buffer> ids = create_ids_buffer(app->asset_manager.get_mesh_p(2)->vertices.size(), node);

//     BufferLayout layout;
//     layout.add(0, BufferLayout::Type::Float, 3);
//     BufferLayout layout2;
//     layout2.add(1, BufferLayout::Type::Int, 1);

//     Rc<Buffer> index_buffer = Buffer::create_index(app->asset_manager.get_mesh_p(2)->indices.data(),
//                                                    app->asset_manager.get_mesh_p(2)->indices.size() * sizeof(unsigned int));

//     Rc<VertexArray> vertex_array = VertexArray::create();
//     index_buffer->bind();
//     vertex_array->add_buffer(vertices, layout);
//     vertex_array->add_buffer(ids, layout2);
//     vertex_array->hold_index_buffer(index_buffer);

//     VertexArray::unbind();

//     scene->registry.emplace<MeshComponent>(node, vertex_array, app->asset_manager.get_mesh_p(2)->indices.size());
// }
