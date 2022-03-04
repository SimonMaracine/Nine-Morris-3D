#include <functional>
#include <vector>
#include <algorithm>
#include <array>
#include <iterator>
#include <string>
#include <stdexcept>
#include <memory>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "application/layer.h"
#include "application/window.h"
#include "application/events.h"
#include "application/input.h"
#include "application/platform.h"
#include "graphics/debug_opengl.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/buffer.h"
#include "graphics/renderer/buffer_layout.h"
#include "graphics/renderer/texture.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/framebuffer.h"
#include "graphics/renderer/framebuffer_reader.h"
#include "graphics/renderer/buffer.h"
#include "graphics/renderer/light.h"
#include "graphics/renderer/camera.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/piece.h"
#include "nine_morris_3d/board.h"
#include "nine_morris_3d/options.h"
#include "nine_morris_3d/save_load.h"
#include "nine_morris_3d/assets.h"
#include "other/mesh.h"
#include "other/loader.h"
#include "other/logging.h"
#include "other/texture_data.h"

const DirectionalLight LIGHT_FIELD = {
    glm::vec3(5.7f, 8.4f, 12.4f),
    glm::vec3(0.3f),
    glm::vec3(1.0f),
    glm::vec3(0.9f)
};

const DirectionalLight LIGHT_AUTUMN = {
    glm::vec3(-4.4f, 11.0f, 6.4f),
    glm::vec3(0.15f),
    glm::vec3(0.9f),
    glm::vec3(0.85f)
};

void GameLayer::on_attach() {
    board_state_history = std::make_shared<std::vector<Board>>();
    setup_board();
    setup_board_paint();

    setup_pieces();
    setup_nodes();

    set_camera();
    set_skybox();
    set_light();

    app->window->set_vsync(app->options.vsync);
    app->window->set_cursor(app->options.custom_cursor ? app->arrow_cursor : 0);

    app->renderer->origin = true;

    app->camera.go_towards_position(default_camera_position);

    // It's ok to be called multiple times
    STOP_ALLOCATION_LOG();
}

void GameLayer::on_detach() {
    try {
        options::save_options_to_file(app->options);
    } catch (const options::OptionsFileNotOpenError& e) {
        REL_ERROR("{}", e.what());
        options::handle_options_file_not_open_error();
    } catch (const options::OptionsFileError& e) {
        REL_ERROR("{}", e.what());

        try {
            options::create_options_file();
        } catch (const options::OptionsFileNotOpenError& e) {
            REL_ERROR("{}", e.what());
        } catch (const options::OptionsFileError& e) {
            REL_ERROR("{}", e.what());
        }
    }

    if (app->options.save_on_exit && !app->running) {
        board.finalize_pieces_state();

        save_load::GameState state;
        state.board = board;
        state.camera = app->camera;
        state.time = gui_layer->timer.get_time_raw();

        const time_t current = time(nullptr);
        state.date = ctime(&current);

        try {
            save_load::save_game_to_file(state);
        } catch (save_load::SaveFileNotOpenError& e) {
            REL_ERROR("{}", e.what());
            save_load::handle_save_file_not_open_error();
            REL_ERROR("Could not save game");
        } catch (save_load::SaveFileError& e) {
            REL_ERROR("{}", e.what());
            REL_ERROR("Could not save game");
        }
    }

    app->renderer->origin = false;

    gui_layer->timer = Timer();

    first_move = false;
}

void GameLayer::on_bind_layers() {
    imgui_layer = get_layer<ImGuiLayer>("imgui");
    gui_layer = get_layer<GuiLayer>("gui");
}

void GameLayer::on_update(float dt) {
    if (!imgui_layer->hovering_gui) {
        app->camera.update(mouse_wheel, dx, dy, dt);
    }
    board.move_pieces(dt);
    board.update_nodes(app->renderer->get_hovered_id());
    board.update_pieces(app->renderer->get_hovered_id());

    mouse_wheel = 0.0f;
    dx = 0.0f;
    dy = 0.0f;
}

void GameLayer::on_fixed_update() {
    if (!imgui_layer->hovering_gui) {
        app->camera.update_friction();
    }
}

void GameLayer::on_draw() {
//     setup_shadows();
//     setup_quad3d_projection_view();

//     app->storage->depth_map_framebuffer->bind();

//     renderer::clear(renderer::Depth);
//     renderer::set_viewport(2048, 2048);

//     render_to_depth();

//     app->storage->scene_framebuffer->bind();

//     renderer::clear(renderer::Color | renderer::Depth | renderer::Stencil);
//     renderer::set_viewport(app->data.width, app->data.height);
//     renderer::set_stencil_mask_zero();

//     renderer::bind_texture(app->storage->depth_map_framebuffer->get_depth_attachment(), 1);

//     renderer::load_projection_view(app->camera.get_projection_view_matrix());
    // setup_camera();
//     render_skybox();
//     renderer::draw_board(board);
//     renderer::disable_output_to_red(1);
//     renderer::draw_board_paint(board.paint);
// #ifdef NINE_MORRIS_3D_DEBUG
//     renderer::draw_origin();
// #endif
//     renderer::enable_output_to_red(1);
//     render_nodes();
//     render_pieces();

// #ifdef NINE_MORRIS_3D_DEBUG
//     renderer::draw_quad_3d(light.position, 1.0f, app->storage->light_texture);
// #endif

//     app->storage->scene_framebuffer->resolve_framebuffer(app->storage->intermediate_framebuffer->get_id(),
//             app->data.width, app->data.height);

//     app->storage->intermediate_framebuffer->bind();

//     const int x = static_cast<int>(input::get_mouse_x());
//     const int y = app->data.height - static_cast<int>(input::get_mouse_y());
//     reader.read(1, x, y);

//     Framebuffer::bind_default();

//     renderer::clear(renderer::Color);
//     renderer::draw_screen_quad(app->storage->intermediate_framebuffer->get_color_attachment(0));

//     int* data;
//     reader.get<int>(&data);
//     hovered_id = *data;
}

void GameLayer::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher (event);

    dispatcher.dispatch<MouseScrolledEvent>(MouseScrolled, BIND(GameLayer::on_mouse_scrolled));
    dispatcher.dispatch<MouseMovedEvent>(MouseMoved, BIND(GameLayer::on_mouse_moved));
    dispatcher.dispatch<MouseButtonPressedEvent>(MouseButtonPressed, BIND(GameLayer::on_mouse_button_pressed));
    dispatcher.dispatch<MouseButtonReleasedEvent>(MouseButtonReleased, BIND(GameLayer::on_mouse_button_released));
    dispatcher.dispatch<KeyReleasedEvent>(KeyReleased, BIND(GameLayer::on_key_released));
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
    if (event.button == MOUSE_BUTTON_LEFT) {
        if (board.next_move) {
            board.press(app->renderer->get_hovered_id());
        }
    }

    return false;
}

bool GameLayer::on_mouse_button_released(events::MouseButtonReleasedEvent& event) {
    if (event.button == MOUSE_BUTTON_LEFT) {
        if (board.next_move) {
            if (board.phase == Board::Phase::PlacePieces) {
                if (board.should_take_piece) {
                    bool taked = board.take_piece(app->renderer->get_hovered_id());

                    if (taked && !first_move) {
                        gui_layer->timer.start(app->window->get_time());
                        first_move = true;
                    }
                } else {
                    bool placed = board.place_piece(app->renderer->get_hovered_id());

                    if (placed && !first_move) {
                        gui_layer->timer.start(app->window->get_time());
                        first_move = true;
                    }
                }
            } else if (board.phase == Board::Phase::MovePieces) {
                if (board.should_take_piece) {
                    bool taked = board.take_piece(app->renderer->get_hovered_id());

                    if (taked && !first_move) {
                        gui_layer->timer.start(app->window->get_time());
                        first_move = true;
                    }
                } else {
                    board.select_piece(app->renderer->get_hovered_id());
                    bool put = board.put_piece(app->renderer->get_hovered_id());

                    if (put && !first_move) {
                        gui_layer->timer.start(app->window->get_time());
                        first_move = true;
                    }
                }
            }

            if (board.phase == Board::Phase::GameOver) {
                gui_layer->timer.stop();
            }

            board.release();
        }
    }

    return false;
}

bool GameLayer::on_key_released(events::KeyReleasedEvent& event) {
    if (event.key == KEY_SPACE) {
        app->camera.go_towards_position(default_camera_position);
    }

    return false;
}

std::shared_ptr<Buffer> GameLayer::create_ids_buffer(unsigned int vertices_size, hoverable::Id id) {
    std::vector<int> array;
    array.resize(vertices_size);
    for (unsigned int i = 0; i < array.size(); i++) {
        array[i] = static_cast<int>(id);
    }
    std::shared_ptr<Buffer> buffer = Buffer::create(array.data(), array.size() * sizeof(int));

    return buffer;
}

void GameLayer::setup_board() {
    if (!app->data.loaded_board) {
        const std::vector<std::string> uniforms = {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess"
        };
        const std::vector<UniformBlockSpecification> uniform_blocks = {
            app->renderer->get_projection_view_uniform_block(),
            app->renderer->get_light_uniform_block(),
            app->renderer->get_light_space_uniform_block()
        };
        app->data.board_shader = Shader::create(
            assets::path(assets::BOARD_VERTEX_SHADER),
            assets::path(assets::BOARD_FRAGMENT_SHADER),
            uniforms,
            uniform_blocks
        );

        std::shared_ptr<Buffer> vertices = Buffer::create(app->assets_data->board_mesh->vertices.data(),
                app->assets_data->board_mesh->vertices.size() * sizeof(mesh::Vertex));

        std::shared_ptr<VertexArray> vertex_array = VertexArray::create();

        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 3);
        layout.add(1, BufferLayout::Type::Float, 2);
        layout.add(2, BufferLayout::Type::Float, 3);

        std::shared_ptr<IndexBuffer> indices = IndexBuffer::create(app->assets_data->board_mesh->indices.data(),
                app->assets_data->board_mesh->indices.size() * sizeof(unsigned int));

        vertex_array->add_buffer(vertices, layout);
        vertex_array->add_index_buffer(indices);

        VertexArray::unbind();

        app->data.board_vertex_array = vertex_array;

        if (app->options.texture_quality == options::NORMAL) {
            app->data.board_diffuse_texture = Texture::create(
                app->assets_data->board_wood_diff_texture, true, -2.0f
            );
        } else if (app->options.texture_quality == options::LOW) {
            app->data.board_diffuse_texture = Texture::create(
                app->assets_data->board_wood_diff_texture_small, true, -2.0f
            );
        } else {
            assert(false);
        }

        app->data.wood_material = std::make_shared<Material>(app->data.board_shader);
        app->data.wood_material->add_texture("u_material.diffuse");
        app->data.wood_material->add_variable(Material::UniformType::Vec3, "u_material.specular");
        app->data.wood_material->add_variable(Material::UniformType::Float, "u_material.shininess");

        app->data.board_material_instance = MaterialInstance::make(app->data.wood_material);
        app->data.board_material_instance->set_texture("u_material.diffuse", app->data.board_diffuse_texture, 0);
        app->data.board_material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
        app->data.board_material_instance->set_float("u_material.shininess", 4.0f);
    }

    board = Board(board_state_history);

    board.model.vertex_array = app->data.board_vertex_array;
    board.model.index_count = app->assets_data->board_mesh->indices.size();
    board.model.scale = 20.0f;
    board.model.material = app->data.board_material_instance;

    app->renderer->add_model(board.model, Renderer::CastShadow | Renderer::HasShadow);

    app->data.loaded_board = true;

    DEB_DEBUG("Built board");
}

void GameLayer::setup_board_paint() {
    if (!app->data.loaded_board_paint) {
        const std::vector<std::string> uniforms = {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess"
        };
        const std::vector<UniformBlockSpecification> uniform_blocks = {
            app->renderer->get_projection_view_uniform_block(),
            app->renderer->get_light_uniform_block(),
            app->renderer->get_light_space_uniform_block()
        };
        app->data.board_paint_shader = Shader::create(
            assets::path(assets::BOARD_PAINT_VERTEX_SHADER),
            assets::path(assets::BOARD_PAINT_FRAGMENT_SHADER),
            uniforms,
            uniform_blocks
        );

        std::shared_ptr<Buffer> vertices = Buffer::create(app->assets_data->board_paint_mesh->vertices.data(),
                app->assets_data->board_paint_mesh->vertices.size() * sizeof(mesh::Vertex));

        std::shared_ptr<VertexArray> vertex_array = VertexArray::create();

        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 3);
        layout.add(1, BufferLayout::Type::Float, 2);
        layout.add(2, BufferLayout::Type::Float, 3);

        std::shared_ptr<IndexBuffer> indices = IndexBuffer::create(app->assets_data->board_paint_mesh->indices.data(),
                app->assets_data->board_paint_mesh->indices.size() * sizeof(unsigned int));

        vertex_array->add_buffer(vertices, layout);
        vertex_array->add_index_buffer(indices);

        VertexArray::unbind();

        app->data.board_paint_vertex_array = vertex_array;

        if (app->options.texture_quality == options::NORMAL) {
            app->data.board_paint_diffuse_texture = Texture::create(
                app->assets_data->board_paint_diff_texture, true, -1.0f
            );
        } else if (app->options.texture_quality == options::LOW) {
            app->data.board_paint_diffuse_texture = Texture::create(
                app->assets_data->board_paint_diff_texture_small, true, -1.0f
            );
        } else {
            assert(false);
        }

        app->data.paint_material = std::make_shared<Material>(app->data.board_paint_shader);
        app->data.paint_material->add_texture("u_material.diffuse");
        app->data.paint_material->add_variable(Material::UniformType::Vec3, "u_material.specular");
        app->data.paint_material->add_variable(Material::UniformType::Float, "u_material.shininess");

        app->data.board_paint_material_instance = MaterialInstance::make(app->data.paint_material);
        app->data.board_paint_material_instance->set_texture("u_material.diffuse", app->data.board_paint_diffuse_texture, 0);
        app->data.board_paint_material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
        app->data.board_paint_material_instance->set_float("u_material.shininess", 4.0f);
    }

    board.paint_model.vertex_array = app->data.board_paint_vertex_array;
    board.paint_model.index_count = app->assets_data->board_paint_mesh->indices.size();
    board.paint_model.position = glm::vec3(0.0f, 0.062f, 0.0f);
    board.paint_model.scale = 20.0f;
    board.paint_model.material = app->data.board_paint_material_instance;

    app->renderer->add_model(board.paint_model, Renderer::HasShadow);

    app->data.loaded_board_paint = true;

    DEB_DEBUG("Built board paint");
}

void GameLayer::setup_pieces() {
    if (!app->data.loaded_pieces) {
        const std::vector<std::string> uniforms = {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
            "u_material.tint"
        };
        const std::vector<UniformBlockSpecification> uniform_blocks = {
            app->renderer->get_projection_view_uniform_block(),
            app->renderer->get_light_uniform_block(),
            app->renderer->get_light_space_uniform_block()
        };
        app->data.piece_shader = Shader::create(
            assets::path(assets::PIECE_VERTEX_SHADER),
            assets::path(assets::PIECE_FRAGMENT_SHADER),
            uniforms,
            uniform_blocks
        );

        if (app->options.texture_quality == options::NORMAL) {
            app->data.white_piece_diffuse_texture = Texture::create(
                app->assets_data->white_piece_diff_texture, true, -1.5f
            );
            app->data.black_piece_diffuse_texture = Texture::create(
                app->assets_data->black_piece_diff_texture, true, -1.5f
            );
        } else if (app->options.texture_quality == options::LOW) {
            app->data.white_piece_diffuse_texture = Texture::create(
                app->assets_data->white_piece_diff_texture_small, true, -1.5f
            );
            app->data.black_piece_diffuse_texture = Texture::create(
                app->assets_data->black_piece_diff_texture_small, true, -1.5f
            );
        } else {
            assert(false);
        }

        app->data.tinted_wood_material = std::make_shared<Material>(app->data.piece_shader, Material::Hoverable);
        app->data.tinted_wood_material->add_texture("u_material.diffuse");
        app->data.tinted_wood_material->add_variable(Material::UniformType::Vec3, "u_material.specular");
        app->data.tinted_wood_material->add_variable(Material::UniformType::Float, "u_material.shininess");
        app->data.tinted_wood_material->add_variable(Material::UniformType::Vec3, "u_material.tint");
    }

    for (unsigned int i = 0; i < 9; i++) {
        setup_piece(i, Piece::Type::White, app->assets_data->white_piece_mesh,
                app->data.white_piece_diffuse_texture, glm::vec3(-4.0f, 0.3f, -2.0f + i * 0.5f));
    }
    for (unsigned int i = 9; i < 18; i++) {
        setup_piece(i, Piece::Type::Black, app->assets_data->black_piece_mesh,
                app->data.black_piece_diffuse_texture, glm::vec3(4.0f, 0.3f, -2.0f + (i - 9) * 0.5f));
    }

    app->data.loaded_pieces = true;
}

void GameLayer::setup_piece(unsigned int index, Piece::Type type, std::shared_ptr<mesh::Mesh<mesh::Vertex>> mesh,
        std::shared_ptr<Texture> texture, const glm::vec3& position) {
    if (!app->data.loaded_pieces) {
        hoverable::Id id = hoverable::generate_id();
        app->data.pieces_id[index] = id;

        std::shared_ptr<Buffer> vertices = Buffer::create(mesh->vertices.data(),
            mesh->vertices.size() * sizeof(mesh::Vertex));

        std::shared_ptr<Buffer> ids = create_ids_buffer(mesh->vertices.size(), id);

        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 3);
        layout.add(1, BufferLayout::Type::Float, 2);
        layout.add(2, BufferLayout::Type::Float, 3);

        BufferLayout layout2;
        layout2.add(3, BufferLayout::Type::Int, 1);

        std::shared_ptr<IndexBuffer> indices = IndexBuffer::create(mesh->indices.data(),
                mesh->indices.size() * sizeof(unsigned int));

        std::shared_ptr<VertexArray> vertex_array = VertexArray::create();
        vertex_array->add_buffer(vertices, layout);
        vertex_array->add_buffer(ids, layout2);
        vertex_array->add_index_buffer(indices);

        VertexArray::unbind();

        app->data.piece_vertex_arrays[index] = vertex_array;

        if (type == Piece::Type::White) {
            app->data.piece_material_instances[index] = MaterialInstance::make(app->data.tinted_wood_material);
            app->data.piece_material_instances[index]->set_texture("u_material.diffuse", app->data.white_piece_diffuse_texture, 0);
            app->data.piece_material_instances[index]->set_vec3("u_material.specular", glm::vec3(0.2f));
            app->data.piece_material_instances[index]->set_float("u_material.shininess", 4.0f);
            app->data.piece_material_instances[index]->set_vec3("u_material.tint", glm::vec3(1.0f));  // TODO use 1.0f, 0.2f, 0.2f
        } else {
            app->data.piece_material_instances[index] = MaterialInstance::make(app->data.tinted_wood_material);
            app->data.piece_material_instances[index]->set_texture("u_material.diffuse", app->data.black_piece_diffuse_texture, 0);
            app->data.piece_material_instances[index]->set_vec3("u_material.specular", glm::vec3(0.2f));
            app->data.piece_material_instances[index]->set_float("u_material.shininess", 4.0f);
            app->data.piece_material_instances[index]->set_vec3("u_material.tint", glm::vec3(1.0f));
        }
    }

    board.pieces[index] = Piece(app->data.pieces_id[index], type);

    int random_rotation = rand() % 360;

    board.pieces[index].model.vertex_array = app->data.piece_vertex_arrays[index];
    board.pieces[index].model.index_count = mesh->indices.size();
    board.pieces[index].model.position = position;
    board.pieces[index].model.rotation = glm::vec3(0.0f, glm::radians(static_cast<float>(random_rotation)), 0.0f);
    board.pieces[index].model.scale = 20.0f;
    board.pieces[index].model.material = app->data.piece_material_instances[index];

    app->renderer->add_model(board.pieces[index].model, Renderer::CastShadow | Renderer::HasShadow);

    DEB_DEBUG("Built piece {}", index);
}

void GameLayer::setup_nodes() {
    if (!app->data.loaded_nodes) {
        const std::vector<std::string> uniforms = {
            "u_model_matrix",
            "u_color"
        };
        app->data.node_shader = Shader::create(
            assets::path(assets::NODE_VERTEX_SHADER),
            assets::path(assets::NODE_FRAGMENT_SHADER),
            uniforms,
            { app->renderer->get_projection_view_uniform_block() }
        );

        app->data.basic_material = std::make_shared<Material>(app->data.node_shader, Material::Hoverable);
        app->data.basic_material->add_variable(Material::UniformType::Vec4, "u_color");
    }

    for (unsigned int i = 0; i < 24; i++) {
        setup_node(i, NODE_POSITIONS[i]);
    }

    app->data.loaded_nodes = true;
}

void GameLayer::setup_node(unsigned int index, const glm::vec3& position) {
    if (!app->data.loaded_nodes) {
        hoverable::Id id = hoverable::generate_id();
        app->data.nodes_id[index] = id;

        std::shared_ptr<Buffer> vertices = Buffer::create(app->assets_data->node_mesh->vertices.data(),
                app->assets_data->node_mesh->vertices.size() * sizeof(mesh::VertexP));

        std::shared_ptr<Buffer> ids = create_ids_buffer(app->assets_data->node_mesh->vertices.size(), id);

        std::shared_ptr<VertexArray> vertex_array = VertexArray::create();

        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 3);
        BufferLayout layout2;
        layout2.add(1, BufferLayout::Type::Int, 1);

        std::shared_ptr<IndexBuffer> indices = IndexBuffer::create(app->assets_data->node_mesh->indices.data(),
                app->assets_data->node_mesh->indices.size() * sizeof(unsigned int));

        vertex_array->add_buffer(vertices, layout);
        vertex_array->add_buffer(ids, layout2);
        vertex_array->add_index_buffer(indices);

        VertexArray::unbind();

        app->data.node_vertex_arrays[index] = vertex_array;

        app->data.node_material_instances[index] = MaterialInstance::make(app->data.basic_material);
        app->data.node_material_instances[index]->set_vec4("u_color", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    }

    board.nodes[index] = Node(app->data.nodes_id[index], index);

    board.nodes[index].model.vertex_array = app->data.node_vertex_arrays[index];
    board.nodes[index].model.index_count = app->assets_data->node_mesh->indices.size();
    board.nodes[index].model.position = position;
    board.nodes[index].model.scale = 20.0f;
    board.nodes[index].model.material = app->data.node_material_instances[index];

    app->renderer->add_model(board.nodes[index].model, Renderer::NoLighting);

    DEB_DEBUG("Built node {}", index);
}

void GameLayer::set_camera() {
    app->camera = Camera(
        app->options.sensitivity,
        47.0f,
        glm::vec3(0.0f),
        8.0f,
        glm::perspective(
            glm::radians(FOV), static_cast<float>(app->app_data.width) / app->app_data.height, NEAR, FAR
        )
    );

    default_camera_position = app->camera.get_position();

    app->camera = Camera(
        app->options.sensitivity,
        47.0f,
        glm::vec3(0.0f),
        8.7f,
        glm::perspective(
            glm::radians(FOV), static_cast<float>(app->app_data.width) / app->app_data.height, NEAR, FAR
        )
    );

    DEB_DEBUG("Set camera");
}

void GameLayer::set_skybox() {
    std::array<std::shared_ptr<TextureData>, 6> data;

    if (app->options.texture_quality == options::NORMAL) {
        data = {
            app->assets_data->skybox_px_texture,
            app->assets_data->skybox_nx_texture,
            app->assets_data->skybox_py_texture,
            app->assets_data->skybox_ny_texture,
            app->assets_data->skybox_pz_texture,
            app->assets_data->skybox_nz_texture
        };
    } else if (app->options.texture_quality == options::LOW) {
        data = {
            app->assets_data->skybox_px_texture_small,
            app->assets_data->skybox_nx_texture_small,
            app->assets_data->skybox_py_texture_small,
            app->assets_data->skybox_ny_texture_small,
            app->assets_data->skybox_pz_texture_small,
            app->assets_data->skybox_nz_texture_small
        };
    } else {
        assert(false);
    }

    app->renderer->set_skybox(Texture3D::create(data));  // FIXME only if not already made

    DEB_DEBUG("Built skybox");
}

void GameLayer::set_light() {
    if (app->options.skybox == options::FIELD) {
        app->renderer->light = LIGHT_FIELD;
    } else if (app->options.skybox == options::AUTUMN) {
        app->renderer->light = LIGHT_AUTUMN;
    } else {
        assert(false);
    }

    DEB_DEBUG("Set light");
}

void GameLayer::build_turn_indicator() {
    // if (!app->storage->white_indicator_texture) {
    //     app->storage->white_indicator_texture = Texture::create(app->assets_data->white_indicator_texture, false);
    //     app->storage->black_indicator_texture = Texture::create(app->assets_data->black_indicator_texture, false);
    // }

    // gui_layer->turn_indicator.position = glm::vec3(static_cast<float>(app->data.width - 90),
    //         static_cast<float>(app->data.height - 115), 0.0f);
    // gui_layer->turn_indicator.scale = 1.0f;

    DEB_DEBUG("Built turn indicator");
}

// void GameLayer::render_skybox() {
    // const glm::mat4& projection_matrix = app->camera.get_projection_matrix();
    // const glm::mat4 view_matrix = glm::mat4(glm::mat3(app->camera.get_view_matrix()));

    // renderer::draw_skybox(projection_matrix * view_matrix);
// }

// void GameLayer::setup_light() {
    // app->data.board_shader->bind();
    // app->data.board_shader->set_uniform_vec3("u_light.position", light.position);
    // app->data.board_shader->set_uniform_vec3("u_light.ambient", light.ambient_color);
    // app->data.board_shader->set_uniform_vec3("u_light.diffuse", light.diffuse_color);
    // app->data.board_shader->set_uniform_vec3("u_light.specular", light.specular_color);
    // app->data.board_shader->set_uniform_vec3("u_view_position", app->camera.get_position());

    // app->storage->board_paint_shader->bind();
    // app->storage->board_paint_shader->set_uniform_vec3("u_light.position", light.position);
    // app->storage->board_paint_shader->set_uniform_vec3("u_light.ambient", light.ambient_color);
    // app->storage->board_paint_shader->set_uniform_vec3("u_light.diffuse", light.diffuse_color);
    // app->storage->board_paint_shader->set_uniform_vec3("u_light.specular", light.specular_color);
    // app->storage->board_paint_shader->set_uniform_vec3("u_view_position", app->camera.get_position());

    // app->storage->piece_shader->bind();
    // app->storage->piece_shader->set_uniform_vec3("u_light.position", light.position);
    // app->storage->piece_shader->set_uniform_vec3("u_light.ambient", light.ambient_color);
    // app->storage->piece_shader->set_uniform_vec3("u_light.diffuse", light.diffuse_color);
    // app->storage->piece_shader->set_uniform_vec3("u_light.specular", light.specular_color);
    // app->storage->piece_shader->set_uniform_vec3("u_view_position", app->camera.get_position());
// }

// void GameLayer::setup_camera() {
    // app->data.board_shader->bind();
    // app->data.board_shader->set_uniform_vec3("u_view_position", app->camera.get_position());

    // app->storage->board_paint_shader->bind();
    // app->storage->board_paint_shader->set_uniform_vec3("u_view_position", app->camera.get_position());

    // app->storage->piece_shader->bind();
    // app->storage->piece_shader->set_uniform_vec3("u_view_position", app->camera.get_position());
// }

// void GameLayer::setup_board() {
    // app->data.board_shader->bind();
    // app->data.board_shader->set_uniform_int("u_material.diffuse", 0);
    // app->data.board_shader->set_uniform_vec3("u_material.specular", board.model.specular_color);
    // app->data.board_shader->set_uniform_float("u_material.shininess", board.model.shininess);
// }

// void GameLayer::setup_board_paint() {
    // app->storage->board_paint_shader->bind();
    // app->storage->board_paint_shader->set_uniform_int("u_material.diffuse", 0);
    // app->storage->board_paint_shader->set_uniform_vec3("u_material.specular", board.paint.specular_color);
    // app->storage->board_paint_shader->set_uniform_float("u_material.shininess", board.paint.shininess);
// }

// void GameLayer::setup_pieces() {
    // app->storage->piece_shader->bind();
    // app->storage->piece_shader->set_uniform_int("u_material.diffuse", 0);
    // app->storage->piece_shader->set_uniform_vec3("u_material.specular", board.pieces[0].specular_color);  // TODO think about a better way
    // app->storage->piece_shader->set_uniform_float("u_material.shininess", board.pieces[0].shininess);
// }

// void GameLayer::render_pieces() {
//     constexpr auto copy = [](const Piece* piece) {
//         return piece->active;
//     };
//     const auto sort = [this](const Piece* lhs, const Piece* rhs) {
//         const float distance1 = glm::length(app->camera.get_position() - lhs->model.position);
//         const float distance2 = glm::length(app->camera.get_position() - rhs->model.position);
//         return distance1 > distance2;
//     };

//     std::array<Piece*, 18> pointer_pieces;
//     for (unsigned int i = 0; i < 18; i++) {
//         pointer_pieces[i] = &board.pieces[i];
//     }
//     std::vector<Piece*> active_pieces;
//     std::copy_if(pointer_pieces.begin(), pointer_pieces.end(), std::back_inserter(active_pieces), copy);
//     std::sort(active_pieces.begin(), active_pieces.end(), sort);

//     for (const Piece* piece : active_pieces) {
        // if (piece->selected) {
            // renderer::draw_piece_with_outline(piece, piece->select_color);
        // } else if (piece->show_outline && piece->id == hovered_id && piece->in_use && !piece->pending_remove) {
            // renderer::draw_piece_with_outline(piece, piece->hover_color);
        // } else if (piece->to_take && piece->id == hovered_id && piece->in_use) {
            // renderer::draw_piece(piece, glm::vec3(1.0f, 0.2f, 0.2f));
        // } else {
            // renderer::draw_piece(piece, glm::vec3(1.0f, 1.0f, 1.0f));
        // }
//     }
// }

// void GameLayer::render_nodes() {
//     for (Node& node : board.nodes) {
        // if (node.id == hovered_id && board.phase != Board::Phase::None &&
                // board.phase != Board::Phase::GameOver) {
            // renderer::draw_node(node, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
        // } else {
            // renderer::draw_node(node, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        // }
//     }
// }

// void GameLayer::render_to_depth() {
    // renderer::draw_to_depth(glm::vec3(0.0f), glm::vec3(0.0f), board.scale, board.vertex_array,
    //         board.index_count);

    // constexpr auto copy = [](const Piece* piece) {
    //     return piece->active;
    // };

    // std::array<Piece*, 18> pointer_pieces;
    // for (unsigned int i = 0; i < 18; i++) {
    //     pointer_pieces[i] = &board.pieces[i];
    // }
    // std::vector<Piece*> active_pieces;
    // std::copy_if(pointer_pieces.begin(), pointer_pieces.end(), std::back_inserter(active_pieces), copy);

    // for (Piece* piece : active_pieces) {
        // renderer::draw_to_depth(piece->position, piece->rotation, piece->scale, piece->vertex_array,
        //         piece->index_count);
//     }
// }

// void GameLayer::setup_shadows() {
    // const glm::mat4 projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 9.0f);
    // const glm::mat4 view = glm::lookAt(light.position / 4.0f,
            // glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // const glm::mat4 light_space_matrix = projection * view;

    // app->storage->shadow_shader->bind();
    // app->storage->shadow_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);
    // app->storage->board_shader->bind();
    // app->storage->board_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);
    // app->storage->board_shader->set_uniform_int("u_shadow_map", 1);
    // app->storage->board_paint_shader->bind();
    // app->storage->board_paint_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);
    // app->storage->board_paint_shader->set_uniform_int("u_shadow_map", 1);
    // app->storage->piece_shader->bind();
    // app->storage->piece_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);
    // app->storage->piece_shader->set_uniform_int("u_shadow_map", 1);
// }

// void GameLayer::setup_quad3d_projection_view() {
    // app->storage->quad3d_shader->bind();
    // app->storage->quad3d_shader->set_uniform_matrix("u_projection_matrix", app->camera.get_projection_matrix());
    // app->storage->quad3d_shader->set_uniform_matrix("u_view_matrix", app->camera.get_view_matrix());
// }

void GameLayer::set_scene_framebuffer(int samples) {
    // if (app->storage->scene_framebuffer->get_specification().samples == samples) {
    //     return;
    // }

    // FramebufferSpecification specification;
    // specification.width = app->data.width;
    // specification.height = app->data.height;
    // specification.samples = app->options.samples;
    // specification.color_attachments = {
    //     Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture),
    //     Attachment(AttachmentFormat::RED_I, AttachmentType::Texture)
    // };
    // specification.depth_attachment = Attachment(AttachmentFormat::DEPTH24_STENCIL8,
    //         AttachmentType::Renderbuffer);

    // app->storage->scene_framebuffer = Framebuffer::create(specification);

    // app->purge_framebuffers();
    // app->add_framebuffer(app->storage->scene_framebuffer);
}

void GameLayer::set_textures_quality(const std::string& quality) {
    using namespace assets;

    // quality is the new option; options.texture_quality is not set yet

    if (quality == app->options.texture_quality) {
        return;
    }

    if (quality == options::NORMAL) {
        app->assets_data->board_wood_diff_texture_small = nullptr;
        app->assets_data->board_paint_diff_texture_small = nullptr;
        app->assets_data->white_piece_diff_texture_small = nullptr;
        app->assets_data->black_piece_diff_texture_small = nullptr;
        app->assets_data->skybox_px_texture_small = nullptr;
        app->assets_data->skybox_nx_texture_small = nullptr;
        app->assets_data->skybox_py_texture_small = nullptr;
        app->assets_data->skybox_ny_texture_small = nullptr;
        app->assets_data->skybox_pz_texture_small = nullptr;
        app->assets_data->skybox_nz_texture_small = nullptr;
        app->assets_data->board_wood_diff_texture = std::make_shared<TextureData>(path(BOARD_WOOD_TEXTURE), true);
        app->assets_data->board_paint_diff_texture = std::make_shared<TextureData>(path(BOARD_PAINT_TEXTURE), true);
        app->assets_data->white_piece_diff_texture = std::make_shared<TextureData>(path(WHITE_PIECE_TEXTURE), true);
        app->assets_data->black_piece_diff_texture = std::make_shared<TextureData>(path(BLACK_PIECE_TEXTURE), true);
        if (app->options.skybox == options::FIELD) {
            app->assets_data->skybox_px_texture = std::make_shared<TextureData>(path(FIELD_PX_TEXTURE), false);
            app->assets_data->skybox_nx_texture = std::make_shared<TextureData>(path(FIELD_NX_TEXTURE), false);
            app->assets_data->skybox_py_texture = std::make_shared<TextureData>(path(FIELD_PY_TEXTURE), false);
            app->assets_data->skybox_ny_texture = std::make_shared<TextureData>(path(FIELD_NY_TEXTURE), false);
            app->assets_data->skybox_pz_texture = std::make_shared<TextureData>(path(FIELD_PZ_TEXTURE), false);
            app->assets_data->skybox_nz_texture = std::make_shared<TextureData>(path(FIELD_NZ_TEXTURE), false);
        } else if (app->options.skybox == options::AUTUMN) {
            app->assets_data->skybox_px_texture = std::make_shared<TextureData>(path(AUTUMN_PX_TEXTURE), false);
            app->assets_data->skybox_nx_texture = std::make_shared<TextureData>(path(AUTUMN_NX_TEXTURE), false);
            app->assets_data->skybox_py_texture = std::make_shared<TextureData>(path(AUTUMN_PY_TEXTURE), false);
            app->assets_data->skybox_ny_texture = std::make_shared<TextureData>(path(AUTUMN_NY_TEXTURE), false);
            app->assets_data->skybox_pz_texture = std::make_shared<TextureData>(path(AUTUMN_PZ_TEXTURE), false);
            app->assets_data->skybox_nz_texture = std::make_shared<TextureData>(path(AUTUMN_NZ_TEXTURE), false);
        } else {
            assert(false);
        }

        // app->storage->board_wood_diff_texture = Texture::create(app->assets_data->board_wood_diff_texture, true, -2.0f);
        // board.diffuse_texture = app->storage->board_wood_diff_texture;

        // app->storage->board_paint_diff_texture = Texture::create(app->assets_data->board_paint_diff_texture, true, -2.0f);
        // board.paint.diffuse_texture = app->storage->board_paint_diff_texture;

        // app->storage->white_piece_diff_texture = Texture::create(app->assets_data->white_piece_diff_texture, true, -1.5f);
        // app->storage->black_piece_diff_texture = Texture::create(app->assets_data->black_piece_diff_texture, true, -1.5f);
        for (Piece& piece : board.pieces) {
            if (piece.type == Piece::Type::White) {
                // piece.diffuse_texture = app->storage->white_piece_diff_texture;
            } else {
                // piece.diffuse_texture = app->storage->black_piece_diff_texture;
            }
        }

        const std::array<std::shared_ptr<TextureData>, 6> data = {
            app->assets_data->skybox_px_texture,
            app->assets_data->skybox_nx_texture,
            app->assets_data->skybox_py_texture,
            app->assets_data->skybox_ny_texture,
            app->assets_data->skybox_pz_texture,
            app->assets_data->skybox_nz_texture
        };
        // app->storage->skybox_texture = Texture3D::create(data);
    } else if (quality == options::LOW) {
        app->assets_data->board_wood_diff_texture = nullptr;
        app->assets_data->board_paint_diff_texture = nullptr;
        app->assets_data->white_piece_diff_texture = nullptr;
        app->assets_data->black_piece_diff_texture = nullptr;
        app->assets_data->skybox_px_texture = nullptr;
        app->assets_data->skybox_nx_texture = nullptr;
        app->assets_data->skybox_py_texture = nullptr;
        app->assets_data->skybox_ny_texture = nullptr;
        app->assets_data->skybox_pz_texture = nullptr;
        app->assets_data->skybox_nz_texture = nullptr;
        app->assets_data->board_wood_diff_texture_small = std::make_shared<TextureData>(path(BOARD_WOOD_TEXTURE_SMALL), true);
        app->assets_data->board_paint_diff_texture_small = std::make_shared<TextureData>(path(BOARD_PAINT_TEXTURE_SMALL), true);
        app->assets_data->white_piece_diff_texture_small = std::make_shared<TextureData>(path(WHITE_PIECE_TEXTURE_SMALL), true);
        app->assets_data->black_piece_diff_texture_small = std::make_shared<TextureData>(path(BLACK_PIECE_TEXTURE_SMALL), true);
        if (app->options.skybox == options::FIELD) {
            app->assets_data->skybox_px_texture_small = std::make_shared<TextureData>(path(FIELD_PX_TEXTURE_SMALL), false);
            app->assets_data->skybox_nx_texture_small = std::make_shared<TextureData>(path(FIELD_NX_TEXTURE_SMALL), false);
            app->assets_data->skybox_py_texture_small = std::make_shared<TextureData>(path(FIELD_PY_TEXTURE_SMALL), false);
            app->assets_data->skybox_ny_texture_small = std::make_shared<TextureData>(path(FIELD_NY_TEXTURE_SMALL), false);
            app->assets_data->skybox_pz_texture_small = std::make_shared<TextureData>(path(FIELD_PZ_TEXTURE_SMALL), false);
            app->assets_data->skybox_nz_texture_small = std::make_shared<TextureData>(path(FIELD_NZ_TEXTURE_SMALL), false);
        } else if (app->options.skybox == options::AUTUMN) {
            app->assets_data->skybox_px_texture_small = std::make_shared<TextureData>(path(AUTUMN_PX_TEXTURE_SMALL), false);
            app->assets_data->skybox_nx_texture_small = std::make_shared<TextureData>(path(AUTUMN_NX_TEXTURE_SMALL), false);
            app->assets_data->skybox_py_texture_small = std::make_shared<TextureData>(path(AUTUMN_PY_TEXTURE_SMALL), false);
            app->assets_data->skybox_ny_texture_small = std::make_shared<TextureData>(path(AUTUMN_NY_TEXTURE_SMALL), false);
            app->assets_data->skybox_pz_texture_small = std::make_shared<TextureData>(path(AUTUMN_PZ_TEXTURE_SMALL), false);
            app->assets_data->skybox_nz_texture_small = std::make_shared<TextureData>(path(AUTUMN_NZ_TEXTURE_SMALL), false);
        } else {
            assert(false);
        }

        // app->storage->board_wood_diff_texture = Texture::create(app->assets_data->board_wood_diff_texture_small, true, -2.0f);
        // board.diffuse_texture = app->storage->board_wood_diff_texture;

        // app->storage->board_paint_diff_texture = Texture::create(app->assets_data->board_paint_diff_texture_small, true, -2.0f);
        // board.paint.diffuse_texture = app->storage->board_paint_diff_texture;

        // app->storage->white_piece_diff_texture = Texture::create(app->assets_data->white_piece_diff_texture_small, true, -1.5f);
        // app->storage->black_piece_diff_texture = Texture::create(app->assets_data->black_piece_diff_texture_small, true, -1.5f);
        for (Piece& piece : board.pieces) {
            if (piece.type == Piece::Type::White) {
                // piece.diffuse_texture = app->storage->white_piece_diff_texture;
            } else {
                // piece.diffuse_texture = app->storage->black_piece_diff_texture;
            }
        }

        const std::array<std::shared_ptr<TextureData>, 6> data = {
            app->assets_data->skybox_px_texture_small,
            app->assets_data->skybox_nx_texture_small,
            app->assets_data->skybox_py_texture_small,
            app->assets_data->skybox_ny_texture_small,
            app->assets_data->skybox_pz_texture_small,
            app->assets_data->skybox_nz_texture_small
        };
        // app->storage->skybox_texture = Texture3D::create(data);
    } else {
        assert(false);
    }
}

void GameLayer::set_skybox(const std::string& skybox) {
    using namespace assets;

    // skybox is the new option; options.skybox is not set yet

    if (skybox == app->options.skybox) {
        return;
    }

    if (skybox == options::FIELD) {
        if (app->options.texture_quality == options::NORMAL) {
            app->assets_data->skybox_px_texture = std::make_shared<TextureData>(path(FIELD_PX_TEXTURE), false);
            app->assets_data->skybox_nx_texture = std::make_shared<TextureData>(path(FIELD_NX_TEXTURE), false);
            app->assets_data->skybox_py_texture = std::make_shared<TextureData>(path(FIELD_PY_TEXTURE), false);
            app->assets_data->skybox_ny_texture = std::make_shared<TextureData>(path(FIELD_NY_TEXTURE), false);
            app->assets_data->skybox_pz_texture = std::make_shared<TextureData>(path(FIELD_PZ_TEXTURE), false);
            app->assets_data->skybox_nz_texture = std::make_shared<TextureData>(path(FIELD_NZ_TEXTURE), false);

            const std::array<std::shared_ptr<TextureData>, 6> data = {
                app->assets_data->skybox_px_texture,
                app->assets_data->skybox_nx_texture,
                app->assets_data->skybox_py_texture,
                app->assets_data->skybox_ny_texture,
                app->assets_data->skybox_pz_texture,
                app->assets_data->skybox_nz_texture
            };
            app->renderer->set_skybox(Texture3D::create(data));
        } else if (app->options.texture_quality == options::LOW) {
            app->assets_data->skybox_px_texture_small = std::make_shared<TextureData>(path(FIELD_PX_TEXTURE_SMALL), false);
            app->assets_data->skybox_nx_texture_small = std::make_shared<TextureData>(path(FIELD_NX_TEXTURE_SMALL), false);
            app->assets_data->skybox_py_texture_small = std::make_shared<TextureData>(path(FIELD_PY_TEXTURE_SMALL), false);
            app->assets_data->skybox_ny_texture_small = std::make_shared<TextureData>(path(FIELD_NY_TEXTURE_SMALL), false);
            app->assets_data->skybox_pz_texture_small = std::make_shared<TextureData>(path(FIELD_PZ_TEXTURE_SMALL), false);
            app->assets_data->skybox_nz_texture_small = std::make_shared<TextureData>(path(FIELD_NZ_TEXTURE_SMALL), false);

            const std::array<std::shared_ptr<TextureData>, 6> data = {
                app->assets_data->skybox_px_texture_small,
                app->assets_data->skybox_nx_texture_small,
                app->assets_data->skybox_py_texture_small,
                app->assets_data->skybox_ny_texture_small,
                app->assets_data->skybox_pz_texture_small,
                app->assets_data->skybox_nz_texture_small
            };
            app->renderer->set_skybox(Texture3D::create(data));
        } else {
            assert(false);
        }

        app->renderer->light = LIGHT_FIELD;
    } else if (skybox == options::AUTUMN) {
        if (app->options.texture_quality == options::NORMAL) {
            app->assets_data->skybox_px_texture = std::make_shared<TextureData>(path(AUTUMN_PX_TEXTURE), false);
            app->assets_data->skybox_nx_texture = std::make_shared<TextureData>(path(AUTUMN_NX_TEXTURE), false);
            app->assets_data->skybox_py_texture = std::make_shared<TextureData>(path(AUTUMN_PY_TEXTURE), false);
            app->assets_data->skybox_ny_texture = std::make_shared<TextureData>(path(AUTUMN_NY_TEXTURE), false);
            app->assets_data->skybox_pz_texture = std::make_shared<TextureData>(path(AUTUMN_PZ_TEXTURE), false);
            app->assets_data->skybox_nz_texture = std::make_shared<TextureData>(path(AUTUMN_NZ_TEXTURE), false);

            const std::array<std::shared_ptr<TextureData>, 6> data = {
                app->assets_data->skybox_px_texture,
                app->assets_data->skybox_nx_texture,
                app->assets_data->skybox_py_texture,
                app->assets_data->skybox_ny_texture,
                app->assets_data->skybox_pz_texture,
                app->assets_data->skybox_nz_texture
            };
            app->renderer->set_skybox(Texture3D::create(data));
        } else if (app->options.texture_quality == options::LOW) {
            app->assets_data->skybox_px_texture_small = std::make_shared<TextureData>(path(AUTUMN_PX_TEXTURE_SMALL), false);
            app->assets_data->skybox_nx_texture_small = std::make_shared<TextureData>(path(AUTUMN_NX_TEXTURE_SMALL), false);
            app->assets_data->skybox_py_texture_small = std::make_shared<TextureData>(path(AUTUMN_PY_TEXTURE_SMALL), false);
            app->assets_data->skybox_ny_texture_small = std::make_shared<TextureData>(path(AUTUMN_NY_TEXTURE_SMALL), false);
            app->assets_data->skybox_pz_texture_small = std::make_shared<TextureData>(path(AUTUMN_PZ_TEXTURE_SMALL), false);
            app->assets_data->skybox_nz_texture_small = std::make_shared<TextureData>(path(AUTUMN_NZ_TEXTURE_SMALL), false);

            const std::array<std::shared_ptr<TextureData>, 6> data = {
                app->assets_data->skybox_px_texture_small,
                app->assets_data->skybox_nx_texture_small,
                app->assets_data->skybox_py_texture_small,
                app->assets_data->skybox_ny_texture_small,
                app->assets_data->skybox_pz_texture_small,
                app->assets_data->skybox_nz_texture_small
            };
            app->renderer->set_skybox(Texture3D::create(data));
        } else {
            assert(false);
        }

        app->renderer->light = LIGHT_AUTUMN;
    } else {
        assert(false);
    }
}

void GameLayer::load_game() {
    save_load::GameState state;
    try {
        save_load::load_game_from_file(state);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_ERROR("{}", e.what());
        save_load::handle_save_file_not_open_error();
        REL_ERROR("Could not load game");
        return;
    } catch (const save_load::SaveFileError& e) {
        REL_ERROR("{}", e.what());  // TODO maybe delete file
        REL_ERROR("Could not load game");
        return;
    }

    app->camera = state.camera;

    // Board& board = board;
    // board.id = state.board.id;
    // board.scale = state.board.scale;
    // board.index_count = state.board.index_count;
    // board.specular_color = state.board.specular_color;
    // board.shininess = state.board.shininess;

    for (unsigned int i = 0; i < 24; i++) {
        Node& node = board.nodes[i];

        node.id = state.board.nodes[i].id;
        node.model.position = state.board.nodes[i].model.position;
        node.model.scale = state.board.nodes[i].model.scale;
        // node.model.index_count = state.board.nodes[i].model.index_count;
        node.piece_id = state.board.nodes[i].piece_id;
        node.piece = nullptr;  // It must be NULL, if the ids don't match
        for (unsigned int i = 0; i < 18; i++) {  // Assign correct addresses
            if (state.board.pieces[i].id == node.piece_id) {
                node.piece = &board.pieces[i];
            }
        }
        node.index = state.board.nodes[i].index;
    }

    for (unsigned int i = 0; i < 18; i++) {
        Piece& piece = board.pieces[i];

        piece.id = state.board.pieces[i].id;
        piece.model.position = state.board.pieces[i].model.position;
        piece.model.rotation = state.board.pieces[i].model.rotation;
        piece.model.scale = state.board.pieces[i].model.scale;
        piece.movement.type = state.board.pieces[i].movement.type;
        piece.movement.velocity = state.board.pieces[i].movement.velocity;
        piece.movement.target = state.board.pieces[i].movement.target;
        piece.movement.target0 = state.board.pieces[i].movement.target0;
        piece.movement.target1 = state.board.pieces[i].movement.target1;
        piece.movement.reached_target0 = state.board.pieces[i].movement.reached_target0;
        piece.movement.reached_target1 = state.board.pieces[i].movement.reached_target1;
        piece.should_move = state.board.pieces[i].should_move;
        // piece.index_count = state.board.pieces[i].index_count;
        // piece.specular_color = state.board.pieces[i].specular_color;
        // piece.shininess = state.board.pieces[i].shininess;
        // piece.select_color = state.board.pieces[i].select_color;
        // piece.hover_color = state.board.pieces[i].hover_color;
        piece.type = state.board.pieces[i].type;
        piece.in_use = state.board.pieces[i].in_use;
        piece.node_id = state.board.pieces[i].node_id;
        piece.node = nullptr;  // It must be NULL, if the ids don't match
        // Assign correct addresses; use only board as nodes have already been assigned
        for (Node& node : board.nodes) {
            if (node.id == piece.node_id) {
                piece.node = &node;
            }
        }
        piece.show_outline = state.board.pieces[i].show_outline;
        piece.to_take = state.board.pieces[i].to_take;
        piece.pending_remove = false;
        piece.selected = false;
        piece.active = state.board.pieces[i].active;
    }

    board.phase = state.board.phase;
    board.turn = state.board.turn;
    board.ending = state.board.ending;
    board.white_pieces_count = state.board.white_pieces_count;
    board.black_pieces_count = state.board.black_pieces_count;
    board.not_placed_white_pieces_count = state.board.not_placed_white_pieces_count;
    board.not_placed_black_pieces_count = state.board.not_placed_black_pieces_count;
    board.should_take_piece = state.board.should_take_piece;
    board.selected_piece = nullptr;
    board.can_jump = state.board.can_jump;
    board.turns_without_mills = state.board.turns_without_mills;
    board.repetition_history = state.board.repetition_history;
    
    // board.paint.position = state.board.paint.position;
    // board.paint.scale = state.board.paint.scale;
    // board.paint.index_count = state.board.paint.index_count;
    // board.paint.specular_color = state.board.paint.specular_color;
    // board.paint.shininess = state.board.paint.shininess;

    board.state_history = state.board.state_history;
    board.next_move = state.board.next_move;

    gui_layer->timer.stop();
    gui_layer->timer.set_time(state.time);
    first_move = false;

    board.update_cursor();

    DEB_INFO("Loaded game");
}
