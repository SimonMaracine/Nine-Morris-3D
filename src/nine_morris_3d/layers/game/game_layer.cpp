#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "application/layer.h"
#include "application/window.h"
#include "application/events.h"
#include "application/input.h"
#include "application/platform.h"
#include "graphics/renderer/buffer_layout.h"
#include "graphics/renderer/light.h"
#include "graphics/renderer/camera.h"
#include "graphics/renderer/opengl/vertex_array.h"
#include "graphics/renderer/opengl/buffer.h"
#include "graphics/renderer/opengl/texture.h"
#include "graphics/renderer/opengl/vertex_array.h"
#include "graphics/renderer/opengl/framebuffer.h"
#include "graphics/renderer/opengl/buffer.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/piece.h"
#include "nine_morris_3d/board.h"
#include "nine_morris_3d/options.h"
#include "nine_morris_3d/save_load.h"
#include "nine_morris_3d/assets.h"
#include "nine_morris_3d/paths.h"
#include "other/mesh.h"
#include "other/loader.h"
#include "other/texture_data.h"
#include "other/logging.h"
#include "other/assert.h"
#include "other/encryption.h"

using namespace encryption;

constexpr DirectionalLight LIGHT_FIELD = {
    glm::vec3(5.7f, 8.4f, 12.4f),
    glm::vec3(0.4f),
    glm::vec3(0.88f),
    glm::vec3(0.9f)
};

constexpr DirectionalLight LIGHT_AUTUMN = {
    glm::vec3(-4.4f, 11.0f, 6.4f),
    glm::vec3(0.32f),
    glm::vec3(0.82f),
    glm::vec3(0.82f)
};

void GameLayer::on_attach() {
    board_state_history = std::make_shared<std::vector<Board>>();
    setup_board();
    setup_board_paint();

    setup_pieces();
    setup_nodes();

    setup_camera();

    app->window->set_vsync(app->options.vsync);
    app->window->set_cursor(app->options.custom_cursor ? app->arrow_cursor : 0);

#ifdef NINE_MORRIS_3D_DEBUG
    app->renderer->origin = true;
#endif

    app->camera.go_towards_position(default_camera_position);
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

    if (app->options.save_on_exit && !app->running && first_move) {
        board.finalize_pieces_state();

        save_load::GameState state;
        state.board = board;
        state.camera = app->camera;
        state.time = gui_layer->timer.get_time_raw();

        const time_t current = time(nullptr);
        state.date = ctime(&current);

        try {
            save_load::save_game_to_file(state);
        } catch (const save_load::SaveFileNotOpenError& e) {
            REL_ERROR("{}", e.what());
            save_load::handle_save_file_not_open_error();
            REL_ERROR("Could not save game");
        } catch (const save_load::SaveFileError& e) {
            REL_ERROR("{}", e.what());
            REL_ERROR("Could not save game");
        }
    }

#ifdef NINE_MORRIS_3D_DEBUG
    app->renderer->origin = false;
#endif

    app->renderer->clear_models();

    gui_layer->timer = Timer();

    first_move = false;

    if (loader != nullptr) {
        loader->get_thread().join();
    }
}

void GameLayer::on_awake() {
    imgui_layer = get_layer<ImGuiLayer>("imgui");
    gui_layer = get_layer<GuiLayer>("gui");

    prepare_board();
    prepare_board_paint();
    prepare_pieces();
    prepare_nodes();

    setup_skybox();
    setup_light();
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

    // Short-circuit optimization
    if (changed_skybox && loader->done_loading()) {
        actually_change_skybox();

        changed_skybox = false;

        loader->get_thread().join();
        loader = nullptr;
    }

    // Short-circuit optimization
    if (changed_texture_quality && loader->done_loading()) {
        actually_change_texture_quality();

        changed_texture_quality = false;

        loader->get_thread().join();
        loader = nullptr;
    }
}

void GameLayer::on_fixed_update() {
    if (!imgui_layer->hovering_gui) {
        app->camera.update_friction();
    }
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
                    const bool took = board.take_piece(app->renderer->get_hovered_id());

                    if (took && !first_move) {
                        gui_layer->timer.start(app->window->get_time());
                        first_move = true;
                    }
                } else {
                    const bool placed = board.place_piece(app->renderer->get_hovered_id());

                    if (placed && !first_move) {
                        gui_layer->timer.start(app->window->get_time());
                        first_move = true;
                    }
                }
            } else if (board.phase == Board::Phase::MovePieces) {
                if (board.should_take_piece) {
                    const bool took = board.take_piece(app->renderer->get_hovered_id());

                    if (took && !first_move) {
                        gui_layer->timer.start(app->window->get_time());
                        first_move = true;
                    }
                } else {
                    board.select_piece(app->renderer->get_hovered_id());
                    const bool put = board.put_piece(app->renderer->get_hovered_id());

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

void GameLayer::prepare_board() {
    std::vector<std::string> uniforms = {
        "u_model_matrix",
        "u_shadow_map",
        "u_material.diffuse",
        "u_material.specular",
        "u_material.shininess"
    };
    std::vector<UniformBlockSpecification> uniform_blocks = {
        app->renderer->get_projection_view_uniform_block(),
        app->renderer->get_light_uniform_block(),
        app->renderer->get_light_space_uniform_block()
    };
    app->data.board_shader = Shader::create(
        convert(paths::path_for_assets(assets::BOARD_VERTEX_SHADER)),
        convert(paths::path_for_assets(assets::BOARD_FRAGMENT_SHADER)),
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

    app->data.board_diffuse_texture = Texture::create(
        app->assets_data->board_wood_diff_texture, true, -2.0f, app->options.anisotropic_filtering
    );

    app->data.wood_material = std::make_shared<Material>(app->data.board_shader);
    app->data.wood_material->add_texture("u_material.diffuse");
    app->data.wood_material->add_variable(Material::UniformType::Vec3, "u_material.specular");
    app->data.wood_material->add_variable(Material::UniformType::Float, "u_material.shininess");

    app->data.board_material_instance = MaterialInstance::make(app->data.wood_material);
    app->data.board_material_instance->set_texture("u_material.diffuse", app->data.board_diffuse_texture, 0);
    app->data.board_material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    app->data.board_material_instance->set_float("u_material.shininess", 4.0f);
}

void GameLayer::prepare_board_paint() {
    std::vector<std::string> uniforms = {
        "u_model_matrix",
        "u_shadow_map",
        "u_material.diffuse",
        "u_material.specular",
        "u_material.shininess"
    };
    std::vector<UniformBlockSpecification> uniform_blocks = {
        app->renderer->get_projection_view_uniform_block(),
        app->renderer->get_light_uniform_block(),
        app->renderer->get_light_space_uniform_block()
    };
    app->data.board_paint_shader = Shader::create(
        convert(paths::path_for_assets(assets::BOARD_PAINT_VERTEX_SHADER)),
        convert(paths::path_for_assets(assets::BOARD_PAINT_FRAGMENT_SHADER)),
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

    app->data.board_paint_diffuse_texture = Texture::create(
        app->assets_data->board_paint_diff_texture, true, -1.0f, app->options.anisotropic_filtering
    );

    app->data.paint_material = std::make_shared<Material>(app->data.board_paint_shader);
    app->data.paint_material->add_texture("u_material.diffuse");
    app->data.paint_material->add_variable(Material::UniformType::Vec3, "u_material.specular");
    app->data.paint_material->add_variable(Material::UniformType::Float, "u_material.shininess");

    app->data.board_paint_material_instance = MaterialInstance::make(app->data.paint_material);
    app->data.board_paint_material_instance->set_texture("u_material.diffuse", app->data.board_paint_diffuse_texture, 0);
    app->data.board_paint_material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    app->data.board_paint_material_instance->set_float("u_material.shininess", 4.0f);
}

void GameLayer::prepare_pieces() {
    std::vector<std::string> uniforms = {
        "u_model_matrix",
        "u_shadow_map",
        "u_material.diffuse",
        "u_material.specular",
        "u_material.shininess",
        "u_material.tint"
    };
    std::vector<UniformBlockSpecification> uniform_blocks = {
        app->renderer->get_projection_view_uniform_block(),
        app->renderer->get_light_uniform_block(),
        app->renderer->get_light_space_uniform_block()
    };
    app->data.piece_shader = Shader::create(
        convert(paths::path_for_assets(assets::PIECE_VERTEX_SHADER)),
        convert(paths::path_for_assets(assets::PIECE_FRAGMENT_SHADER)),
        uniforms,
        uniform_blocks
    );

    app->data.white_piece_diffuse_texture = Texture::create(
        app->assets_data->white_piece_diff_texture, true, -1.5f, app->options.anisotropic_filtering
    );
    app->data.black_piece_diffuse_texture = Texture::create(
        app->assets_data->black_piece_diff_texture, true, -1.5f, app->options.anisotropic_filtering
    );

    app->data.tinted_wood_material = std::make_shared<Material>(app->data.piece_shader, Material::Hoverable);
    app->data.tinted_wood_material->add_texture("u_material.diffuse");
    app->data.tinted_wood_material->add_variable(Material::UniformType::Vec3, "u_material.specular");
    app->data.tinted_wood_material->add_variable(Material::UniformType::Float, "u_material.shininess");
    app->data.tinted_wood_material->add_variable(Material::UniformType::Vec3, "u_material.tint");

    for (unsigned int i = 0; i < 9; i++) {
        prepare_piece(i, Piece::Type::White, app->assets_data->white_piece_mesh,
                app->data.white_piece_diffuse_texture, glm::vec3(-4.0f, 0.3f, -2.0f + i * 0.5f));
    }
    for (unsigned int i = 9; i < 18; i++) {
        prepare_piece(i, Piece::Type::Black, app->assets_data->black_piece_mesh,
                app->data.black_piece_diffuse_texture, glm::vec3(4.0f, 0.3f, -2.0f + (i - 9) * 0.5f));
    }
}

void GameLayer::prepare_piece(unsigned int index, Piece::Type type, std::shared_ptr<mesh::Mesh<mesh::Vertex>> mesh,
        std::shared_ptr<Texture> texture, const glm::vec3& position) {
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

    if (type == Piece::Type::White) {  // TODO see if branch is really needed
        app->data.piece_material_instances[index] = MaterialInstance::make(app->data.tinted_wood_material);
        app->data.piece_material_instances[index]->set_texture("u_material.diffuse", texture, 0);
        app->data.piece_material_instances[index]->set_vec3("u_material.specular", glm::vec3(0.2f));
        app->data.piece_material_instances[index]->set_float("u_material.shininess", 4.0f);
        app->data.piece_material_instances[index]->set_vec3("u_material.tint", glm::vec3(1.0f));
    } else {
        app->data.piece_material_instances[index] = MaterialInstance::make(app->data.tinted_wood_material);
        app->data.piece_material_instances[index]->set_texture("u_material.diffuse", texture, 0);
        app->data.piece_material_instances[index]->set_vec3("u_material.specular", glm::vec3(0.2f));
        app->data.piece_material_instances[index]->set_float("u_material.shininess", 4.0f);
        app->data.piece_material_instances[index]->set_vec3("u_material.tint", glm::vec3(1.0f));
    }
}

void GameLayer::prepare_nodes() {
    std::vector<std::string> uniforms = {
        "u_model_matrix",
        "u_color"
    };
    app->data.node_shader = Shader::create(
        convert(paths::path_for_assets(assets::NODE_VERTEX_SHADER)),
        convert(paths::path_for_assets(assets::NODE_FRAGMENT_SHADER)),
        uniforms,
        { app->renderer->get_projection_view_uniform_block() }
    );

    app->data.basic_material = std::make_shared<Material>(app->data.node_shader, Material::Hoverable);
    app->data.basic_material->add_variable(Material::UniformType::Vec4, "u_color");

    for (unsigned int i = 0; i < 24; i++) {
        prepare_node(i, NODE_POSITIONS[i]);
    }
}

void GameLayer::prepare_node(unsigned int index, const glm::vec3& position) {
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

void GameLayer::resetup_textures() {
    app->data.board_diffuse_texture = Texture::create(
        app->assets_data->board_wood_diff_texture, true, -2.0f, app->options.anisotropic_filtering
    );
     app->data.board_material_instance->set_texture(
        "u_material.diffuse", app->data.board_diffuse_texture, 0
    );

    app->data.board_paint_diffuse_texture = Texture::create(
        app->assets_data->board_paint_diff_texture, true, -1.0f, app->options.anisotropic_filtering
    );
    app->data.board_paint_material_instance->set_texture(
        "u_material.diffuse",
        app->data.board_paint_diffuse_texture, 0
    );

    app->data.white_piece_diffuse_texture = Texture::create(
        app->assets_data->white_piece_diff_texture, true, -1.5f, app->options.anisotropic_filtering
    );
    app->data.black_piece_diffuse_texture = Texture::create(
        app->assets_data->black_piece_diff_texture, true, -1.5f, app->options.anisotropic_filtering
    );
    for (unsigned int i = 0; i < 9; i++) {
        app->data.piece_material_instances[i]->set_texture(
            "u_material.diffuse",
            app->data.white_piece_diffuse_texture, 0
        );
    }
    for (unsigned int i = 9; i < 18; i++) {
        app->data.piece_material_instances[i]->set_texture(
            "u_material.diffuse",
            app->data.black_piece_diffuse_texture, 0
        );
    }
}

void GameLayer::setup_board() {
    board = Board(board_state_history);

    board.model.vertex_array = app->data.board_vertex_array;
    board.model.index_count = app->assets_data->board_mesh->indices.size();
    board.model.scale = 20.0f;
    board.model.material = app->data.board_material_instance;

    app->renderer->add_model(board.model, Renderer::CastShadow | Renderer::HasShadow);

    DEB_DEBUG("Built board");
}

void GameLayer::setup_board_paint() {
    board.paint_model.vertex_array = app->data.board_paint_vertex_array;
    board.paint_model.index_count = app->assets_data->board_paint_mesh->indices.size();
    board.paint_model.position = glm::vec3(0.0f, 0.062f, 0.0f);
    board.paint_model.scale = 20.0f;
    board.paint_model.material = app->data.board_paint_material_instance;

    app->renderer->add_model(board.paint_model, Renderer::HasShadow);

    DEB_DEBUG("Built board paint");
}

void GameLayer::setup_pieces() {
    for (unsigned int i = 0; i < 9; i++) {
        setup_piece(i, Piece::Type::White, app->assets_data->white_piece_mesh,
                app->data.white_piece_diffuse_texture, glm::vec3(-4.0f, 0.3f, -2.0f + i * 0.5f));
    }
    for (unsigned int i = 9; i < 18; i++) {
        setup_piece(i, Piece::Type::Black, app->assets_data->black_piece_mesh,
                app->data.black_piece_diffuse_texture, glm::vec3(4.0f, 0.3f, -2.0f + (i - 9) * 0.5f));
    }
}

void GameLayer::setup_piece(unsigned int index, Piece::Type type, std::shared_ptr<mesh::Mesh<mesh::Vertex>> mesh,
        std::shared_ptr<Texture> texture, const glm::vec3& position) {
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
    for (unsigned int i = 0; i < 24; i++) {
        setup_node(i, NODE_POSITIONS[i]);
    }
}

void GameLayer::setup_node(unsigned int index, const glm::vec3& position) {
    board.nodes[index] = Node(app->data.nodes_id[index], index);

    board.nodes[index].model.vertex_array = app->data.node_vertex_arrays[index];
    board.nodes[index].model.index_count = app->assets_data->node_mesh->indices.size();
    board.nodes[index].model.position = position;
    board.nodes[index].model.scale = 20.0f;
    board.nodes[index].model.material = app->data.node_material_instances[index];

    app->renderer->add_model(board.nodes[index].model, Renderer::NoLighting);

    DEB_DEBUG("Built node {}", index);
}

void GameLayer::setup_camera() {
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

    DEB_DEBUG("Setup camera");
}

void GameLayer::setup_skybox() {
    std::array<std::shared_ptr<TextureData>, 6> data = {
        app->assets_data->skybox_px_texture,
        app->assets_data->skybox_nx_texture,
        app->assets_data->skybox_py_texture,
        app->assets_data->skybox_ny_texture,
        app->assets_data->skybox_pz_texture,
        app->assets_data->skybox_nz_texture
    };
    app->renderer->set_skybox(Texture3D::create(data));

    DEB_DEBUG("Setup skybox");
}

void GameLayer::setup_light() {
    if (app->options.skybox == options::FIELD) {
        app->renderer->light = LIGHT_FIELD;
    } else if (app->options.skybox == options::AUTUMN) {
        app->renderer->light = LIGHT_AUTUMN;
    } else {
        ASSERT(false, "Invalid skybox");
    }

    DEB_DEBUG("Setup light");
}

void GameLayer::set_scene_framebuffer(int samples) {
    if (app->renderer->get_scene_framebuffer()->get_specification().samples == samples) {
        return;
    }

    FramebufferSpecification specification;
    specification.width = app->app_data.width;
    specification.height = app->app_data.height;
    specification.samples = app->options.samples;
    specification.color_attachments = {
        Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture),
        Attachment(AttachmentFormat::RED_I, AttachmentType::Texture)
    };
    specification.depth_attachment = Attachment(
        AttachmentFormat::DEPTH24_STENCIL8, AttachmentType::Renderbuffer
    );

    app->renderer->set_scene_framebuffer(Framebuffer::create(specification));

    app->purge_framebuffers();
    app->add_framebuffer(app->renderer->get_scene_framebuffer());
}

void GameLayer::set_texture_quality(std::string_view quality) {
    using namespace assets;

    // quality is the new option; options.texture_quality is not set yet

    if (quality == app->options.texture_quality) {
        return;
    }

    // Don't do anything, if there are assets already loading
    if (changed_skybox || changed_texture_quality) {
        DEB_WARN("Assets already loading");
        return;
    }

    app->options.texture_quality = quality;

    if (quality == options::NORMAL) {
        if (app->options.skybox == options::FIELD) {
            loader = std::make_unique<Loader<AssetsData>>(app->assets_data, assets_data::field_texture);
        } else if (app->options.skybox == options::AUTUMN) {
            loader = std::make_unique<Loader<AssetsData>>(app->assets_data, assets_data::autumn_texture);
        } else {
            ASSERT(false, "Invalid skybox");
        }
    } else if (quality == options::LOW) {
        if (app->options.skybox == options::FIELD) {
            loader = std::make_unique<Loader<AssetsData>>(app->assets_data, assets_data::field_low_tex_texture);
        } else if (app->options.skybox == options::AUTUMN) {
            loader = std::make_unique<Loader<AssetsData>>(app->assets_data, assets_data::autumn_low_tex_texture);
        } else {
            ASSERT(false, "Invalid skybox");
        }
    } else {
        ASSERT(false, "Invalid texture quality");
    }

    changed_texture_quality = true;
    loader->start_loading_thread();
}

void GameLayer::set_skybox(std::string_view skybox) {
    using namespace assets;

    // skybox is the new option; options.skybox is not set yet

    if (skybox == app->options.skybox) {
        return;
    }

    // Don't do anything, if there are assets already loading
    if (changed_skybox || changed_texture_quality) {
        DEB_WARN("Assets already loading");
        return;
    }

    app->options.skybox = skybox;

    if (skybox == options::FIELD) {
        if (app->options.texture_quality == options::NORMAL) {
            loader = std::make_unique<Loader<AssetsData>>(app->assets_data, assets_data::field_skybox);
        } else if (app->options.texture_quality == options::LOW) {
            loader = std::make_unique<Loader<AssetsData>>(app->assets_data, assets_data::field_low_tex_skybox);
        } else {
            ASSERT(false, "Invalid texture quality");
        }
    } else if (skybox == options::AUTUMN) {
        if (app->options.texture_quality == options::NORMAL) {
            loader = std::make_unique<Loader<AssetsData>>(app->assets_data, assets_data::autumn_skybox);
        } else if (app->options.texture_quality == options::LOW) {
            loader = std::make_unique<Loader<AssetsData>>(app->assets_data, assets_data::autumn_low_tex_skybox);
        } else {
            ASSERT(false, "Invalid texture quality");
        }
    } else {
        ASSERT(false, "Invalid skybox");
    }

    loader->start_loading_thread();
    changed_skybox = true;
}

void GameLayer::actually_change_texture_quality() {
    app->data.board_diffuse_texture = Texture::create(
        app->assets_data->board_wood_diff_texture, true, -2.0f, app->options.anisotropic_filtering
    );
    app->data.board_material_instance->set_texture(
        "u_material.diffuse", app->data.board_diffuse_texture, 0
    );

    app->data.board_paint_diffuse_texture = Texture::create(
        app->assets_data->board_paint_diff_texture, true, -1.0f, app->options.anisotropic_filtering
    );
    app->data.board_paint_material_instance->set_texture(
        "u_material.diffuse",
        app->data.board_paint_diffuse_texture, 0
    );

    app->data.white_piece_diffuse_texture = Texture::create(
        app->assets_data->white_piece_diff_texture, true, -1.5f, app->options.anisotropic_filtering
    );
    app->data.black_piece_diffuse_texture = Texture::create(
        app->assets_data->black_piece_diff_texture, true, -1.5f, app->options.anisotropic_filtering
    );
    for (unsigned int i = 0; i < 9; i++) {
        app->data.piece_material_instances[i]->set_texture(
            "u_material.diffuse",
            app->data.white_piece_diffuse_texture, 0
        );
    }
    for (unsigned int i = 9; i < 18; i++) {
        app->data.piece_material_instances[i]->set_texture(
            "u_material.diffuse",
            app->data.black_piece_diffuse_texture, 0
        );
    }

    std::array<std::shared_ptr<TextureData>, 6> data = {
        app->assets_data->skybox_px_texture,
        app->assets_data->skybox_nx_texture,
        app->assets_data->skybox_py_texture,
        app->assets_data->skybox_ny_texture,
        app->assets_data->skybox_pz_texture,
        app->assets_data->skybox_nz_texture
    };
    app->renderer->set_skybox(Texture3D::create(data));
}

void GameLayer::actually_change_skybox() {
    std::array<std::shared_ptr<TextureData>, 6> data = {
        app->assets_data->skybox_px_texture,
        app->assets_data->skybox_nx_texture,
        app->assets_data->skybox_py_texture,
        app->assets_data->skybox_ny_texture,
        app->assets_data->skybox_pz_texture,
        app->assets_data->skybox_nz_texture
    };
    app->renderer->set_skybox(Texture3D::create(data));

    if (app->options.skybox == options::FIELD) {
        app->renderer->light = LIGHT_FIELD;
    } else if (app->options.skybox == options::AUTUMN) {
        app->renderer->light = LIGHT_AUTUMN;
    } else {
        ASSERT(false, "Invalid skybox");
    }
}

void GameLayer::load_game() {
    if (imgui_layer->last_save_game_date == save_load::NO_LAST_GAME) {
        imgui_layer->show_no_last_game = true;
        return;
    }

    save_load::GameState state;
    try {
        save_load::load_game_from_file(state);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_ERROR("{}", e.what());
        save_load::handle_save_file_not_open_error();
        REL_ERROR("Could not load game");
        imgui_layer->show_could_not_load_game = true;
        return;
    } catch (const save_load::SaveFileError& e) {
        REL_ERROR("{}", e.what());  // TODO maybe delete file
        REL_ERROR("Could not load game");
        imgui_layer->show_could_not_load_game = true;
        return;
    }

    app->camera = state.camera;

    Renderer::Model& model = board.model;
    model.index_count = state.board.model.index_count;
    model.position = state.board.model.position;
    model.rotation = state.board.model.rotation;
    model.scale = state.board.model.scale;
    model.outline_color = state.board.model.outline_color;

    Renderer::Model& paint_model = board.paint_model;
    paint_model.index_count = state.board.paint_model.index_count;
    paint_model.position = state.board.paint_model.position;
    paint_model.rotation = state.board.paint_model.rotation;
    paint_model.scale = state.board.paint_model.scale;
    paint_model.outline_color = state.board.paint_model.outline_color;

    for (unsigned int i = 0; i < 24; i++) {
        Node& node = board.nodes[i];
        node.id = state.board.nodes[i].id;
        node.model.index_count = state.board.nodes[i].model.index_count;
        node.model.position = state.board.nodes[i].model.position;
        node.model.rotation = state.board.nodes[i].model.rotation;
        node.model.scale = state.board.nodes[i].model.scale;
        node.model.outline_color = state.board.nodes[i].model.outline_color;
        node.piece_id = state.board.nodes[i].piece_id;
        node.piece = nullptr;  // It must be NULL, if the ids don't match
        // Assign correct addresses
        for (unsigned int i = 0; i < 18; i++) {
            if (state.board.pieces[i].id == node.piece_id) {
                node.piece = &board.pieces[i];
                break;
            }
        }
        node.index = state.board.nodes[i].index;
    }

    for (unsigned int i = 0; i < 18; i++) {
        Piece& piece = board.pieces[i];
        piece.id = state.board.pieces[i].id;
        piece.model.index_count = state.board.pieces[i].model.index_count;
        piece.model.position = state.board.pieces[i].model.position;
        piece.model.rotation = state.board.pieces[i].model.rotation;
        piece.model.scale = state.board.pieces[i].model.scale;
        piece.model.outline_color = state.board.pieces[i].model.outline_color;
        piece.movement.type = state.board.pieces[i].movement.type;
        piece.movement.velocity = state.board.pieces[i].movement.velocity;
        piece.movement.target = state.board.pieces[i].movement.target;
        piece.movement.target0 = state.board.pieces[i].movement.target0;
        piece.movement.target1 = state.board.pieces[i].movement.target1;
        piece.movement.reached_target0 = state.board.pieces[i].movement.reached_target0;
        piece.movement.reached_target1 = state.board.pieces[i].movement.reached_target1;
        piece.should_move = state.board.pieces[i].should_move;
        piece.type = state.board.pieces[i].type;
        piece.in_use = state.board.pieces[i].in_use;
        piece.node_id = state.board.pieces[i].node_id;
        piece.node = nullptr;  // It must be NULL, if the ids don't match
        // Assign correct addresses; use only board as nodes have already been assigned
        for (Node& node : board.nodes) {
            if (node.id == piece.node_id) {
                piece.node = &node;
                break;
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
    board.hovered_node = nullptr;
    board.hovered_piece = nullptr;
    board.selected_piece = nullptr;
    board.can_jump = state.board.can_jump;
    board.turns_without_mills = state.board.turns_without_mills;
    board.repetition_history = state.board.repetition_history;
    board.state_history = state.board.state_history;
    board.next_move = state.board.next_move;

    // Reset pieces' models
    for (Piece& piece : board.pieces) {
        app->renderer->remove_model(piece.model.handle);

        if (piece.active) {
            app->renderer->add_model(piece.model, Renderer::CastShadow | Renderer::HasShadow);
        }
    }

    gui_layer->timer.stop();
    gui_layer->timer.set_time(state.time);
    first_move = false;

    board.update_cursor();

    DEB_INFO("Done loading game");
}
