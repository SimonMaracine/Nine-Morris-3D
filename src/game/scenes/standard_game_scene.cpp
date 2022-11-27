#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/scenes/standard_game_scene.h"
#include "game/scenes/imgui_layer.h"
#include "game/game_options.h"
#include "game/save_load.h"
#include "game/assets.h"
#include "game/entities/boards/standard_board.h"
#include "game/entities/serialization/standard_board_serialized.h"
#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "other/constants.h"
#include "other/data.h"
#include "other/options.h"
#include "other/save_load_gracefully.h"

using namespace encrypt;

void StandardGameScene::on_start() {
    auto& data = app->user_data<Data>();

    imgui_layer.update();

    setup_entities();

    setup_and_add_model_board();
    setup_and_add_model_board_paint();
    setup_and_add_model_pieces();
    setup_and_add_model_nodes();

    setup_camera();
    setup_widgets();

    keyboard = KeyboardControls {app, &board, data.quad_cache["keyboard_controls"_h]};
    keyboard.initialize_refs();

    undo_redo_state = UndoRedoState<StandardBoardSerialized> {};

    minimax_thread = MinimaxThread {&board};

    game = GameContext {
        static_cast<GamePlayer>(data.options.white_player),
        static_cast<GamePlayer>(data.options.black_player),
        &board,
        &minimax_thread
    };

    timer = Timer {app};

    board.app = app;
    board.keyboard = &keyboard;
    board.camera = &camera;
    board.undo_redo_state = &undo_redo_state;

    app->window->set_cursor(data.options.custom_cursor ? data.arrow_cursor : 0);

#ifdef PLATFORM_GAME_DEBUG
    app->renderer->origin = true;
    app->renderer->add_quad(data.quad_cache["light_bulb"_h]);
#endif

    camera.go_towards_position(default_camera_position);
}

void StandardGameScene::on_stop() {
    auto& data = app->user_data<Data>();

    imgui_layer.reset();

    save_load_gracefully::save_to_file<game_options::GameOptions>(
        game_options::GAME_OPTIONS_FILE, data.options, app
    );

    if (data.options.save_on_exit && !app->running && made_first_move) {
        save_game();
    }

#ifdef PLATFORM_GAME_DEBUG
    app->renderer->origin = false;
#endif

    app->renderer->clear();
    app->gui_renderer->clear();

    made_first_move = false;

    if (skybox_loader != nullptr) {
        if (skybox_loader->joinable()) {
            skybox_loader->join();
        }
    }
}

void StandardGameScene::on_awake() {
    auto& data = app->user_data<Data>();

    imgui_layer = ImGuiLayer<StandardGameScene, StandardBoardSerialized> {app, this};

    if (data.launcher_options.normal_mapping) {
        initialize_rendering_board();
        initialize_rendering_board_paint();
        initialize_rendering_pieces();
    } else {
        initialize_rendering_board_no_normal();
        initialize_rendering_board_paint_no_normal();
        initialize_rendering_pieces_no_normal();
    }
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
}

void StandardGameScene::on_update() {
    if (!imgui_layer.hovering_gui) {
        camera.update(app->get_mouse_wheel(), app->get_dx(), app->get_dy(), app->get_delta());
    }

    board.update_nodes(app->renderer->get_hovered_id());
    board.update_pieces(app->renderer->get_hovered_id());

    update_game_state();

    timer.update();

    update_timer_text();
    update_wait_indicator();
    update_computer_thinking_indicator();

    check_skybox_loader();
    check_board_paint_texture_loader();
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
        if (board.next_move && board.is_players_turn
                && (board.phase == BoardPhase::PlacePieces || board.phase == BoardPhase::MovePieces)) {
            const auto [did_action, switched_turn, must_take_piece_or_took_piece] = (
                board.release(app->renderer->get_hovered_id())
            );

            update_after_human_move(did_action, switched_turn, must_take_piece_or_took_piece);
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
            if (board.next_move && board.is_players_turn
                    && (board.phase == BoardPhase::PlacePieces || board.phase == BoardPhase::MovePieces)) {
                const auto [did_action, switched_turn, must_take_piece_or_took_piece] = (
                    keyboard.click_and_release()
                );

                update_after_human_move(did_action, switched_turn, must_take_piece_or_took_piece);
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

std::shared_ptr<Buffer> StandardGameScene::create_id_buffer(size_t vertices_size, identifier::Id id, hs hash) {
    using IdType = float;

    std::vector<IdType> array;
    array.resize(vertices_size);

    for (size_t i = 0; i < array.size(); i++) {
        array[i] = id;
    }

    return app->res.buffer.load(hash, array.data(), array.size() * sizeof(IdType));
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

    auto vertex_buffer = app->res.buffer.load(
        "board_wood_vertex_buffer"_h,
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
    vertex_array->add_buffer(vertex_buffer, layout);
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

    auto vertex_buffer = app->res.buffer.load(
        "board_paint_vertex_buffer"_h,
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
    vertex_array->add_buffer(vertex_buffer, layout);
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

    auto white_piece_vertex_buffer = app->res.buffer.load(
        "white_piece_vertex_buffer"_h,
        app->res.mesh_ptnt["white_piece_mesh"_h]->vertices.data(),
        app->res.mesh_ptnt["white_piece_mesh"_h]->vertices.size() * sizeof(PTNT)
    );

    auto white_piece_index_buffer = app->res.index_buffer.load(
        "white_piece_index_buffer"_h,
        app->res.mesh_ptnt["white_piece_mesh"_h]->indices.data(),
        app->res.mesh_ptnt["white_piece_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    auto black_piece_vertex_buffer = app->res.buffer.load(
        "black_piece_vertex_buffer"_h,
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
            white_piece_diffuse_texture, white_piece_vertex_buffer, white_piece_index_buffer
        );
    }

    for (size_t i = 9; i < 18; i++) {
        initialize_rendering_piece(
            i, app->res.mesh_ptnt["black_piece_mesh"_h],
            black_piece_diffuse_texture, black_piece_vertex_buffer, black_piece_index_buffer
        );
    }
}

void StandardGameScene::initialize_rendering_piece(
        size_t index,
        std::shared_ptr<Mesh<PTNT>> mesh,
        std::shared_ptr<Texture> diffuse_texture,
        std::shared_ptr<Buffer> vertex_buffer,
        std::shared_ptr<IndexBuffer> index_buffer) {
    auto& data = app->user_data<Data>();

    const identifier::Id id = identifier::generate_id();
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
    layout2.add(4, BufferLayout::Float, 1);

    auto vertex_array = app->res.vertex_array.load(hs {"piece_vertex_array" + std::to_string(index)});
    vertex_array->add_buffer(vertex_buffer, layout);
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
    material_instance->set_vec3("u_material.tint", DEFAULT_TINT);
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

    auto vertex_buffer = app->res.buffer.load(
        "node_vertex_buffer"_h,
        app->res.mesh_p["node_mesh"_h]->vertices.data(),
        app->res.mesh_p["node_mesh"_h]->vertices.size() * sizeof(P)
    );

    auto index_buffer = app->res.index_buffer.load(
        "node_index_buffer"_h,
        app->res.mesh_p["node_mesh"_h]->indices.data(),
        app->res.mesh_p["node_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    for (size_t i = 0; i < 24; i++) {
        initialize_rendering_node(i, vertex_buffer, index_buffer);
    }
}

void StandardGameScene::initialize_rendering_node(size_t index, std::shared_ptr<Buffer> vertex_buffer,
        std::shared_ptr<IndexBuffer> index_buffer) {
    auto& data = app->user_data<Data>();

    const identifier::Id id = identifier::generate_id();
    data.node_ids[index] = id;

    auto id_buffer = create_id_buffer(
        app->res.mesh_p["node_mesh"_h]->vertices.size(), id,
        hs {"node_id_buffer" + std::to_string(index)}
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);

    BufferLayout layout2;
    layout2.add(1, BufferLayout::Float, 1);

    auto vertex_array = app->res.vertex_array.load(hs {"node_vertex_array" + std::to_string(index)});
    vertex_array->add_buffer(vertex_buffer, layout);
    vertex_array->add_buffer(id_buffer, layout2);
    vertex_array->add_index_buffer(index_buffer);
    VertexArray::unbind();

    auto material_instance = app->res.material_instance.load(
        hs {"node_material_instance" + std::to_string(index)},
        app->res.material["basic_material"_h]
    );
    material_instance->set_vec4("u_color", glm::vec4(0.0f));
}

void StandardGameScene::initialize_rendering_board_no_normal() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_wood_shader"_h,
        encr(paths::path_for_assets(assets::BOARD_VERTEX_SHADER_NO_NORMAL)),
        encr(paths::path_for_assets(assets::BOARD_FRAGMENT_SHADER_NO_NORMAL)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess"
        },
        std::vector {
            app->renderer->get_projection_view_uniform_block(),
            app->renderer->get_light_uniform_block(),
            app->renderer->get_light_view_position_uniform_block(),
            app->renderer->get_light_space_uniform_block()
        }
    );
    app->renderer->setup_shader(shader);

    auto vertex_buffer = app->res.buffer.load(
        "board_wood_vertex_buffer"_h,
        app->res.mesh_ptn["board_wood_mesh"_h]->vertices.data(),
        app->res.mesh_ptn["board_wood_mesh"_h]->vertices.size() * sizeof(PTN)
    );

    auto index_buffer = app->res.index_buffer.load(
        "board_wood_index_buffer"_h,
        app->res.mesh_ptn["board_wood_mesh"_h]->indices.data(),
        app->res.mesh_ptn["board_wood_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("board_wood_vertex_array"_h);
    vertex_array->add_buffer(vertex_buffer, layout);
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

    auto material = app->res.material.load("wood_material"_h, shader);
    material->add_texture("u_material.diffuse");
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular");
    material->add_uniform(Material::Uniform::Float, "u_material.shininess");

    auto material_instance = app->res.material_instance.load("board_wood_material_instance"_h, material);
    material_instance->set_texture("u_material.diffuse", diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess", 4.0f);
}

void StandardGameScene::initialize_rendering_board_paint_no_normal() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_paint_shader"_h,
        encr(paths::path_for_assets(assets::BOARD_PAINT_VERTEX_SHADER_NO_NORMAL)),
        encr(paths::path_for_assets(assets::BOARD_PAINT_FRAGMENT_SHADER_NO_NORMAL)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
        },
        std::vector {
            app->renderer->get_projection_view_uniform_block(),
            app->renderer->get_light_uniform_block(),
            app->renderer->get_light_view_position_uniform_block(),
            app->renderer->get_light_space_uniform_block()
        }
    );
    app->renderer->setup_shader(shader);

    auto vertex_buffer = app->res.buffer.load(
        "board_paint_vertex_buffer"_h,
        app->res.mesh_ptn["board_paint_mesh"_h]->vertices.data(),
        app->res.mesh_ptn["board_paint_mesh"_h]->vertices.size() * sizeof(PTN)
    );

    auto index_buffer = app->res.index_buffer.load(
        "board_paint_index_buffer"_h,
        app->res.mesh_ptn["board_paint_mesh"_h]->indices.data(),
        app->res.mesh_ptn["board_paint_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("board_paint_vertex_array"_h);
    vertex_array->add_buffer(vertex_buffer, layout);
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

    auto material_instance = app->res.material_instance.load("board_paint_material_instance"_h, material);
    material_instance->set_texture("u_material.diffuse", diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess", 4.0f);
}

void StandardGameScene::initialize_rendering_pieces_no_normal() {
        auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "piece_shader"_h,
        encr(paths::path_for_assets(assets::PIECE_VERTEX_SHADER_NO_NORMAL)),
        encr(paths::path_for_assets(assets::PIECE_FRAGMENT_SHADER_NO_NORMAL)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
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

    auto material = app->res.material.load("tinted_wood_material"_h, shader);
    material->add_texture("u_material.diffuse");
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular");
    material->add_uniform(Material::Uniform::Float, "u_material.shininess");
    material->add_uniform(Material::Uniform::Vec3, "u_material.tint");

    auto white_piece_vertex_buffer = app->res.buffer.load(
        "white_piece_vertex_buffer"_h,
        app->res.mesh_ptn["white_piece_mesh"_h]->vertices.data(),
        app->res.mesh_ptn["white_piece_mesh"_h]->vertices.size() * sizeof(PTN)
    );

    auto white_piece_index_buffer = app->res.index_buffer.load(
        "white_piece_index_buffer"_h,
        app->res.mesh_ptn["white_piece_mesh"_h]->indices.data(),
        app->res.mesh_ptn["white_piece_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    auto black_piece_vertex_buffer = app->res.buffer.load(
        "black_piece_vertex_buffer"_h,
        app->res.mesh_ptn["black_piece_mesh"_h]->vertices.data(),
        app->res.mesh_ptn["black_piece_mesh"_h]->vertices.size() * sizeof(PTN)
    );

    auto black_piece_index_buffer = app->res.index_buffer.load(
        "black_piece_index_buffer"_h,
        app->res.mesh_ptn["black_piece_mesh"_h]->indices.data(),
        app->res.mesh_ptn["black_piece_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    for (size_t i = 0; i < 9; i++) {
        initialize_rendering_piece_no_normal(
            i, app->res.mesh_ptn["white_piece_mesh"_h],
            white_piece_diffuse_texture, white_piece_vertex_buffer, white_piece_index_buffer
        );
    }

    for (size_t i = 9; i < 18; i++) {
        initialize_rendering_piece_no_normal(
            i, app->res.mesh_ptn["black_piece_mesh"_h],
            black_piece_diffuse_texture, black_piece_vertex_buffer, black_piece_index_buffer
        );
    }
}

void StandardGameScene::initialize_rendering_piece_no_normal(
        size_t index,
        std::shared_ptr<Mesh<PTN>> mesh,
        std::shared_ptr<Texture> diffuse_texture,
        std::shared_ptr<Buffer> vertex_buffer,
        std::shared_ptr<IndexBuffer> index_buffer) {
    auto& data = app->user_data<Data>();

    const identifier::Id id = identifier::generate_id();
    data.piece_ids[index] = id;

    auto id_buffer = create_id_buffer(
        mesh->vertices.size(), id,
        hs {"piece_id_buffer" + std::to_string(index)}
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);

    BufferLayout layout2;
    layout2.add(4, BufferLayout::Float, 1);

    auto vertex_array = app->res.vertex_array.load(hs {"piece_vertex_array" + std::to_string(index)});
    vertex_array->add_buffer(vertex_buffer, layout);
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
    material_instance->set_vec3("u_material.tint", DEFAULT_TINT);
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
        "data/textures/light_bulb/light_bulb.png",
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

void StandardGameScene::setup_and_add_model_piece(size_t index, const glm::vec3& position,
        std::shared_ptr<IndexBuffer> index_buffer) {
    auto& data = app->user_data<Data>();

    Piece& piece = board.pieces.at(index);

    piece.model->position = position;
    piece.model->rotation = RANDOM_PIECE_ROTATION();
    piece.model->vertex_array = app->res.vertex_array[hs {"piece_vertex_array" + std::to_string(index)}];
    piece.model->index_buffer = index_buffer;
    piece.model->scale = WORLD_SCALE;
    piece.model->material = app->res.material_instance[hs {"piece_material_instance" + std::to_string(index)}];
    piece.model->outline_color = std::make_optional<glm::vec3>(1.0f);
    piece.model->id = std::make_optional<identifier::Id>(data.piece_ids[index]);
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

    Node& node = board.nodes.at(index);

    node.model->vertex_array = app->res.vertex_array[hs {"node_vertex_array" + std::to_string(index)}];
    node.model->index_buffer = app->res.index_buffer["node_index_buffer"_h];
    node.model->position = position;
    node.model->scale = WORLD_SCALE;
    node.model->material = app->res.material_instance[hs {"node_material_instance" + std::to_string(index)}];
    node.model->id = std::make_optional<identifier::Id>(data.node_ids[index]);

    app->renderer->add_model(node.model);

    DEB_DEBUG("Setup model node {}", index);
}

void StandardGameScene::setup_entities() {
    auto& data = app->user_data<Data>();

    board = StandardBoard {};
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
        board.pieces.at(i).model->id = data.piece_ids[i];  // FIXME think this through
    }
    for (size_t i = 9; i < 18; i++) {
        board.pieces.at(i).model->id = data.piece_ids[i];  // FIXME this
    }
    for (size_t i = 0; i < 24; i++) {
        board.nodes.at(i).model->id = data.node_ids[i];  // FIXME this
    }

    DEB_DEBUG("Setup board's entities' IDs");
}

void StandardGameScene::setup_skybox() {
    if (app->user_data<Data>().options.skybox == game_options::NONE) {
        DEB_DEBUG("Setup skybox");
        return;
    }

    const std::array<std::shared_ptr<TextureData>, 6> data = {
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
    } else if (data.options.skybox == game_options::NONE) {
        app->renderer->light = LIGHT_NONE;
        app->renderer->light_space = SHADOWS_NONE;
    } else {
        ASSERT(false, "Invalid skybox");
    }

#ifdef PLATFORM_GAME_DEBUG
    if (data.options.skybox == game_options::FIELD) {
        data.quad_cache["light_bulb"_h]->position = LIGHT_FIELD.position;
    } else if (data.options.skybox == game_options::AUTUMN) {
        data.quad_cache["light_bulb"_h]->position = LIGHT_AUTUMN.position;
    } else if (data.options.skybox == game_options::NONE) {
        data.quad_cache["light_bulb"_h]->position = LIGHT_NONE.position;
    }
#endif

    DEB_DEBUG("Setup light");
}

void StandardGameScene::setup_camera() {
    auto& data = app->user_data<Data>();

    constexpr float PITCH = 47.0f;
    constexpr float DISTANCE_TO_POINT = 8.0f;

    const glm::mat4 projection = glm::perspective(
        glm::radians(FOV),
        static_cast<float>(app->data().width) / app->data().height,
        NEAR, FAR
    );

    camera = Camera {
        data.options.sensitivity,
        PITCH,
        glm::vec3(0.0f),
        DISTANCE_TO_POINT,
        projection
    };

    default_camera_position = camera.get_position();

    camera = Camera {
        data.options.sensitivity,
        PITCH,
        glm::vec3(0.0f),
        DISTANCE_TO_POINT + 0.7f,
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

    auto turn_indicator = data.image_cache.load(
        "turn_indicator"_h, app->res.texture["white_indicator_texture"_h]
    );
    turn_indicator->stick(gui::Sticky::SE);
    turn_indicator->offset(30, gui::Relative::Right);
    turn_indicator->offset(30, gui::Relative::Bottom);
    turn_indicator->scale(0.4f, 1.0f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);
    app->gui_renderer->add_widget(turn_indicator);

    auto timer_text = data.text_cache.load(
        "timer_text"_h,
        app->res.font["good_dog_plain_font"_h],
        "00:00", 1.5f, glm::vec3(0.9f)
    );
    timer_text->stick(gui::Sticky::N);
    timer_text->offset(60, gui::Relative::Top);
    timer_text->scale(0.6f, 1.4f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);
    timer_text->set_shadows(true);

    if (!data.options.hide_timer) {
        app->gui_renderer->add_widget(timer_text);
    }

    auto wait_indicator = data.image_cache.load(
        "wait_indicator"_h, app->res.texture["wait_indicator_texture"_h]
    );
    wait_indicator->stick(gui::Sticky::NE);
    wait_indicator->offset(25, gui::Relative::Right);
    wait_indicator->offset(55, gui::Relative::Top);
    wait_indicator->scale(0.4f, 1.0f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);

    auto computer_thinking_indicator = data.image_cache.load(
        "computer_thinking_indicator"_h, app->res.texture["computer_thinking_indicator_texture"_h]
    );
    computer_thinking_indicator->stick(gui::Sticky::NE);
    computer_thinking_indicator->offset(25, gui::Relative::Right);
    computer_thinking_indicator->offset(55, gui::Relative::Top);
    computer_thinking_indicator->scale(0.4f, 1.0f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);
}

void StandardGameScene::update_game_state() {
    switch (game.state) {
        case GameState::MaybeNextPlayer:
            switch (board.turn) {
                case BoardPlayer::White:
                    switch (game.white_player) {
                        case GamePlayer::None:
                            ASSERT(false, "Player must not be None");
                            break;
                        case GamePlayer::Human:
                            game.state = GameState::HumanBeginMove;
                            break;
                        case GamePlayer::Computer:
                            game.state = GameState::ComputerBeginMove;
                            break;
                    }
                    break;
                case BoardPlayer::Black:
                    switch (game.black_player) {
                        case GamePlayer::None:
                            ASSERT(false, "Player must not be None");
                            break;
                        case GamePlayer::Human:
                            game.state = GameState::HumanBeginMove;
                            break;
                        case GamePlayer::Computer:
                            game.state = GameState::ComputerBeginMove;
                            break;
                    }
                    break;
            }
            break;
        case GameState::HumanBeginMove:
            game.begin_human_move();
            game.state = GameState::HumanThinkingMove;
            break;
        case GameState::HumanThinkingMove:
            break;
        case GameState::HumanDoingMove:
            if (board.next_move) {
                game.state = GameState::HumanEndMove;
            }
            break;
        case GameState::HumanEndMove:
            game.end_human_move();
            game.state = GameState::MaybeNextPlayer;
            break;
        case GameState::ComputerBeginMove:
            game.begin_computer_move();
            game.state = GameState::ComputerThinkingMove;
            break;
        case GameState::ComputerThinkingMove:
            if (!minimax_thread.is_running()) {
                minimax_thread.join();

                const bool switched_turn = game.end_computer_move();

                update_after_computer_move(switched_turn);
            }
            break;
        case GameState::ComputerDoingMove:
            if (board.next_move) {
                game.state = GameState::ComputerEndMove;
            }
            break;
        case GameState::ComputerEndMove:
            game.state = GameState::MaybeNextPlayer;
            break;
    }
}

void StandardGameScene::update_timer_text() {
    auto& data = app->user_data<Data>();

    char time[32];
    timer.get_time_formatted(time);
    data.text_cache["timer_text"_h]->set_text(time);
}

void StandardGameScene::update_turn_indicator() {
    auto& data = app->user_data<Data>();

    if (board.turn == BoardPlayer::White) {
        data.image_cache["turn_indicator"_h]->set_image(app->res.texture["white_indicator_texture"_h]);
    } else {
        data.image_cache["turn_indicator"_h]->set_image(app->res.texture["black_indicator_texture"_h]);
    }
}

void StandardGameScene::update_wait_indicator() {
    auto& data = app->user_data<Data>();

    if (!board.next_move) {
        if (!show_wait_indicator) {
            app->gui_renderer->add_widget(data.image_cache["wait_indicator"_h]);
            show_wait_indicator = true;
        }
    } else {
        if (show_wait_indicator) {
            app->gui_renderer->remove_widget(data.image_cache["wait_indicator"_h]);
            show_wait_indicator = false;
        }
    }
}

void StandardGameScene::update_computer_thinking_indicator() {
    auto& data = app->user_data<Data>();

    if (game.state == GameState::ComputerThinkingMove) {
        if (!show_computer_thinking_indicator) {
            app->gui_renderer->add_widget(data.image_cache["computer_thinking_indicator"_h]);
            show_computer_thinking_indicator = true;
        }
    } else {
        if (show_computer_thinking_indicator) {
            app->gui_renderer->remove_widget(data.image_cache["computer_thinking_indicator"_h]);
            show_computer_thinking_indicator = false;
        }
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

void StandardGameScene::update_after_human_move(bool did_action, bool switched_turn, bool must_take_piece_or_took_piece) {
    if (did_action) {
        game.state = GameState::HumanDoingMove;
    }

    if (did_action && !made_first_move && !timer.is_running()) {
        timer.start();
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

    imgui_layer.can_undo = undo_redo_state.undo.size() > 0;
    imgui_layer.can_redo = undo_redo_state.redo.size() > 0;
}

void StandardGameScene::update_after_computer_move(bool switched_turn) {
    game.state = GameState::ComputerDoingMove;

    if (!made_first_move && !timer.is_running()) {
        timer.start();
        made_first_move = true;
    }

    if (board.phase == BoardPhase::GameOver) {
        timer.stop();
    }

    if (switched_turn) {
        update_turn_indicator();
    }

    imgui_layer.can_undo = undo_redo_state.undo.size() > 0;
    imgui_layer.can_redo = undo_redo_state.redo.size() > 0;
}

void StandardGameScene::set_skybox(Skybox skybox) {
    if (skybox == Skybox::None) {
        app->renderer->set_skybox(nullptr);
        return;
    }

    if (skybox_loader != nullptr) {
        DEB_WARN("Skybox already loading");
        return;
    }

    auto& data = app->user_data<Data>();

    skybox_loader = std::make_unique<assets_load::SkyboxLoader>(assets_load::skybox);
    skybox_loader->start_loading_thread(data.launcher_options.texture_quality, data.options.skybox);
}

void StandardGameScene::change_skybox() {
    const std::array<std::shared_ptr<TextureData>, 6> data = {
        app->res.texture_data["skybox_px_texture"_h],
        app->res.texture_data["skybox_nx_texture"_h],
        app->res.texture_data["skybox_py_texture"_h],
        app->res.texture_data["skybox_ny_texture"_h],
        app->res.texture_data["skybox_pz_texture"_h],
        app->res.texture_data["skybox_nz_texture"_h]
    };

    auto texture = app->res.texture_3d.force_load("skybox_texture"_h, data);
    app->renderer->set_skybox(texture);

    setup_light();
}

void StandardGameScene::check_skybox_loader() {
    if (skybox_loader != nullptr && skybox_loader->done_loading()) {
        skybox_loader->join_and_merge(app->res);
        skybox_loader.reset();

        change_skybox();
    }
}

void StandardGameScene::set_board_paint_texture() {
    if (board_paint_texture_loader != nullptr) {
        DEB_WARN("Board paint texture already loading");
        return;
    }

    auto& data = app->user_data<Data>();

    board_paint_texture_loader = std::make_unique<assets_load::BoardPaintTextureLoader>(assets_load::board_paint_texture);
    board_paint_texture_loader->start_loading_thread(data.launcher_options.texture_quality, data.options.labeled_board);
}

void StandardGameScene::change_board_paint_texture() {
    auto& data = app->user_data<Data>();

    TextureSpecification specification;
    specification.mag_filter = Filter::Linear;
    specification.mipmapping = true;
    specification.bias = -1.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.force_load(
        "board_paint_diffuse_texture"_h,
        app->res.texture_data["board_paint_diffuse_texture"_h],
        specification
    );

    app->res.material_instance["board_paint_material_instance"_h]->set_texture("u_material.diffuse", diffuse_texture, 0);
}

void StandardGameScene::check_board_paint_texture_loader() {
    if (board_paint_texture_loader != nullptr && board_paint_texture_loader->done_loading()) {
        board_paint_texture_loader->join_and_merge(app->res);
        board_paint_texture_loader.reset();

        change_board_paint_texture();
    }
}

void StandardGameScene::save_game() {
    board.finalize_pieces_state();

    StandardBoardSerialized serialized;
    board.to_serialized(serialized);

    save_load::SavedGame<StandardBoardSerialized> saved_game;
    saved_game.board_serialized = serialized;
    saved_game.camera = camera;
    saved_game.time = timer.get_time();

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
        REL_ERROR("Could not save game");

        save_load::handle_save_file_not_open_error(app->data().application_name);
    } catch (const save_load::SaveFileError& e) {
        REL_ERROR("{}", e.what());
        REL_ERROR("Could not save game");
    }
}

void StandardGameScene::load_game() {
    board.finalize_pieces_state();

    save_load::SavedGame<StandardBoardSerialized> saved_game;

    try {
        save_load::load_game_from_file(saved_game);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_WARN("{}", e.what());
        REL_WARN("Could not load game");

        save_load::handle_save_file_not_open_error(app->data().application_name);

        imgui_layer.show_could_not_load_game = true;
        return;
    } catch (const save_load::SaveFileError& e) {
        REL_WARN("{}", e.what());  // TODO maybe delete file
        REL_WARN("Could not load game");

        imgui_layer.show_could_not_load_game = true;
        return;
    }

    board.from_serialized(saved_game.board_serialized);
    camera = saved_game.camera;
    timer = Timer {app, saved_game.time};
    undo_redo_state = std::move(saved_game.undo_redo_state);
    game.white_player = saved_game.white_player;
    game.black_player = saved_game.black_player;

    made_first_move = false;

    update_cursor();
}

void StandardGameScene::undo() {
    ASSERT(!undo_redo_state.undo.empty(), "Undo history must not be empty");

    if (!board.next_move) {
        DEB_WARN("Cannot undo when pieces are in air");
        return;
    }

    const bool undo_game_over = board.phase == BoardPhase::None;

    using State = UndoRedoState<StandardBoardSerialized>::State;

    StandardBoardSerialized serialized;
    board.to_serialized(serialized);

    State current_state = { serialized, camera };
    const State& previous_state = undo_redo_state.undo.back();

    board.from_serialized(previous_state.board_serialized);
    camera = previous_state.camera;

    undo_redo_state.undo.pop_back();
    undo_redo_state.redo.push_back(current_state);

    DEB_DEBUG("Undid move; popped from undo stack and pushed onto redo stack");

    game.state = GameState::MaybeNextPlayer;
    made_first_move = board.not_placed_white_pieces_count + board.not_placed_black_pieces_count != 18;

    if (undo_game_over) {
        timer.start();
    }

    update_cursor();
}

void StandardGameScene::redo() {
    ASSERT(!undo_redo_state.redo.empty(), "Redo history must not be empty");

    if (!board.next_move) {
        DEB_WARN("Cannot redo when pieces are in air");
        return;
    }

    using State = UndoRedoState<StandardBoardSerialized>::State;

    StandardBoardSerialized serialized;
    board.to_serialized(serialized);

    State current_state = { serialized, camera };
    const State& previous_state = undo_redo_state.redo.back();

    board.from_serialized(previous_state.board_serialized);
    camera = previous_state.camera;

    undo_redo_state.redo.pop_back();
    undo_redo_state.undo.push_back(current_state);

    DEB_DEBUG("Redid move; popped from redo stack and pushed onto undo stack");

    game.state = GameState::MaybeNextPlayer;
    made_first_move = board.not_placed_white_pieces_count + board.not_placed_black_pieces_count != 18;

    const bool redo_game_over = board.phase == BoardPhase::None;

    if (redo_game_over) {
        timer.stop();
        board.phase = BoardPhase::GameOver;  // Make the game over screen show up again
    }

    update_cursor();
}
