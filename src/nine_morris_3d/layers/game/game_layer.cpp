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
#include "other/paths.h"
#include "other/mesh.h"
#include "other/loader.h"
#include "other/texture_data.h"
#include "other/logging.h"
#include "other/assert.h"
#include "other/encryption.h"

using namespace encryption;
using namespace mesh;

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
    board = Board();
    for (unsigned int i = 0; i < 9; i++) {
        board.pieces[i] = Piece(app->data.pieces_id[i], Piece::Type::White);
        board.pieces[i].model.position = glm::vec3(-4.0f, 0.3f, -2.0f + i * 0.5f);
        board.pieces[i].model.rotation = glm::vec3(0.0f, glm::radians(static_cast<float>(rand() % 360)), 0.0f);
    }
    for (unsigned int i = 9; i < 18; i++) {
        board.pieces[i] = Piece(app->data.pieces_id[i], Piece::Type::Black);
        board.pieces[i].model.position = glm::vec3(4.0f, 0.3f, -2.0f + (i - 9) * 0.5f);
        board.pieces[i].model.rotation = glm::vec3(0.0f, glm::radians(static_cast<float>(rand() % 360)), 0.0f);
    }
    for (unsigned int i = 0; i < 24; i++) {
        board.nodes[i] = Node(app->data.nodes_id[i], i);
    }

    if (app->options.normal_mapping) {
        setup_board();
        setup_board_paint();
        setup_pieces();
    } else {
        setup_board_no_normal();
        setup_board_paint_no_normal();
        setup_pieces_no_normal();
    }

    setup_nodes();
    setup_camera();

    keyboard = KeyboardControls(&board);

    app->window->set_cursor(app->options.custom_cursor ? app->arrow_cursor : 0);
    app->renderer->add_quad(keyboard.quad);

#ifdef PLATFORM_GAME_DEBUG
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

        time_t current;
        time(&current);
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

#ifdef PLATFORM_GAME_DEBUG
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

    if (app->options.normal_mapping) {
        prepare_board();
        prepare_board_paint();
        prepare_pieces();
    } else {
        prepare_board_no_normal();
        prepare_board_paint_no_normal();
        prepare_pieces_no_normal();
    }

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

    // Short-circuit optimization
    if (changed_labeled_board_texture && loader->done_loading()) {
        actually_change_labeled_board_texture();

        changed_labeled_board_texture = false;

        loader->get_thread().join();
        loader = nullptr;
    }

    // Short-circuit optimization
    if (changed_normal_mapping && loader->done_loading()) {
        actually_change_normal_mapping();

        changed_normal_mapping = false;

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
    dispatcher.dispatch<KeyPressedEvent>(KeyPressed, BIND(GameLayer::on_key_pressed));
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
    if (event.button == input::MouseButton::LEFT) {
        if (board.next_move) {
            board.press(app->renderer->get_hovered_id());
        }
    }

    return false;
}

bool GameLayer::on_mouse_button_released(events::MouseButtonReleasedEvent& event) {
    if (event.button == input::MouseButton::LEFT) {
        if (board.next_move) {
            bool did = false;

            if (board.phase == Board::Phase::PlacePieces) {
                if (board.should_take_piece) {
                    did = board.take_piece(app->renderer->get_hovered_id());
                } else {
                    did = board.place_piece(app->renderer->get_hovered_id());
                }
            } else if (board.phase == Board::Phase::MovePieces) {
                if (board.should_take_piece) {
                    did = board.take_piece(app->renderer->get_hovered_id());
                } else {
                    board.select_piece(app->renderer->get_hovered_id());
                    did = board.put_down_piece(app->renderer->get_hovered_id());
                }
            }

            if (did && !first_move && !gui_layer->timer.get_running()) {
                gui_layer->timer.start(app->window->get_time());
                first_move = true;
            }

            if (board.phase == Board::Phase::GameOver) {
                gui_layer->timer.stop();
            }

            if (board.redo_state_history->empty()) {
                imgui_layer->can_redo = false;
            }

            board.release();
        }
    }

    return false;
}

bool GameLayer::on_key_pressed(events::KeyPressedEvent& event) {
    if (event.key == input::Key::UP) {
        const KeyboardControls::Direction direction = KeyboardControls::calculate(
            KeyboardControls::Direction::Up, app->camera.get_angle_around_point()
        );
        keyboard.move(direction);
    } else if (event.key == input::Key::DOWN) {
        const KeyboardControls::Direction direction = KeyboardControls::calculate(
            KeyboardControls::Direction::Down, app->camera.get_angle_around_point()
        );
        keyboard.move(direction);
    } else if (event.key == input::Key::LEFT) {
        const KeyboardControls::Direction direction = KeyboardControls::calculate(
            KeyboardControls::Direction::Left, app->camera.get_angle_around_point()
        );
        keyboard.move(direction);
    } else if (event.key == input::Key::RIGHT) {
        const KeyboardControls::Direction direction = KeyboardControls::calculate(
            KeyboardControls::Direction::Right, app->camera.get_angle_around_point()
        );
        keyboard.move(direction);
    } else if (event.key == input::Key::ENTER) {
        if (board.next_move) {
            const bool did = keyboard.press(first_move);

            if (did && !first_move && !gui_layer->timer.get_running()) {
                gui_layer->timer.start(app->window->get_time());
                first_move = true;
            }

            if (board.phase == Board::Phase::GameOver) {
                gui_layer->timer.stop();
            }

            if (board.redo_state_history->empty()) {
                imgui_layer->can_redo = false;
            }

            board.release();
        }
    }

    return false;
}

bool GameLayer::on_key_released(events::KeyReleasedEvent& event) {
    if (event.key == input::Key::SPACE) {
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
        "u_material.shininess",
        "u_material.normal"
    };
    std::vector<UniformBlockSpecification> uniform_blocks = {
        app->renderer->get_projection_view_uniform_block(),
        app->renderer->get_light_uniform_block(),
        app->renderer->get_light_view_position_uniform_block(),
        app->renderer->get_light_space_uniform_block()
    };
    app->data.board_wood_shader = Shader::create(
        convert(paths::path_for_assets(assets::BOARD_VERTEX_SHADER)),
        convert(paths::path_for_assets(assets::BOARD_FRAGMENT_SHADER)),
        uniforms,
        uniform_blocks
    );

    std::shared_ptr<Buffer> vertices = Buffer::create(
        app->assets_data->board_wood_mesh->vertices.data(),
        app->assets_data->board_wood_mesh->vertices.size() * sizeof(VPTNT)
    );

    app->data.board_wood_vertex_array = VertexArray::create();

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    layout.add(1, BufferLayout::Type::Float, 2);
    layout.add(2, BufferLayout::Type::Float, 3);
    layout.add(3, BufferLayout::Type::Float, 3);

    std::shared_ptr<IndexBuffer> indices = IndexBuffer::create(
        app->assets_data->board_wood_mesh->indices.data(),
        app->assets_data->board_wood_mesh->indices.size() * sizeof(unsigned int)
    );

    app->data.board_wood_vertex_array->add_buffer(vertices, layout);
    app->data.board_wood_vertex_array->add_index_buffer(indices);

    VertexArray::unbind();

    app->data.board_wood_diffuse_texture = Texture::create(
        app->assets_data->board_wood_diff_texture, true, -2.0f, app->options.anisotropic_filtering
    );

    app->data.board_normal_texture = Texture::create(
        app->assets_data->board_norm_texture, true, -2.0f, app->options.anisotropic_filtering
    );

    app->data.wood_material = std::make_shared<Material>(app->data.board_wood_shader);
    app->data.wood_material->add_texture("u_material.diffuse");
    app->data.wood_material->add_variable(Material::UniformType::Vec3, "u_material.specular");
    app->data.wood_material->add_variable(Material::UniformType::Float, "u_material.shininess");
    app->data.wood_material->add_texture("u_material.normal");

    app->data.board_wood_material_instance = MaterialInstance::make(app->data.wood_material);
    app->data.board_wood_material_instance->set_texture("u_material.diffuse", app->data.board_wood_diffuse_texture, 0);
    app->data.board_wood_material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    app->data.board_wood_material_instance->set_float("u_material.shininess", 4.0f);
    app->data.board_wood_material_instance->set_texture("u_material.normal", app->data.board_normal_texture, 1);
}

void GameLayer::prepare_board_paint() {
    std::vector<std::string> uniforms = {
        "u_model_matrix",
        "u_shadow_map",
        "u_material.diffuse",
        "u_material.specular",
        "u_material.shininess",
        "u_material.normal"
    };
    std::vector<UniformBlockSpecification> uniform_blocks = {
        app->renderer->get_projection_view_uniform_block(),
        app->renderer->get_light_uniform_block(),
        app->renderer->get_light_view_position_uniform_block(),
        app->renderer->get_light_space_uniform_block()
    };
    app->data.board_paint_shader = Shader::create(
        convert(paths::path_for_assets(assets::BOARD_PAINT_VERTEX_SHADER)),
        convert(paths::path_for_assets(assets::BOARD_PAINT_FRAGMENT_SHADER)),
        uniforms,
        uniform_blocks
    );

    std::shared_ptr<Buffer> vertices = Buffer::create(
        app->assets_data->board_paint_mesh->vertices.data(),
        app->assets_data->board_paint_mesh->vertices.size() * sizeof(VPTNT)
    );

    app->data.board_paint_vertex_array = VertexArray::create();

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    layout.add(1, BufferLayout::Type::Float, 2);
    layout.add(2, BufferLayout::Type::Float, 3);
    layout.add(3, BufferLayout::Type::Float, 3);

    std::shared_ptr<IndexBuffer> indices = IndexBuffer::create(
        app->assets_data->board_paint_mesh->indices.data(),
        app->assets_data->board_paint_mesh->indices.size() * sizeof(unsigned int)
    );

    app->data.board_paint_vertex_array->add_buffer(vertices, layout);
    app->data.board_paint_vertex_array->add_index_buffer(indices);

    VertexArray::unbind();

    app->data.board_paint_diffuse_texture = Texture::create(
        app->assets_data->board_paint_diff_texture, true, -1.0f, app->options.anisotropic_filtering
    );

    app->data.paint_material = std::make_shared<Material>(app->data.board_paint_shader);
    app->data.paint_material->add_texture("u_material.diffuse");
    app->data.paint_material->add_variable(Material::UniformType::Vec3, "u_material.specular");
    app->data.paint_material->add_variable(Material::UniformType::Float, "u_material.shininess");
    app->data.paint_material->add_texture("u_material.normal");

    app->data.board_paint_material_instance = MaterialInstance::make(app->data.paint_material);
    app->data.board_paint_material_instance->set_texture("u_material.diffuse", app->data.board_paint_diffuse_texture, 0);
    app->data.board_paint_material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    app->data.board_paint_material_instance->set_float("u_material.shininess", 4.0f);
    app->data.board_paint_material_instance->set_texture("u_material.normal", app->data.board_normal_texture, 1);
}

void GameLayer::prepare_pieces() {
    std::vector<std::string> uniforms = {
        "u_model_matrix",
        "u_shadow_map",
        "u_material.diffuse",
        "u_material.specular",
        "u_material.shininess",
        "u_material.normal",
        "u_material.tint"
    };
    std::vector<UniformBlockSpecification> uniform_blocks = {
        app->renderer->get_projection_view_uniform_block(),
        app->renderer->get_light_uniform_block(),
        app->renderer->get_light_view_position_uniform_block(),
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
    app->data.piece_normal_texture = Texture::create(
        app->assets_data->piece_norm_texture, true, -1.5f, app->options.anisotropic_filtering
    );

    app->data.tinted_wood_material = std::make_shared<Material>(app->data.piece_shader, Material::Hoverable);
    app->data.tinted_wood_material->add_texture("u_material.diffuse");
    app->data.tinted_wood_material->add_variable(Material::UniformType::Vec3, "u_material.specular");
    app->data.tinted_wood_material->add_variable(Material::UniformType::Float, "u_material.shininess");
    app->data.tinted_wood_material->add_texture("u_material.normal");
    app->data.tinted_wood_material->add_variable(Material::UniformType::Vec3, "u_material.tint");

    for (unsigned int i = 0; i < 9; i++) {
        prepare_piece(
            i, Piece::Type::White, app->assets_data->white_piece_mesh,
            app->data.white_piece_diffuse_texture
        );
    }
    for (unsigned int i = 9; i < 18; i++) {
        prepare_piece(
            i, Piece::Type::Black, app->assets_data->black_piece_mesh,
            app->data.black_piece_diffuse_texture
        );
    }
}

void GameLayer::prepare_piece(unsigned int index, Piece::Type type, std::shared_ptr<Mesh<VPTNT>> mesh,
        std::shared_ptr<Texture> diffuse_texture) {
    hoverable::Id id = hoverable::generate_id();
    app->data.pieces_id[index] = id;

    std::shared_ptr<Buffer> vertices = Buffer::create(mesh->vertices.data(),
        mesh->vertices.size() * sizeof(VPTNT));

    std::shared_ptr<Buffer> ids = create_ids_buffer(mesh->vertices.size(), id);

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    layout.add(1, BufferLayout::Type::Float, 2);
    layout.add(2, BufferLayout::Type::Float, 3);
    layout.add(3, BufferLayout::Type::Float, 3);

    BufferLayout layout2;
    layout2.add(4, BufferLayout::Type::Int, 1);

    std::shared_ptr<IndexBuffer> indices = IndexBuffer::create(mesh->indices.data(),
            mesh->indices.size() * sizeof(unsigned int));

    app->data.piece_vertex_arrays[index] = VertexArray::create();
    app->data.piece_vertex_arrays[index]->add_buffer(vertices, layout);
    app->data.piece_vertex_arrays[index]->add_buffer(ids, layout2);
    app->data.piece_vertex_arrays[index]->add_index_buffer(indices);

    VertexArray::unbind();

    if (type == Piece::Type::White) {  // TODO see if branch is really needed
        app->data.piece_material_instances[index] = MaterialInstance::make(app->data.tinted_wood_material);
        app->data.piece_material_instances[index]->set_texture("u_material.diffuse", diffuse_texture, 0);
        app->data.piece_material_instances[index]->set_vec3("u_material.specular", glm::vec3(0.2f));
        app->data.piece_material_instances[index]->set_float("u_material.shininess", 4.0f);
        app->data.piece_material_instances[index]->set_texture("u_material.normal", app->data.piece_normal_texture, 1);
        app->data.piece_material_instances[index]->set_vec3("u_material.tint", glm::vec3(1.0f));
    } else {
        app->data.piece_material_instances[index] = MaterialInstance::make(app->data.tinted_wood_material);
        app->data.piece_material_instances[index]->set_texture("u_material.diffuse", diffuse_texture, 0);
        app->data.piece_material_instances[index]->set_vec3("u_material.specular", glm::vec3(0.2f));
        app->data.piece_material_instances[index]->set_float("u_material.shininess", 4.0f);
        app->data.piece_material_instances[index]->set_texture("u_material.normal", app->data.piece_normal_texture, 1);
        app->data.piece_material_instances[index]->set_vec3("u_material.tint", glm::vec3(1.0f));
    }
}

void GameLayer::prepare_board_no_normal() {
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
        app->renderer->get_light_view_position_uniform_block(),
        app->renderer->get_light_space_uniform_block()
    };
    app->data.board_wood_shader = Shader::create(
        convert(paths::path_for_assets(assets::BOARD_NO_NORMAL_VERTEX_SHADER)),
        convert(paths::path_for_assets(assets::BOARD_NO_NORMAL_FRAGMENT_SHADER)),
        uniforms,
        uniform_blocks
    );

    std::shared_ptr<Buffer> vertices = Buffer::create(
        app->assets_data->board_wood_no_normal_mesh->vertices.data(),
        app->assets_data->board_wood_no_normal_mesh->vertices.size() * sizeof(VPTN)
    );

    app->data.board_wood_vertex_array = VertexArray::create();

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    layout.add(1, BufferLayout::Type::Float, 2);
    layout.add(2, BufferLayout::Type::Float, 3);

    std::shared_ptr<IndexBuffer> indices = IndexBuffer::create(
        app->assets_data->board_wood_no_normal_mesh->indices.data(),
        app->assets_data->board_wood_no_normal_mesh->indices.size() * sizeof(unsigned int)
    );

    app->data.board_wood_vertex_array->add_buffer(vertices, layout);
    app->data.board_wood_vertex_array->add_index_buffer(indices);

    VertexArray::unbind();

    app->data.board_wood_diffuse_texture = Texture::create(
        app->assets_data->board_wood_diff_texture, true, -2.0f, app->options.anisotropic_filtering
    );

    app->data.wood_material = std::make_shared<Material>(app->data.board_wood_shader);
    app->data.wood_material->add_texture("u_material.diffuse");
    app->data.wood_material->add_variable(Material::UniformType::Vec3, "u_material.specular");
    app->data.wood_material->add_variable(Material::UniformType::Float, "u_material.shininess");

    app->data.board_wood_material_instance = MaterialInstance::make(app->data.wood_material);
    app->data.board_wood_material_instance->set_texture("u_material.diffuse", app->data.board_wood_diffuse_texture, 0);
    app->data.board_wood_material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    app->data.board_wood_material_instance->set_float("u_material.shininess", 4.0f);
}

void GameLayer::prepare_board_paint_no_normal() {
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
        app->renderer->get_light_view_position_uniform_block(),
        app->renderer->get_light_space_uniform_block()
    };
    app->data.board_paint_shader = Shader::create(
        convert(paths::path_for_assets(assets::BOARD_PAINT_NO_NORMAL_VERTEX_SHADER)),
        convert(paths::path_for_assets(assets::BOARD_PAINT_NO_NORMAL_FRAGMENT_SHADER)),
        uniforms,
        uniform_blocks
    );

    std::shared_ptr<Buffer> vertices = Buffer::create(
        app->assets_data->board_paint_no_normal_mesh->vertices.data(),
        app->assets_data->board_paint_no_normal_mesh->vertices.size() * sizeof(VPTN)
    );

    app->data.board_paint_vertex_array = VertexArray::create();

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    layout.add(1, BufferLayout::Type::Float, 2);
    layout.add(2, BufferLayout::Type::Float, 3);

    std::shared_ptr<IndexBuffer> indices = IndexBuffer::create(
        app->assets_data->board_paint_no_normal_mesh->indices.data(),
        app->assets_data->board_paint_no_normal_mesh->indices.size() * sizeof(unsigned int)
    );

    app->data.board_paint_vertex_array->add_buffer(vertices, layout);
    app->data.board_paint_vertex_array->add_index_buffer(indices);

    VertexArray::unbind();

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

void GameLayer::prepare_pieces_no_normal() {
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
        app->renderer->get_light_view_position_uniform_block(),
        app->renderer->get_light_space_uniform_block()
    };
    app->data.piece_shader = Shader::create(
        convert(paths::path_for_assets(assets::PIECE_NO_NORMAL_VERTEX_SHADER)),
        convert(paths::path_for_assets(assets::PIECE_NO_NORMAL_FRAGMENT_SHADER)),
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
        prepare_piece_no_normal(
            i, Piece::Type::White, app->assets_data->white_piece_no_normal_mesh,
            app->data.white_piece_diffuse_texture
        );
    }
    for (unsigned int i = 9; i < 18; i++) {
        prepare_piece_no_normal(
            i, Piece::Type::Black, app->assets_data->black_piece_no_normal_mesh,
            app->data.black_piece_diffuse_texture
        );
    }
}

void GameLayer::prepare_piece_no_normal(unsigned int index, Piece::Type type, std::shared_ptr<Mesh<VPTN>> mesh,
        std::shared_ptr<Texture> diffuse_texture) {
    hoverable::Id id = hoverable::generate_id();
    app->data.pieces_id[index] = id;

    std::shared_ptr<Buffer> vertices = Buffer::create(mesh->vertices.data(),
        mesh->vertices.size() * sizeof(VPTN));

    std::shared_ptr<Buffer> ids = create_ids_buffer(mesh->vertices.size(), id);

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    layout.add(1, BufferLayout::Type::Float, 2);
    layout.add(2, BufferLayout::Type::Float, 3);

    BufferLayout layout2;
    layout2.add(4, BufferLayout::Type::Int, 1);

    std::shared_ptr<IndexBuffer> indices = IndexBuffer::create(mesh->indices.data(),
            mesh->indices.size() * sizeof(unsigned int));

    app->data.piece_vertex_arrays[index] = VertexArray::create();
    app->data.piece_vertex_arrays[index]->add_buffer(vertices, layout);
    app->data.piece_vertex_arrays[index]->add_buffer(ids, layout2);
    app->data.piece_vertex_arrays[index]->add_index_buffer(indices);

    VertexArray::unbind();

    if (type == Piece::Type::White) {  // TODO see if branch is really needed
        app->data.piece_material_instances[index] = MaterialInstance::make(app->data.tinted_wood_material);
        app->data.piece_material_instances[index]->set_texture("u_material.diffuse", diffuse_texture, 0);
        app->data.piece_material_instances[index]->set_vec3("u_material.specular", glm::vec3(0.2f));
        app->data.piece_material_instances[index]->set_float("u_material.shininess", 4.0f);
        app->data.piece_material_instances[index]->set_vec3("u_material.tint", glm::vec3(1.0f));
    } else {
        app->data.piece_material_instances[index] = MaterialInstance::make(app->data.tinted_wood_material);
        app->data.piece_material_instances[index]->set_texture("u_material.diffuse", diffuse_texture, 0);
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
            app->assets_data->node_mesh->vertices.size() * sizeof(VP));

    std::shared_ptr<Buffer> ids = create_ids_buffer(app->assets_data->node_mesh->vertices.size(), id);

    app->data.node_vertex_arrays[index] = VertexArray::create();

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    BufferLayout layout2;
    layout2.add(1, BufferLayout::Type::Int, 1);

    std::shared_ptr<IndexBuffer> indices = IndexBuffer::create(app->assets_data->node_mesh->indices.data(),
            app->assets_data->node_mesh->indices.size() * sizeof(unsigned int));

    app->data.node_vertex_arrays[index]->add_buffer(vertices, layout);
    app->data.node_vertex_arrays[index]->add_buffer(ids, layout2);
    app->data.node_vertex_arrays[index]->add_index_buffer(indices);

    VertexArray::unbind();

    app->data.node_material_instances[index] = MaterialInstance::make(app->data.basic_material);
    app->data.node_material_instances[index]->set_vec4("u_color", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
}

void GameLayer::resetup_textures() {
    app->data.board_wood_diffuse_texture = Texture::create(
        app->assets_data->board_wood_diff_texture, true, -2.0f, app->options.anisotropic_filtering
    );
    app->data.board_wood_material_instance->set_texture(
        "u_material.diffuse", app->data.board_wood_diffuse_texture, 0
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

    if (app->options.normal_mapping) {
        app->data.board_normal_texture = Texture::create(
            app->assets_data->board_norm_texture, true, -2.0f, app->options.anisotropic_filtering
        );
        app->data.board_wood_material_instance->set_texture(
            "u_material.normal", app->data.board_normal_texture, 1
        );
        app->data.board_paint_material_instance->set_texture(
            "u_material.normal", app->data.board_normal_texture, 1
        );
        app->data.piece_normal_texture = Texture::create(
            app->assets_data->piece_norm_texture, true, -1.5f, app->options.anisotropic_filtering
        );
        for (unsigned int i = 0; i < 9; i++) {
            app->data.piece_material_instances[i]->set_texture(
                "u_material.normal",
                app->data.piece_normal_texture, 1
            );
        }
        for (unsigned int i = 9; i < 18; i++) {
            app->data.piece_material_instances[i]->set_texture(
                "u_material.normal",
                app->data.piece_normal_texture, 1
            );
        }
    }
}

void GameLayer::setup_board() {
    board.model.vertex_array = app->data.board_wood_vertex_array;
    board.model.index_count = app->assets_data->board_wood_mesh->indices.size();
    board.model.scale = 20.0f;
    board.model.material = app->data.board_wood_material_instance;

    app->renderer->add_model(board.model, Renderer::CastShadow | Renderer::HasShadow);

    DEB_DEBUG("Built board");
}

void GameLayer::setup_board_paint() {
    board.paint_model.vertex_array = app->data.board_paint_vertex_array;
    board.paint_model.index_count = app->assets_data->board_paint_mesh->indices.size();
    board.paint_model.position = glm::vec3(0.0f, PAINT_Y_POSITION, 0.0f);
    board.paint_model.scale = 20.0f;
    board.paint_model.material = app->data.board_paint_material_instance;

    app->renderer->add_model(board.paint_model, Renderer::HasShadow);

    DEB_DEBUG("Built board paint");
}

void GameLayer::setup_pieces() {
    for (unsigned int i = 0; i < 9; i++) {
        setup_piece(
            i, Piece::Type::White, app->assets_data->white_piece_mesh
        );
    }
    for (unsigned int i = 9; i < 18; i++) {
        setup_piece(
            i, Piece::Type::Black, app->assets_data->black_piece_mesh
        );
    }
}

void GameLayer::setup_piece(unsigned int index, Piece::Type type, std::shared_ptr<Mesh<VPTNT>> mesh) {
    board.pieces[index].model.vertex_array = app->data.piece_vertex_arrays[index];
    board.pieces[index].model.index_count = mesh->indices.size();
    board.pieces[index].model.scale = 20.0f;
    board.pieces[index].model.material = app->data.piece_material_instances[index];

    app->renderer->add_model(board.pieces[index].model, Renderer::CastShadow | Renderer::HasShadow);

    DEB_DEBUG("Built piece {}", index);
}

void GameLayer::setup_board_no_normal() {
    board.model.vertex_array = app->data.board_wood_vertex_array;
    board.model.index_count = app->assets_data->board_wood_no_normal_mesh->indices.size();
    board.model.scale = 20.0f;
    board.model.material = app->data.board_wood_material_instance;

    app->renderer->add_model(board.model, Renderer::CastShadow | Renderer::HasShadow);

    DEB_DEBUG("Built board");
}

void GameLayer::setup_board_paint_no_normal() {
    board.paint_model.vertex_array = app->data.board_paint_vertex_array;
    board.paint_model.index_count = app->assets_data->board_paint_no_normal_mesh->indices.size();
    board.paint_model.position = glm::vec3(0.0f, PAINT_Y_POSITION, 0.0f);
    board.paint_model.scale = 20.0f;
    board.paint_model.material = app->data.board_paint_material_instance;

    app->renderer->add_model(board.paint_model, Renderer::HasShadow);

    DEB_DEBUG("Built board paint");
}

void GameLayer::setup_pieces_no_normal() {
    for (unsigned int i = 0; i < 9; i++) {
        setup_piece_no_normal(
            i, Piece::Type::White, app->assets_data->white_piece_no_normal_mesh
        );
    }
    for (unsigned int i = 9; i < 18; i++) {
        setup_piece_no_normal(
            i, Piece::Type::Black, app->assets_data->black_piece_no_normal_mesh
        );
    }
}

void GameLayer::setup_piece_no_normal(unsigned int index, Piece::Type type, std::shared_ptr<Mesh<VPTN>> mesh) {
    board.pieces[index].model.vertex_array = app->data.piece_vertex_arrays[index];
    board.pieces[index].model.index_count = mesh->indices.size();
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
            glm::radians(FOV),
            static_cast<float>(app->app_data.width) / app->app_data.height,
            NEAR,
            FAR
        )
    );

    default_camera_position = app->camera.get_position();

    app->camera = Camera(
        app->options.sensitivity,
        47.0f,
        glm::vec3(0.0f),
        8.7f,
        glm::perspective(
            glm::radians(FOV),
            static_cast<float>(app->app_data.width) / app->app_data.height,
            NEAR,
            FAR
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
        app->renderer->set_light(LIGHT_FIELD);
    } else if (app->options.skybox == options::AUTUMN) {
        app->renderer->set_light(LIGHT_AUTUMN);
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
    if (changed_skybox || changed_texture_quality || changed_labeled_board_texture || changed_normal_mapping) {
        DEB_WARN("Assets already loading");
        return;
    }

    app->options.texture_quality = quality;

    loader = std::make_unique<Loader<AssetsData, options::Options>>(app->assets_data, assets_data::texture_quality);
    loader->start_loading_thread(app->options);

    changed_texture_quality = true;
}

void GameLayer::set_skybox(std::string_view skybox) {
    using namespace assets;

    // skybox is the new option; options.skybox is not set yet

    if (skybox == app->options.skybox) {
        return;
    }

    // Don't do anything, if there are assets already loading
    if (changed_skybox || changed_texture_quality || changed_labeled_board_texture || changed_normal_mapping) {
        DEB_WARN("Assets already loading");
        return;
    }

    app->options.skybox = skybox;

    loader = std::make_unique<Loader<AssetsData, options::Options>>(app->assets_data, assets_data::skybox);
    loader->start_loading_thread(app->options);

    changed_skybox = true;
}

void GameLayer::set_labeled_board_texture(bool labeled_board) {
    using namespace assets;

    // labeled_board is the new option; options.labeled_board is not set yet

    if (labeled_board == app->options.labeled_board) {
        return;
    }

    // Don't do anything, if there are assets already loading
    if (changed_skybox || changed_texture_quality || changed_labeled_board_texture || changed_normal_mapping) {
        DEB_WARN("Assets already loading");
        return;
    }

    app->options.labeled_board = labeled_board;

    loader = std::make_unique<Loader<AssetsData, options::Options>>(app->assets_data, assets_data::board_paint_texture);
    loader->start_loading_thread(app->options);

    changed_labeled_board_texture = true;
}

void GameLayer::set_normal_mapping(bool normal_mapping) {
    using namespace assets;

    // normal_mapping is the new option; options.normal_mapping is not set yet

    if (normal_mapping == app->options.normal_mapping) {
        return;
    }

    // Don't do anything, if there are assets already loading
    if (changed_skybox || changed_texture_quality || changed_labeled_board_texture || changed_normal_mapping) {
        DEB_WARN("Assets already loading");
        return;
    }

    app->options.normal_mapping = normal_mapping;

    loader = std::make_unique<Loader<AssetsData, options::Options>>(app->assets_data, assets_data::normal_mapping);
    loader->start_loading_thread(app->options);

    changed_normal_mapping = true;
}

void GameLayer::actually_change_texture_quality() {
    app->data.board_wood_diffuse_texture = Texture::create(
        app->assets_data->board_wood_diff_texture, true, -2.0f, app->options.anisotropic_filtering
    );
    app->data.board_wood_material_instance->set_texture(
        "u_material.diffuse", app->data.board_wood_diffuse_texture, 0
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

    if (app->options.normal_mapping) {
        app->data.board_normal_texture = Texture::create(
            app->assets_data->board_norm_texture, true, -2.0f, app->options.anisotropic_filtering
        );
        app->data.board_wood_material_instance->set_texture(
            "u_material.normal", app->data.board_normal_texture, 1
        );
        app->data.board_paint_material_instance->set_texture(
            "u_material.normal", app->data.board_normal_texture, 1
        );
        app->data.piece_normal_texture = Texture::create(
            app->assets_data->piece_norm_texture, true, -1.5f, app->options.anisotropic_filtering
        );
        for (unsigned int i = 0; i < 9; i++) {
            app->data.piece_material_instances[i]->set_texture(
                "u_material.normal",
                app->data.board_normal_texture, 1
            );
        }
        for (unsigned int i = 9; i < 18; i++) {
            app->data.piece_material_instances[i]->set_texture(
                "u_material.normal",
                app->data.board_normal_texture, 1
            );
        }
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
        app->renderer->set_light(LIGHT_FIELD);
    } else if (app->options.skybox == options::AUTUMN) {
        app->renderer->set_light(LIGHT_AUTUMN);
    } else {
        ASSERT(false, "Invalid skybox");
    }
}

void GameLayer::actually_change_labeled_board_texture() {
    app->data.board_paint_diffuse_texture = Texture::create(
        app->assets_data->board_paint_diff_texture, true, -1.0f, app->options.anisotropic_filtering
    );
    app->data.board_paint_material_instance->set_texture(
        "u_material.diffuse",
        app->data.board_paint_diffuse_texture, 0
    );
}

void GameLayer::actually_change_normal_mapping() {
    app->renderer->remove_model(board.model.handle);
    app->renderer->remove_model(board.paint_model.handle);
    for (unsigned int i = 0; i < 18; i++) {
        app->renderer->remove_model(board.pieces[i].model.handle);
    }

    if (app->options.normal_mapping) {
        prepare_board();
        prepare_board_paint();
        prepare_pieces();

        setup_board();
        setup_board_paint();
        setup_pieces();
    } else {
        prepare_board_no_normal();
        prepare_board_paint_no_normal();
        prepare_pieces_no_normal();

        setup_board_no_normal();
        setup_board_paint_no_normal();
        setup_pieces_no_normal();
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

    Board::copy_smart(board, state.board, true);

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
