#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/scenes/standard_game_scene.h"
#include "game/scenes/imgui_layer.h"
#include "game/game_options.h"
#include "game/save_load.h"
#include "game/assets.h"
#include "game/constants.h"
#include "game/piece.h"
#include "game/node.h"
#include "other/data.h"
#include "options/options.h"

using namespace encrypt;

void StandardGameScene::on_start() {
    auto& data = app->user_data<Data>();

    imgui_layer.update();

    undo_redo_state = UndoRedoState {};

    setup_entities();

    setup_and_add_model_board();
    setup_and_add_model_board_paint();
    setup_and_add_model_pieces();
    setup_and_add_model_nodes();

    setup_camera();
    setup_widgets();

    keyboard = KeyboardControls {&board, data.quad_cache["keyboard_controls"_h]};
    keyboard.initialize_refs();

    minimax_thread = MinimaxThread {&board};

    game = GameContext {
        static_cast<GamePlayer>(data.options.white_player),
        static_cast<GamePlayer>(data.options.black_player),
        &board,
        &minimax_thread
    };

    board.undo_redo_state = &undo_redo_state;
    board.keyboard = &keyboard;
    board.game_context = &game;

    app->window->set_cursor(data.options.custom_cursor ? data.arrow_cursor : 0);

#ifdef PLATFORM_GAME_DEBUG
    app->renderer->origin = true;
    app->renderer->add_quad(data.quad_cache["light_bulb"_h]);
#endif

    timer = Timer {};

    camera.go_towards_position(default_camera_position);
}

void StandardGameScene::on_stop() {
    auto& data = app->user_data<Data>();

    imgui_layer.reset();

    // Save game options
    using namespace game_options;

    try {
        options::save_options_to_file<GameOptions>(data.options, GAME_OPTIONS_FILE);  // TODO put this into a function
    } catch (const options::OptionsFileNotOpenError& e) {
        REL_ERROR("{}", e.what());

        options::handle_options_file_not_open_error<GameOptions>(
            GAME_OPTIONS_FILE, app->data().application_name
        );
    } catch (const options::OptionsFileError& e) {
        REL_ERROR("{}", e.what());

        try {
            options::create_options_file<GameOptions>(GAME_OPTIONS_FILE);
        } catch (const options::OptionsFileNotOpenError& e) {
            REL_ERROR("{}", e.what());
        } catch (const options::OptionsFileError& e) {
            REL_ERROR("{}", e.what());
        }
    }

    // Save this game
    if (data.options.save_on_exit && !app->running && made_first_move) {
        save_game();
    }

#ifdef PLATFORM_GAME_DEBUG
    app->renderer->origin = false;
#endif

    app->renderer->clear();
    app->gui_renderer->clear();

    made_first_move = false;

    // TODO stop timer here?

    if (loader != nullptr) {
        if (loader->joinable()) {
            loader->join();
        }
    }
}

void StandardGameScene::on_awake() {
    imgui_layer = ImGuiLayer<StandardGameScene> {app, this};

    initialize_rendering_board();
    initialize_rendering_board_paint();
    initialize_rendering_pieces();
    initialize_rendering_nodes();

    initialize_rendering_keyboard_controls();
#ifdef PLATFORM_GAME_DEBUG
    initialize_rendering_light_bulb();
#endif

    setup_skybox();
    setup_light();
    setup_indicators_textures();

    app->evt.sink<MouseButtonPressedEvent>().connect<&StandardGameScene::on_mouse_button_pressed>(*this);
    app->evt.sink<MouseButtonReleasedEvent>().connect<&StandardGameScene::on_mouse_button_released>(*this);
    app->evt.sink<KeyPressedEvent>().connect<&StandardGameScene::on_key_pressed>(*this);
    app->evt.sink<KeyReleasedEvent>().connect<&StandardGameScene::on_key_released>(*this);
    app->evt.sink<WindowResizedEvent>().connect<&StandardGameScene::on_window_resized>(*this);

    // It's ok to be called multiple times
    LOG_TOTAL_GPU_MEMORY_ALLOCATED()
}

void StandardGameScene::on_update() {
    if (!imgui_layer.hovering_gui) {
        camera.update(app->get_mouse_wheel(), app->get_dx(), app->get_dy(), app->get_delta());
    }

    board.update_nodes(app->renderer->get_hovered_id());
    board.update_pieces(app->renderer->get_hovered_id());

    timer.update(app->window->get_time());

    char time[32];
    timer.get_time_formatted(time);
    timer_text->set_text(time);
}

void StandardGameScene::on_fixed_update() {
    if (!imgui_layer.hovering_gui) {
        camera.update_friction();
    }
}

void StandardGameScene::on_imgui_update() {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(app->data().width, app->data().height);
    io.DeltaTime = app->get_delta();

    imgui_layer.draw_menu_bar();

    if (imgui_layer.show_about) {
        imgui_layer.draw_about();
    } else if (imgui_layer.show_could_not_load_game) {
        imgui_layer.draw_could_not_load_game();
    } else if (imgui_layer.show_no_last_game) {
        imgui_layer.draw_no_last_game();
    } else if (board.phase == BoardPhase::GameOver) {
        imgui_layer.draw_game_over();
    }

    if (imgui_layer.show_info && !imgui_layer.show_about) {
        imgui_layer.draw_info();
    }

#ifdef PLATFORM_GAME_DEBUG
    imgui_layer.draw_debug();
#endif

    if (undo_redo_state.undo.size() > 0) {  // TODO this can be better
        imgui_layer.can_undo = true;
    }

    if (undo_redo_state.redo.size() > 0) {
        imgui_layer.can_redo = true;
    }
}

void StandardGameScene::on_mouse_button_pressed(const MouseButtonPressedEvent& event) {
    if (event.button == input::MouseButton::LEFT) {
        if (board.next_move) {
            board.click(app->renderer->get_hovered_id());
        }
    }
}

void StandardGameScene::on_mouse_button_released(const MouseButtonReleasedEvent& event) {
    auto& data = app->user_data<Data>();

    if (event.button == input::MouseButton::LEFT) {
        if (board.next_move && board.is_players_turn) {
            const auto [did_action, switched_turn, must_take_piece_or_took_piece] = board.release();

            if (did_action) {  // FIXME here is some repetition
                game.state = GameState::HumanDoingMove;
            }

            if (did_action && !made_first_move && !timer.is_running()) {
                timer.start(app->window->get_time());
                made_first_move = true;
            }

            if (board.phase == BoardPhase::GameOver) {
                timer.stop();
            }

            if (switched_turn) {
                update_turn_indicator();
            }

            if (must_take_piece_or_took_piece) {
                update_cursor();
            }

            if (undo_redo_state.redo.empty()) {
                imgui_layer.can_redo = false;
            }
        }

        if (show_keyboard_controls) {
            app->renderer->remove_quad(data.quad_cache["keyboard_controls"_h]);
            show_keyboard_controls = false;
        }
    }
}

void StandardGameScene::on_key_pressed(const KeyPressedEvent& event) {
    auto& data = app->user_data<Data>();

    switch (event.key) {
        case input::Key::UP:
        case input::Key::DOWN:
        case input::Key::LEFT:
        case input::Key::RIGHT:
        case input::Key::ENTER:
            if (!show_keyboard_controls) {
                app->renderer->add_quad(data.quad_cache["keyboard_controls"_h]);
                show_keyboard_controls = true;
                return;
            }
        default:
            break;
    }

    using KB = KeyboardControls;

    switch (event.key) {
        case input::Key::UP:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Up, camera.get_angle_around_point()
                )
            );
            break;
        case input::Key::DOWN:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Down, camera.get_angle_around_point()
                )
            );
            break;
        case input::Key::LEFT:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Left, camera.get_angle_around_point()
                )
            );
            break;
        case input::Key::RIGHT:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Right, camera.get_angle_around_point()
                )
            );
            break;
        case input::Key::ENTER:
            if (board.next_move && board.is_players_turn) {
                const auto [did_action, switched_turn, must_take_piece_or_took_piece] = keyboard.click_and_release();

                if (did_action) {
                    game.state = GameState::HumanDoingMove;
                }

                if (did_action && !made_first_move && !timer.is_running()) {
                    timer.start(app->window->get_time());
                    made_first_move = true;
                }

                if (board.phase == BoardPhase::GameOver) {
                    timer.stop();
                }

                if (switched_turn) {
                    update_turn_indicator();
                }

                if (must_take_piece_or_took_piece) {
                    update_cursor();
                }

                if (undo_redo_state.redo.empty()) {
                    imgui_layer.can_redo = false;
                }
            }
            break;
        default:
            break;
    }
}

void StandardGameScene::on_key_released(const KeyReleasedEvent& event) {
    if (event.key == input::Key::SPACE) {
        camera.go_towards_position(default_camera_position);
    }
}

void StandardGameScene::on_window_resized(const WindowResizedEvent& event) {
    camera.update_projection(static_cast<float>(event.width), static_cast<float>(event.height));
}

std::shared_ptr<Buffer> StandardGameScene::create_id_buffer(size_t vertices_size, hover::Id id, hs hash) {
    std::vector<int> array;
    array.resize(vertices_size);

    for (size_t i = 0; i < array.size(); i++) {
        array[i] = static_cast<int>(id);
    }

    return app->res.buffer.load(hash, array.data(), array.size() * sizeof(int));
}

void StandardGameScene::initialize_rendering_board() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_wood_shader"_h,
        encr(paths::path_for_assets(assets::BOARD_VERTEX_SHADER)),
        encr(paths::path_for_assets(assets::BOARD_FRAGMENT_SHADER)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
            "u_material.normal"
        },
        std::vector {
            app->renderer->get_projection_view_uniform_block(),
            app->renderer->get_light_uniform_block(),
            app->renderer->get_light_view_position_uniform_block(),
            app->renderer->get_light_space_uniform_block()
        }
    );
    app->renderer->setup_shader(shader);

    auto buffer = app->res.buffer.load(
        "board_wood_buffer"_h,
        app->res.mesh_ptnt["board_wood_mesh"_h]->vertices.data(),
        app->res.mesh_ptnt["board_wood_mesh"_h]->vertices.size() * sizeof(PTNT)
    );

    auto index_buffer = app->res.index_buffer.load(
        "board_wood_index_buffer"_h,
        app->res.mesh_ptnt["board_wood_mesh"_h]->indices.data(),
        app->res.mesh_ptnt["board_wood_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);
    layout.add(3, BufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("board_wood_vertex_array"_h);
    vertex_array->add_buffer(buffer, layout);
    vertex_array->add_index_buffer(index_buffer);
    VertexArray::unbind();

    TextureSpecification specification;
    specification.mag_filter = Filter::Linear;
    specification.mipmapping = true;
    specification.bias = -2.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.load(
        "board_wood_diffuse_texture"_h,
        app->res.texture_data["board_wood_diffuse_texture"_h],
        specification
    );

    auto normal_texture = app->res.texture.load(
        "board_normal_texture"_h,
        app->res.texture_data["board_normal_texture"_h],
        specification
    );

    auto material = app->res.material.load("wood_material"_h, shader);
    material->add_texture("u_material.diffuse");
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular");
    material->add_uniform(Material::Uniform::Float, "u_material.shininess");
    material->add_texture("u_material.normal");

    auto material_instance = app->res.material_instance.load("board_wood_material_instance"_h, material);
    material_instance->set_texture("u_material.diffuse", diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess", 4.0f);
    material_instance->set_texture("u_material.normal", normal_texture, 1);
}

void StandardGameScene::initialize_rendering_board_paint() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_paint_shader"_h,
        encr(paths::path_for_assets(assets::BOARD_PAINT_VERTEX_SHADER)),
        encr(paths::path_for_assets(assets::BOARD_PAINT_FRAGMENT_SHADER)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
            "u_material.normal"
        },
        std::vector {
            app->renderer->get_projection_view_uniform_block(),
            app->renderer->get_light_uniform_block(),
            app->renderer->get_light_view_position_uniform_block(),
            app->renderer->get_light_space_uniform_block()
        }
    );
    app->renderer->setup_shader(shader);

    auto buffer = app->res.buffer.load(
        "board_paint_buffer"_h,
        app->res.mesh_ptnt["board_paint_mesh"_h]->vertices.data(),
        app->res.mesh_ptnt["board_paint_mesh"_h]->vertices.size() * sizeof(PTNT)
    );

    auto index_buffer = app->res.index_buffer.load(
        "board_paint_index_buffer"_h,
        app->res.mesh_ptnt["board_paint_mesh"_h]->indices.data(),
        app->res.mesh_ptnt["board_paint_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);
    layout.add(3, BufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("board_paint_vertex_array"_h);
    vertex_array->add_buffer(buffer, layout);
    vertex_array->add_index_buffer(index_buffer);
    VertexArray::unbind();

    TextureSpecification specification;
    specification.mag_filter = Filter::Linear;
    specification.mipmapping = true;
    specification.bias = -1.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.load(
        "board_paint_diffuse_texture"_h,
        app->res.texture_data["board_paint_diffuse_texture"_h],
        specification
    );

    auto material = app->res.material.load("board_paint_material"_h, shader);
    material->add_texture("u_material.diffuse");
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular");
    material->add_uniform(Material::Uniform::Float, "u_material.shininess");
    material->add_texture("u_material.normal");

    auto material_instance = app->res.material_instance.load("board_paint_material_instance"_h, material);
    material_instance->set_texture("u_material.diffuse", diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess", 4.0f);
    material_instance->set_texture("u_material.normal", app->res.texture["board_normal_texture"_h], 1);
}

void StandardGameScene::initialize_rendering_pieces() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "piece_shader"_h,
        encr(paths::path_for_assets(assets::PIECE_VERTEX_SHADER)),
        encr(paths::path_for_assets(assets::PIECE_FRAGMENT_SHADER)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
            "u_material.normal",
            "u_material.tint"
        },
        std::vector {
            app->renderer->get_projection_view_uniform_block(),
            app->renderer->get_light_uniform_block(),
            app->renderer->get_light_view_position_uniform_block(),
            app->renderer->get_light_space_uniform_block()
        }
    );
    app->renderer->setup_shader(shader);

    TextureSpecification specification;
    specification.mag_filter = Filter::Linear;
    specification.mipmapping = true;
    specification.bias = -1.5f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto white_piece_diffuse_texture = app->res.texture.load(
        "white_piece_diffuse_texture"_h,
        app->res.texture_data["white_piece_diffuse_texture"_h],
        specification
    );

    auto black_piece_diffuse_texture = app->res.texture.load(
        "black_piece_diffuse_texture"_h,
        app->res.texture_data["black_piece_diffuse_texture"_h],
        specification
    );

    auto piece_normal_texture = app->res.texture.load(
        "piece_normal_texture"_h,
        app->res.texture_data["piece_normal_texture"_h],
        specification
    );

    auto material = app->res.material.load("tinted_wood_material"_h, shader);
    material->add_texture("u_material.diffuse");
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular");
    material->add_uniform(Material::Uniform::Float, "u_material.shininess");
    material->add_texture("u_material.normal");
    material->add_uniform(Material::Uniform::Vec3, "u_material.tint");

    auto white_piece_buffer = app->res.buffer.load(
        "white_piece_buffer"_h,
        app->res.mesh_ptnt["white_piece_mesh"_h]->vertices.data(),
        app->res.mesh_ptnt["white_piece_mesh"_h]->vertices.size() * sizeof(PTNT)
    );

    auto white_piece_index_buffer = app->res.index_buffer.load(
        "white_piece_index_buffer"_h,
        app->res.mesh_ptnt["white_piece_mesh"_h]->indices.data(),
        app->res.mesh_ptnt["white_piece_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    auto black_piece_buffer = app->res.buffer.load(
        "black_piece_buffer"_h,
        app->res.mesh_ptnt["black_piece_mesh"_h]->vertices.data(),
        app->res.mesh_ptnt["black_piece_mesh"_h]->vertices.size() * sizeof(PTNT)
    );

    auto black_piece_index_buffer = app->res.index_buffer.load(
        "black_piece_index_buffer"_h,
        app->res.mesh_ptnt["black_piece_mesh"_h]->indices.data(),
        app->res.mesh_ptnt["black_piece_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    for (size_t i = 0; i < 9; i++) {
        initialize_rendering_piece(
            i, app->res.mesh_ptnt["white_piece_mesh"_h],
            white_piece_diffuse_texture, white_piece_buffer, white_piece_index_buffer
        );
    }

    for (size_t i = 9; i < 18; i++) {
        initialize_rendering_piece(
            i, app->res.mesh_ptnt["black_piece_mesh"_h],
            black_piece_diffuse_texture, black_piece_buffer, black_piece_index_buffer
        );
    }
}

void StandardGameScene::initialize_rendering_piece(
        size_t index,
        std::shared_ptr<Mesh<PTNT>> mesh,
        std::shared_ptr<Texture> diffuse_texture,
        std::shared_ptr<Buffer> buffer,
        std::shared_ptr<IndexBuffer> index_buffer) {
    auto& data = app->user_data<Data>();

    const hover::Id id = hover::generate_id();
    data.piece_ids[index] = id;

    auto id_buffer = create_id_buffer(
        mesh->vertices.size(), id,
        hs {"piece_id_buffer" + std::to_string(index)}
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);
    layout.add(3, BufferLayout::Float, 3);

    BufferLayout layout2;
    layout2.add(4, BufferLayout::Int, 1);

    auto vertex_array = app->res.vertex_array.load(hs {"piece_vertex_array" + std::to_string(index)});
    vertex_array->add_buffer(buffer, layout);
    vertex_array->add_buffer(id_buffer, layout2);
    vertex_array->add_index_buffer(index_buffer);
    VertexArray::unbind();

    auto material_instance = app->res.material_instance.load(
        hs {"piece_material_instance" + std::to_string(index)},
        app->res.material["tinted_wood_material"_h]
    );
    material_instance->set_texture("u_material.diffuse", diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess", 4.0f);
    material_instance->set_texture("u_material.normal", app->res.texture["piece_normal_texture"_h], 1);
    material_instance->set_vec3("u_material.tint", glm::vec3(1.0f));
}

void StandardGameScene::initialize_rendering_nodes() {
    auto shader = app->res.shader.load(
        "node_shader"_h,
        encr(paths::path_for_assets(assets::NODE_VERTEX_SHADER)),
        encr(paths::path_for_assets(assets::NODE_FRAGMENT_SHADER)),
        std::vector<std::string> { "u_model_matrix", "u_color" },
        std::vector {
            app->renderer->get_projection_view_uniform_block()
        }
    );
    app->renderer->setup_shader(shader);

    auto material = app->res.material.load("basic_material"_h, shader);
    material->add_uniform(Material::Uniform::Vec4, "u_color");

    auto buffer = app->res.buffer.load(
        "node_buffer"_h,
        app->res.mesh_p["node_mesh"_h]->vertices.data(),
        app->res.mesh_p["node_mesh"_h]->vertices.size() * sizeof(P)
    );

    auto index_buffer = app->res.index_buffer.load(
        "node_index_buffer"_h,
        app->res.mesh_p["node_mesh"_h]->indices.data(),
        app->res.mesh_p["node_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    for (size_t i = 0; i < 24; i++) {
        initialize_rendering_node(i, buffer, index_buffer);
    }
}

void StandardGameScene::initialize_rendering_node(size_t index, std::shared_ptr<Buffer> buffer,
        std::shared_ptr<IndexBuffer> index_buffer) {
    auto& data = app->user_data<Data>();

    const hover::Id id = hover::generate_id();
    data.node_ids[index] = id;

    auto id_buffer = create_id_buffer(
        app->res.mesh_p["node_mesh"_h]->vertices.size(), id,
        hs {"node_id_buffer" + std::to_string(index)}
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);

    BufferLayout layout2;
    layout2.add(1, BufferLayout::Int, 1);

    auto vertex_array = app->res.vertex_array.load(hs {"node_vertex_array" + std::to_string(index)});
    vertex_array->add_buffer(buffer, layout);
    vertex_array->add_buffer(id_buffer, layout2);
    vertex_array->add_index_buffer(index_buffer);
    VertexArray::unbind();

    auto material_instance = app->res.material_instance.load(
        hs {"node_material_instance" + std::to_string(index)},
        app->res.material["basic_material"_h]
    );
    material_instance->set_vec4("u_color", glm::vec4(0.0f));
}

void StandardGameScene::initialize_rendering_keyboard_controls() {
    auto& data = app->user_data<Data>();

    auto keyboard_controls = data.quad_cache.load("keyboard_controls"_h);

    TextureSpecification specification;
    specification.min_filter = Filter::Linear;
    specification.mag_filter = Filter::Linear;

    auto texture = app->res.texture.load(
        "keyboard_controls_texture"_h,
        app->res.texture_data["keyboard_controls_texture"_h],
        specification
    );
    app->res.texture.load(
        "keyboard_controls_cross_texture"_h,
        app->res.texture_data["keyboard_controls_cross_texture"_h],
        specification
    );

    keyboard_controls->texture = texture;
}

void StandardGameScene::initialize_rendering_light_bulb() {
    auto& data = app->user_data<Data>();

    auto light_bulb = data.quad_cache.load("light_bulb"_h);

    TextureSpecification specification;
    specification.min_filter = Filter::Linear;
    specification.mag_filter = Filter::Linear;

    auto light_bulb_texture = app->res.texture.load(
        "light_bulb_texture"_h,
        "data/textures/internal/light_bulb/light_bulb.png",
        specification
    );

    light_bulb->texture = light_bulb_texture;
}

void StandardGameScene::setup_and_add_model_board() {
    board.model->vertex_array = app->res.vertex_array["board_wood_vertex_array"_h];
    board.model->index_buffer = app->res.index_buffer["board_wood_index_buffer"_h];
    board.model->scale = WORLD_SCALE;
    board.model->material = app->res.material_instance["board_wood_material_instance"_h];
    board.model->cast_shadow = true;

    app->renderer->add_model(board.model);

    DEB_DEBUG("Setup model board");
}

void StandardGameScene::setup_and_add_model_board_paint() {
    board.paint_model->vertex_array = app->res.vertex_array["board_paint_vertex_array"_h];
    board.paint_model->index_buffer = app->res.index_buffer["board_paint_index_buffer"_h];
    board.paint_model->position = glm::vec3(0.0f, PAINT_Y_POSITION, 0.0f);
    board.paint_model->scale = WORLD_SCALE;
    board.paint_model->material = app->res.material_instance["board_paint_material_instance"_h];

    app->renderer->add_model(board.paint_model);

    DEB_DEBUG("Setup model board paint");
}

void StandardGameScene::setup_and_add_model_pieces() {
    for (size_t i = 0; i < 9; i++) {
        setup_and_add_model_piece(
            i, WHITE_PIECE_POSITION(i),
            app->res.index_buffer["white_piece_index_buffer"_h]
        );
    }

    for (size_t i = 9; i < 18; i++) {
        setup_and_add_model_piece(
            i, BLACK_PIECE_POSITION(i),
            app->res.index_buffer["black_piece_index_buffer"_h]
        );
    }
}

void StandardGameScene::setup_and_add_model_piece(size_t index, const glm::vec3& position, std::shared_ptr<IndexBuffer> index_buffer) {
    auto& data = app->user_data<Data>();

    Piece& piece = board.pieces[index];

    piece.model->position = position;
    piece.model->rotation = RANDOM_PIECE_ROTATION();
    piece.model->vertex_array = app->res.vertex_array[hs {"piece_vertex_array" + std::to_string(index)}];
    piece.model->index_buffer = index_buffer;
    piece.model->scale = WORLD_SCALE;
    piece.model->material = app->res.material_instance[hs {"piece_material_instance" + std::to_string(index)}];
    piece.model->outline_color = std::make_optional<glm::vec3>(1.0f);
    piece.model->id = std::make_optional<hover::Id>(data.piece_ids[index]);
    piece.model->cast_shadow = true;

    app->renderer->add_model(piece.model);

    DEB_DEBUG("Setup model piece {}", index);
}

void StandardGameScene::setup_and_add_model_nodes() {
    for (size_t i = 0; i < 24; i++) {
        setup_and_add_model_node(i, NODE_POSITIONS[i]);
    }
}

void StandardGameScene::setup_and_add_model_node(size_t index, const glm::vec3& position) {
    auto& data = app->user_data<Data>();

    Node& node = board.nodes[index];

    node.model->vertex_array = app->res.vertex_array[hs {"node_vertex_array" + std::to_string(index)}];
    node.model->index_buffer = app->res.index_buffer["node_index_buffer"_h];
    node.model->position = position;
    node.model->scale = WORLD_SCALE;
    node.model->material = app->res.material_instance[hs {"node_material_instance" + std::to_string(index)}];
    node.model->id = std::make_optional<hover::Id>(data.node_ids[index]);

    app->renderer->add_model(node.model);

    DEB_DEBUG("Setup model node {}", index);
}

void StandardGameScene::setup_entities() {
    auto& data = app->user_data<Data>();

    board = StandardBoard {app};
    board.model = data.model_cache.load("board"_h);
    board.paint_model = data.model_cache.load("board_paint"_h);

    for (size_t i = 0; i < 9; i++) {
        board.pieces[i] = Piece {i, PieceType::White, data.model_cache.load(hs {"piece" + std::to_string(i)})};
    }

    for (size_t i = 9; i < 18; i++) {
        board.pieces[i] = Piece {i, PieceType::Black, data.model_cache.load(hs {"piece" + std::to_string(i)})};
    }

    for (size_t i = 0; i < 24; i++) {
        board.nodes[i] = Node {i, data.model_cache.load(hs {"node" + std::to_string(i)})};
    }

    DEB_DEBUG("Setup entities");
}

void StandardGameScene::setup_entity_ids() {
    auto& data = app->user_data<Data>();

    for (size_t i = 0; i < 9; i++) {
        board.pieces[i].model->id = data.piece_ids[i];  // FIXME think this through
    }
    for (size_t i = 9; i < 18; i++) {
        board.pieces[i].model->id = data.piece_ids[i];  // FIXME this
    }
    for (size_t i = 0; i < 24; i++) {
        board.nodes[i].model->id = data.node_ids[i];  // FIXME this
    }

    DEB_DEBUG("Setup board's entities' IDs");
}

void StandardGameScene::setup_skybox() {
    std::array<std::shared_ptr<TextureData>, 6> data = {
        app->res.texture_data["skybox_px_texture"_h],
        app->res.texture_data["skybox_nx_texture"_h],
        app->res.texture_data["skybox_py_texture"_h],
        app->res.texture_data["skybox_ny_texture"_h],
        app->res.texture_data["skybox_pz_texture"_h],
        app->res.texture_data["skybox_nz_texture"_h]
    };

    auto texture = app->res.texture_3d.force_load("skybox_texture"_h, data);
    app->renderer->set_skybox(texture);

    DEB_DEBUG("Setup skybox");
}

void StandardGameScene::setup_light() {
    auto& data = app->user_data<Data>();

    if (data.options.skybox == game_options::FIELD) {
        app->renderer->light = LIGHT_FIELD;
        app->renderer->light_space = SHADOWS_FIELD;
    } else if (data.options.skybox == game_options::AUTUMN) {
        app->renderer->light = LIGHT_AUTUMN;
        app->renderer->light_space = SHADOWS_AUTUMN;
    } else {
        ASSERT(false, "Invalid skybox");
    }

#ifdef PLATFORM_GAME_DEBUG
    if (data.options.skybox == game_options::FIELD) {
        data.quad_cache["light_bulb"_h]->position = LIGHT_FIELD.position;
    } else if (data.options.skybox == game_options::AUTUMN) {
        data.quad_cache["light_bulb"_h]->position = LIGHT_AUTUMN.position;
    }
#endif

    DEB_DEBUG("Setup light");
}

void StandardGameScene::setup_camera() {
    auto& data = app->user_data<Data>();

    const glm::mat4 projection = glm::perspective(
        glm::radians(FOV),
        static_cast<float>(app->data().width) / app->data().height,
        NEAR, FAR
    );

    camera = Camera {
        data.options.sensitivity,
        47.0f,
        glm::vec3(0.0f),
        8.0f,
        projection
    };

    default_camera_position = camera.get_position();

    camera = Camera {
        data.options.sensitivity,
        47.0f,
        glm::vec3(0.0f),
        8.7f,
        projection
    };

    app->renderer->set_camera(&camera);

    DEB_DEBUG("Setup camera");
}

void StandardGameScene::setup_indicators_textures() {
    TextureSpecification specification;
    specification.min_filter = Filter::Linear;
    specification.mag_filter = Filter::Linear;

    app->res.texture.load(
        "white_indicator_texture"_h,
        app->res.texture_data["white_indicator_texture"_h],
        specification
    );
    app->res.texture.load(
        "black_indicator_texture"_h,
        app->res.texture_data["black_indicator_texture"_h],
        specification
    );
    app->res.texture.load(
        "wait_indicator_texture"_h,
        app->res.texture_data["wait_indicator_texture"_h],
        specification
    );
    app->res.texture.load(
        "computer_thinking_indicator_texture"_h,
        app->res.texture_data["computer_thinking_indicator_texture"_h],
        specification
    );
}

void StandardGameScene::setup_widgets() {
    auto& data = app->user_data<Data>();

    constexpr int LOWEST_RESOLUTION = 288;
    constexpr int HIGHEST_RESOLUTION = 1035;

    turn_indicator = std::make_shared<gui::Image>(app->res.texture["white_indicator_texture"_h]);
    turn_indicator->stick(gui::Sticky::SE);
    turn_indicator->offset(30, gui::Relative::Right)->offset(30, gui::Relative::Bottom);
    turn_indicator->scale(0.4f, 1.0f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);
    app->gui_renderer->add_widget(turn_indicator);

    timer_text = std::make_shared<gui::Text>(app->res.font["good_dog_plain_font"_h], "00:00", 1.5f, glm::vec3(0.9f));
    timer_text->stick(gui::Sticky::N);
    timer_text->offset(60, gui::Relative::Top);
    timer_text->scale(0.6f, 1.4f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);
    timer_text->set_shadows(true);

    if (!data.options.hide_timer) {
        app->gui_renderer->add_widget(timer_text);
    }

    wait_indicator = std::make_shared<gui::Image>(app->res.texture["wait_indicator_texture"_h]);
    wait_indicator->stick(gui::Sticky::NE);
    wait_indicator->offset(25, gui::Relative::Right)->offset(55, gui::Relative::Top);
    wait_indicator->scale(0.4f, 1.0f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);

    computer_thinking_indicator = std::make_shared<gui::Image>(app->res.texture["computer_thinking_indicator_texture"_h]);
    computer_thinking_indicator->stick(gui::Sticky::NE);
    computer_thinking_indicator->offset(25, gui::Relative::Right)->offset(55, gui::Relative::Top);
    computer_thinking_indicator->scale(0.4f, 1.0f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);
}

void StandardGameScene::update_turn_indicator() {
    if (board.turn == BoardPlayer::White) {
        turn_indicator->set_image(app->res.texture["white_indicator_texture"_h]);
    } else {
        turn_indicator->set_image(app->res.texture["black_indicator_texture"_h]);
    }
}

void StandardGameScene::update_cursor() {
    auto& data = app->user_data<Data>();

    if (data.options.custom_cursor) {
        if (board.must_take_piece) {
            app->window->set_cursor(data.cross_cursor);

            data.quad_cache["keyboard_controls"_h]->texture = app->res.texture["keyboard_controls_cross_texture"_h];
        } else {
            app->window->set_cursor(data.arrow_cursor);

            data.quad_cache["keyboard_controls"_h]->texture = app->res.texture["keyboard_controls_texture"_h];
        }
    }
}

void StandardGameScene::save_game() {
    board.finalize_pieces_state();

    save_load::SavedGame saved_game;
    saved_game.board = board;
    saved_game.camera = camera;
    saved_game.time = timer.get_time_raw();

    time_t current;
    time(&current);
    saved_game.date = ctime(&current);

    saved_game.undo_redo_state = undo_redo_state;  // TODO think if it's alright
    saved_game.white_player = game.white_player;
    saved_game.black_player = game.black_player;

    try {
        save_load::save_game_to_file(saved_game);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_ERROR("{}", e.what());

        save_load::handle_save_file_not_open_error(app->data().application_name);

        REL_ERROR("Could not save game");
    } catch (const save_load::SaveFileError& e) {
        REL_ERROR("{}", e.what());
        REL_ERROR("Could not save game");
    }
}
