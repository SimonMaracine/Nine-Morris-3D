#include <nine_morris_3d_engine/engine_audio.h>
#include <nine_morris_3d_engine/engine_graphics.h>
#include <nine_morris_3d_engine/engine_other.h>

#include "game/entities/boards/standard_board.h"
#include "game/entities/serialization/standard_board_serialized.h"
#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "game/scenes/standard_game_scene.h"
#include "game/scenes/imgui_layer.h"
#include "game/game_options.h"
#include "game/save_load.h"
#include "game/assets.h"
#include "game/assets_load.h"
#include "game/point_camera_controller.h"
#include "other/constants.h"
#include "other/data.h"
#include "other/options.h"
#include "other/save_load_gracefully.h"

using namespace encrypt;

void StandardGameScene::on_start() {
    auto& data = app->user_data<Data>();

    setup_entities();

    setup_and_add_model_board();
    setup_and_add_model_board_paint();
    setup_and_add_model_pieces();
    setup_and_add_model_nodes();

    setup_camera();
    setup_widgets();

    keyboard = KeyboardControls {app, &board, app->res.quad["keyboard_controls"_h]};
    keyboard.post_initialize();

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
    board.camera_controller = &camera_controller;
    board.undo_redo_state = &undo_redo_state;

    app->window->set_cursor(data.options.custom_cursor ? data.arrow_cursor : 0);

#ifdef PLATFORM_GAME_DEBUG
    app->renderer->origin = true;
    app->renderer->add_quad(app->res.quad["light_bulb"_h]);
#endif

    imgui_layer.update();

    camera_controller.go_towards_position(default_camera_position);
    camera_controller.setup_events(app);

    // Can dispose of these
    app->res.texture_data.clear();
    app->res.sound_data.clear();
}

void StandardGameScene::on_stop() {
    auto& data = app->user_data<Data>();

    save_load_gracefully::save_to_file<game_options::GameOptions>(
        game_options::GAME_OPTIONS_FILE, data.options, app
    );

    if (data.options.save_on_exit && !app->running && made_first_move) {
        save_game();
    }

#ifdef PLATFORM_GAME_DEBUG
    app->renderer->origin = false;
#endif

    imgui_layer.reset();
    camera_controller.remove_events(app);
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
        initialize_board();
        initialize_board_paint();
        initialize_pieces();
    } else {
        initialize_board_no_normal();
        initialize_board_paint_no_normal();
        initialize_pieces_no_normal();
    }
    initialize_nodes();

    initialize_keyboard_controls();
#ifdef PLATFORM_GAME_DEBUG
    initialize_light_bulb();
#endif

    initialize_skybox();
    initialize_light();
    initialize_indicators_textures();

    app->evt.sink<MouseButtonPressedEvent>().connect<&StandardGameScene::on_mouse_button_pressed>(this);
    app->evt.sink<MouseButtonReleasedEvent>().connect<&StandardGameScene::on_mouse_button_released>(this);
    app->evt.sink<KeyPressedEvent>().connect<&StandardGameScene::on_key_pressed>(this);
    app->evt.sink<KeyReleasedEvent>().connect<&StandardGameScene::on_key_released>(this);
    app->evt.sink<WindowResizedEvent>().connect<&StandardGameScene::on_window_resized>(this);

    auto track = app->res.music_track.load("music"_h, app->res.sound_data["music"_h]);
    current_music_track = track;

    if (data.options.enable_music) {
        music::play_music_track(track);
    }

    using namespace assets_load;

    skybox_loader = std::make_unique<SkyboxLoader>(
        skybox, std::bind(&StandardGameScene::change_skybox, this)
    );
    board_paint_texture_loader = std::make_unique<BoardPaintTextureLoader>(
        board_paint_texture, std::bind(&StandardGameScene::change_board_paint_texture, this)
    );
}

void StandardGameScene::on_update() {
    if (!imgui_layer.hovering_gui) {
        camera_controller.update(app->get_delta());

        board.update_nodes(app->renderer->get_hovered_id());
        board.update_pieces(app->renderer->get_hovered_id());
        timer.update();  // TODO maybe should have been after update_game_state()

        // Update listener position, look at and up vectors every frame
        app->openal->get_listener().set_position(camera_controller.get_position());
        app->openal->get_listener().set_look_at_and_up(
            camera_controller.get_point() - camera_controller.get_position(),
            glm::rotate(UP_VECTOR, camera_controller.get_rotation().y, UP_VECTOR)
        );
    }

    update_game_state();

    update_timer_text();
    update_wait_indicator();
    update_computer_thinking_indicator();

    skybox_loader->update(app);
    board_paint_texture_loader->update(app);
}

void StandardGameScene::on_fixed_update() {
    camera_controller.update_friction();
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
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.button == input::MouseButton::LEFT) {
        if (board.next_move) {
            board.click(app->renderer->get_hovered_id());
        }
    }
}

void StandardGameScene::on_mouse_button_released(const MouseButtonReleasedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.button == input::MouseButton::LEFT) {
        const bool valid_phases = (
            board.phase == BoardPhase::PlacePieces || board.phase == BoardPhase::MovePieces
        );

        if (board.next_move && board.is_players_turn && valid_phases) {
            const auto [did_action, switched_turn, must_take_piece_or_took_piece] = (
                board.release(app->renderer->get_hovered_id())
            );

            update_after_human_move(did_action, switched_turn, must_take_piece_or_took_piece);
        }

        if (show_keyboard_controls) {
            app->renderer->remove_quad(app->res.quad["keyboard_controls"_h]);
            show_keyboard_controls = false;
        }
    }
}

void StandardGameScene::on_key_pressed(const KeyPressedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    switch (event.key) {
        case input::Key::UP:
        case input::Key::DOWN:
        case input::Key::LEFT:
        case input::Key::RIGHT:
        case input::Key::ENTER:
            if (!show_keyboard_controls) {
                app->renderer->add_quad(app->res.quad["keyboard_controls"_h]);
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
                    KB::Direction::Up, camera_controller.get_angle_around_point()
                )
            );
            break;
        case input::Key::DOWN:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Down, camera_controller.get_angle_around_point()
                )
            );
            break;
        case input::Key::LEFT:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Left, camera_controller.get_angle_around_point()
                )
            );
            break;
        case input::Key::RIGHT:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Right, camera_controller.get_angle_around_point()
                )
            );
            break;
        case input::Key::ENTER: {
            const bool valid_phases = (
                board.phase == BoardPhase::PlacePieces || board.phase == BoardPhase::MovePieces
            );

            if (board.next_move && board.is_players_turn && valid_phases) {
                const auto [did_action, switched_turn, must_take_piece_or_took_piece] = (
                    keyboard.click_and_release()
                );

                update_after_human_move(did_action, switched_turn, must_take_piece_or_took_piece);
            }
            break;
        }
        default:
            break;
    }
}

void StandardGameScene::on_key_released(const KeyReleasedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.key == input::Key::SPACE) {
        camera_controller.go_towards_position(default_camera_position);
    }
}

void StandardGameScene::on_window_resized(const WindowResizedEvent& event) {
    camera.set_projection(event.width, event.height, LENS_FOV, LENS_NEAR, LENS_FAR);
}

std::shared_ptr<gl::Buffer> StandardGameScene::create_id_buffer(size_t vertices_size, identifier::Id id, hs hash) {
    std::vector<float> array;
    array.resize(vertices_size);

    for (size_t i = 0; i < array.size(); i++) {
        array[i] = id;
    }

    return app->res.buffer.load(hash, array.data(), array.size() * sizeof(float));
}

void StandardGameScene::initialize_board() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_wood"_h,
        encr(path::path_for_assets(assets::BOARD_VERTEX_SHADER)),
        encr(path::path_for_assets(assets::BOARD_FRAGMENT_SHADER)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
            "u_material.normal"
        },
        std::vector {
            app->renderer->get_storage().projection_view_uniform_block,
            app->renderer->get_storage().light_uniform_block,
            app->renderer->get_storage().light_view_position_uniform_block,
            app->renderer->get_storage().light_space_uniform_block
        }
    );
    app->renderer->setup_shader(shader);

    auto vertex_buffer = app->res.buffer.load(
        "board_wood"_h,
        app->res.mesh_ptnt["board_wood"_h]->vertices.data(),
        app->res.mesh_ptnt["board_wood"_h]->vertices.size() * sizeof(PTNT)
    );

    auto index_buffer = app->res.index_buffer.load(
        "board_wood"_h,
        app->res.mesh_ptnt["board_wood"_h]->indices.data(),
        app->res.mesh_ptnt["board_wood"_h]->indices.size() * sizeof(unsigned int)
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);
    layout.add(3, BufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("board_wood"_h);
    vertex_array->add_buffer(vertex_buffer, layout);
    vertex_array->add_index_buffer(index_buffer);
    gl::VertexArray::unbind();

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmapping = true;
    specification.bias = -2.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.load(
        "board_wood_diffuse"_h,
        app->res.texture_data["board_wood_diffuse"_h],
        specification
    );

    auto normal_texture = app->res.texture.load(
        "board_normal"_h,
        app->res.texture_data["board_normal"_h],
        specification
    );

    auto material = app->res.material.load("wood"_h, shader);
    material->add_texture("u_material.diffuse");
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular");
    material->add_uniform(Material::Uniform::Float, "u_material.shininess");
    material->add_texture("u_material.normal");

    auto material_instance = app->res.material_instance.load("board_wood"_h, material);
    material_instance->set_texture("u_material.diffuse", diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess", 4.0f);
    material_instance->set_texture("u_material.normal", normal_texture, 1);
}

void StandardGameScene::initialize_board_paint() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_paint"_h,
        encr(path::path_for_assets(assets::BOARD_PAINT_VERTEX_SHADER)),
        encr(path::path_for_assets(assets::BOARD_PAINT_FRAGMENT_SHADER)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
            "u_material.normal"
        },
        std::vector {
            app->renderer->get_storage().projection_view_uniform_block,
            app->renderer->get_storage().light_uniform_block,
            app->renderer->get_storage().light_view_position_uniform_block,
            app->renderer->get_storage().light_space_uniform_block
        }
    );
    app->renderer->setup_shader(shader);

    auto vertex_buffer = app->res.buffer.load(
        "board_paint"_h,
        app->res.mesh_ptnt["board_paint"_h]->vertices.data(),
        app->res.mesh_ptnt["board_paint"_h]->vertices.size() * sizeof(PTNT)
    );

    auto index_buffer = app->res.index_buffer.load(
        "board_paint"_h,
        app->res.mesh_ptnt["board_paint"_h]->indices.data(),
        app->res.mesh_ptnt["board_paint"_h]->indices.size() * sizeof(unsigned int)
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);
    layout.add(3, BufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("board_paint"_h);
    vertex_array->add_buffer(vertex_buffer, layout);
    vertex_array->add_index_buffer(index_buffer);
    gl::VertexArray::unbind();

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmapping = true;
    specification.bias = -1.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.load(
        "board_paint_diffuse"_h,
        app->res.texture_data["board_paint_diffuse"_h],
        specification
    );

    auto material = app->res.material.load("board_paint"_h, shader);
    material->add_texture("u_material.diffuse");
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular");
    material->add_uniform(Material::Uniform::Float, "u_material.shininess");
    material->add_texture("u_material.normal");

    auto material_instance = app->res.material_instance.load("board_paint"_h, material);
    material_instance->set_texture("u_material.diffuse", diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess", 4.0f);
    material_instance->set_texture("u_material.normal", app->res.texture["board_normal"_h], 1);
}

void StandardGameScene::initialize_pieces() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "piece"_h,
        encr(path::path_for_assets(assets::PIECE_VERTEX_SHADER)),
        encr(path::path_for_assets(assets::PIECE_FRAGMENT_SHADER)),
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
            app->renderer->get_storage().projection_view_uniform_block,
            app->renderer->get_storage().light_uniform_block,
            app->renderer->get_storage().light_view_position_uniform_block,
            app->renderer->get_storage().light_space_uniform_block
        }
    );
    app->renderer->setup_shader(shader);

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmapping = true;
    specification.bias = -1.5f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto white_piece_diffuse_texture = app->res.texture.load(
        "white_piece_diffuse"_h,
        app->res.texture_data["white_piece_diffuse"_h],
        specification
    );

    auto black_piece_diffuse_texture = app->res.texture.load(
        "black_piece_diffuse"_h,
        app->res.texture_data["black_piece_diffuse"_h],
        specification
    );

    auto piece_normal_texture = app->res.texture.load(
        "piece_normal"_h,
        app->res.texture_data["piece_normal"_h],
        specification
    );

    auto material = app->res.material.load("tinted_wood"_h, shader);
    material->add_texture("u_material.diffuse");
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular");
    material->add_uniform(Material::Uniform::Float, "u_material.shininess");
    material->add_texture("u_material.normal");
    material->add_uniform(Material::Uniform::Vec3, "u_material.tint");

    auto white_piece_vertex_buffer = app->res.buffer.load(
        "white_piece"_h,
        app->res.mesh_ptnt["white_piece"_h]->vertices.data(),
        app->res.mesh_ptnt["white_piece"_h]->vertices.size() * sizeof(PTNT)
    );

    auto white_piece_index_buffer = app->res.index_buffer.load(
        "white_piece"_h,
        app->res.mesh_ptnt["white_piece"_h]->indices.data(),
        app->res.mesh_ptnt["white_piece"_h]->indices.size() * sizeof(unsigned int)
    );

    auto black_piece_vertex_buffer = app->res.buffer.load(
        "black_piece"_h,
        app->res.mesh_ptnt["black_piece"_h]->vertices.data(),
        app->res.mesh_ptnt["black_piece"_h]->vertices.size() * sizeof(PTNT)
    );

    auto black_piece_index_buffer = app->res.index_buffer.load(
        "black_piece"_h,
        app->res.mesh_ptnt["black_piece"_h]->indices.data(),
        app->res.mesh_ptnt["black_piece"_h]->indices.size() * sizeof(unsigned int)
    );

    app->res.al_buffer.load(
        "piece_place1"_h,
        app->res.sound_data["piece_place1"_h]
    );

    app->res.al_buffer.load(
        "piece_place2"_h,
        app->res.sound_data["piece_place2"_h]
    );

    app->res.al_buffer.load(
        "piece_move1"_h,
        app->res.sound_data["piece_move1"_h]
    );

    app->res.al_buffer.load(
        "piece_move2"_h,
        app->res.sound_data["piece_move2"_h]
    );

    app->res.al_buffer.load(
        "piece_take"_h,
        app->res.sound_data["piece_take"_h]
    );

    for (size_t i = 0; i < 9; i++) {
        initialize_piece(
            i, app->res.mesh_ptnt["white_piece"_h],
            white_piece_diffuse_texture, white_piece_vertex_buffer, white_piece_index_buffer
        );
    }

    for (size_t i = 9; i < 18; i++) {
        initialize_piece(
            i, app->res.mesh_ptnt["black_piece"_h],
            black_piece_diffuse_texture, black_piece_vertex_buffer, black_piece_index_buffer
        );
    }
}

void StandardGameScene::initialize_piece(
        size_t index,
        std::shared_ptr<Mesh<PTNT>> mesh,
        std::shared_ptr<gl::Texture> diffuse_texture,
        std::shared_ptr<gl::Buffer> vertex_buffer,
        std::shared_ptr<gl::IndexBuffer> index_buffer) {
    auto& data = app->user_data<Data>();

    const identifier::Id id = identifier::generate_id();
    data.piece_ids[index] = id;

    auto id_buffer = create_id_buffer(
        mesh->vertices.size(), id,
        hs {"piece_id" + std::to_string(index)}
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);
    layout.add(3, BufferLayout::Float, 3);

    BufferLayout layout2;
    layout2.add(4, BufferLayout::Float, 1);

    auto vertex_array = app->res.vertex_array.load(hs {"piece" + std::to_string(index)});
    vertex_array->add_buffer(vertex_buffer, layout);
    vertex_array->add_buffer(id_buffer, layout2);
    vertex_array->add_index_buffer(index_buffer);
    gl::VertexArray::unbind();

    auto material_instance = app->res.material_instance.load(
        hs {"piece" + std::to_string(index)},
        app->res.material["tinted_wood"_h]
    );
    material_instance->set_texture("u_material.diffuse", diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess", 4.0f);
    material_instance->set_texture("u_material.normal", app->res.texture["piece_normal"_h], 1);
    material_instance->set_vec3("u_material.tint", DEFAULT_TINT);
}

void StandardGameScene::initialize_nodes() {
    auto shader = app->res.shader.load(
        "node"_h,
        encr(path::path_for_assets(assets::NODE_VERTEX_SHADER)),
        encr(path::path_for_assets(assets::NODE_FRAGMENT_SHADER)),
        std::vector<std::string> { "u_model_matrix", "u_color" },
        std::vector {
            app->renderer->get_storage().projection_view_uniform_block
        }
    );
    app->renderer->setup_shader(shader);

    auto material = app->res.material.load("basic"_h, shader);
    material->add_uniform(Material::Uniform::Vec4, "u_color");

    auto vertex_buffer = app->res.buffer.load(
        "node"_h,
        app->res.mesh_p["node"_h]->vertices.data(),
        app->res.mesh_p["node"_h]->vertices.size() * sizeof(P)
    );

    auto index_buffer = app->res.index_buffer.load(
        "node"_h,
        app->res.mesh_p["node"_h]->indices.data(),
        app->res.mesh_p["node"_h]->indices.size() * sizeof(unsigned int)
    );

    for (size_t i = 0; i < 24; i++) {
        initialize_node(i, vertex_buffer, index_buffer);
    }
}

void StandardGameScene::initialize_node(size_t index, std::shared_ptr<gl::Buffer> vertex_buffer,
        std::shared_ptr<gl::IndexBuffer> index_buffer) {
    auto& data = app->user_data<Data>();

    const identifier::Id id = identifier::generate_id();
    data.node_ids[index] = id;

    auto id_buffer = create_id_buffer(
        app->res.mesh_p["node"_h]->vertices.size(), id,
        hs {"node_id" + std::to_string(index)}
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);

    BufferLayout layout2;
    layout2.add(1, BufferLayout::Float, 1);

    auto vertex_array = app->res.vertex_array.load(hs {"node" + std::to_string(index)});
    vertex_array->add_buffer(vertex_buffer, layout);
    vertex_array->add_buffer(id_buffer, layout2);
    vertex_array->add_index_buffer(index_buffer);
    gl::VertexArray::unbind();

    auto material_instance = app->res.material_instance.load(
        hs {"node" + std::to_string(index)},
        app->res.material["basic"_h]
    );
    material_instance->set_vec4("u_color", glm::vec4(0.0f));
}

void StandardGameScene::initialize_board_no_normal() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_wood"_h,
        encr(path::path_for_assets(assets::BOARD_VERTEX_SHADER_NO_NORMAL)),
        encr(path::path_for_assets(assets::BOARD_FRAGMENT_SHADER_NO_NORMAL)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess"
        },
        std::vector {
            app->renderer->get_storage().projection_view_uniform_block,
            app->renderer->get_storage().light_uniform_block,
            app->renderer->get_storage().light_view_position_uniform_block,
            app->renderer->get_storage().light_space_uniform_block
        }
    );
    app->renderer->setup_shader(shader);

    auto vertex_buffer = app->res.buffer.load(
        "board_wood"_h,
        app->res.mesh_ptn["board_wood"_h]->vertices.data(),
        app->res.mesh_ptn["board_wood"_h]->vertices.size() * sizeof(PTN)
    );

    auto index_buffer = app->res.index_buffer.load(
        "board_wood"_h,
        app->res.mesh_ptn["board_wood"_h]->indices.data(),
        app->res.mesh_ptn["board_wood"_h]->indices.size() * sizeof(unsigned int)
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("board_wood"_h);
    vertex_array->add_buffer(vertex_buffer, layout);
    vertex_array->add_index_buffer(index_buffer);
    gl::VertexArray::unbind();

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmapping = true;
    specification.bias = -2.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.load(
        "board_wood_diffuse"_h,
        app->res.texture_data["board_wood_diffuse"_h],
        specification
    );

    auto material = app->res.material.load("wood"_h, shader);
    material->add_texture("u_material.diffuse");
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular");
    material->add_uniform(Material::Uniform::Float, "u_material.shininess");

    auto material_instance = app->res.material_instance.load("board_wood"_h, material);
    material_instance->set_texture("u_material.diffuse", diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess", 4.0f);
}

void StandardGameScene::initialize_board_paint_no_normal() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_paint"_h,
        encr(path::path_for_assets(assets::BOARD_PAINT_VERTEX_SHADER_NO_NORMAL)),
        encr(path::path_for_assets(assets::BOARD_PAINT_FRAGMENT_SHADER_NO_NORMAL)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
        },
        std::vector {
            app->renderer->get_storage().projection_view_uniform_block,
            app->renderer->get_storage().light_uniform_block,
            app->renderer->get_storage().light_view_position_uniform_block,
            app->renderer->get_storage().light_space_uniform_block
        }
    );
    app->renderer->setup_shader(shader);

    auto vertex_buffer = app->res.buffer.load(
        "board_paint"_h,
        app->res.mesh_ptn["board_paint"_h]->vertices.data(),
        app->res.mesh_ptn["board_paint"_h]->vertices.size() * sizeof(PTN)
    );

    auto index_buffer = app->res.index_buffer.load(
        "board_paint"_h,
        app->res.mesh_ptn["board_paint"_h]->indices.data(),
        app->res.mesh_ptn["board_paint"_h]->indices.size() * sizeof(unsigned int)
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("board_paint"_h);
    vertex_array->add_buffer(vertex_buffer, layout);
    vertex_array->add_index_buffer(index_buffer);
    gl::VertexArray::unbind();

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmapping = true;
    specification.bias = -1.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.load(
        "board_paint_diffuse"_h,
        app->res.texture_data["board_paint_diffuse"_h],
        specification
    );

    auto material = app->res.material.load("board_paint"_h, shader);
    material->add_texture("u_material.diffuse");
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular");
    material->add_uniform(Material::Uniform::Float, "u_material.shininess");

    auto material_instance = app->res.material_instance.load("board_paint"_h, material);
    material_instance->set_texture("u_material.diffuse", diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess", 4.0f);
}

void StandardGameScene::initialize_pieces_no_normal() {
    auto& data = app->user_data<Data>();

    // FIXME maybe should give another name
    auto shader = app->res.shader.load(
        "piece"_h,
        encr(path::path_for_assets(assets::PIECE_VERTEX_SHADER_NO_NORMAL)),
        encr(path::path_for_assets(assets::PIECE_FRAGMENT_SHADER_NO_NORMAL)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
            "u_material.tint"
        },
        std::vector {
            app->renderer->get_storage().projection_view_uniform_block,
            app->renderer->get_storage().light_uniform_block,
            app->renderer->get_storage().light_view_position_uniform_block,
            app->renderer->get_storage().light_space_uniform_block
        }
    );
    app->renderer->setup_shader(shader);

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmapping = true;
    specification.bias = -1.5f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto white_piece_diffuse_texture = app->res.texture.load(
        "white_piece_diffuse"_h,
        app->res.texture_data["white_piece_diffuse"_h],
        specification
    );

    auto black_piece_diffuse_texture = app->res.texture.load(
        "black_piece_diffuse"_h,
        app->res.texture_data["black_piece_diffuse"_h],
        specification
    );

    auto material = app->res.material.load("tinted_wood"_h, shader);
    material->add_texture("u_material.diffuse");
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular");
    material->add_uniform(Material::Uniform::Float, "u_material.shininess");
    material->add_uniform(Material::Uniform::Vec3, "u_material.tint");

    auto white_piece_vertex_buffer = app->res.buffer.load(
        "white_piece"_h,
        app->res.mesh_ptn["white_piece"_h]->vertices.data(),
        app->res.mesh_ptn["white_piece"_h]->vertices.size() * sizeof(PTN)
    );

    auto white_piece_index_buffer = app->res.index_buffer.load(
        "white_piece"_h,
        app->res.mesh_ptn["white_piece"_h]->indices.data(),
        app->res.mesh_ptn["white_piece"_h]->indices.size() * sizeof(unsigned int)
    );

    auto black_piece_vertex_buffer = app->res.buffer.load(
        "black_piece"_h,
        app->res.mesh_ptn["black_piece"_h]->vertices.data(),
        app->res.mesh_ptn["black_piece"_h]->vertices.size() * sizeof(PTN)
    );

    auto black_piece_index_buffer = app->res.index_buffer.load(
        "black_piece"_h,
        app->res.mesh_ptn["black_piece"_h]->indices.data(),
        app->res.mesh_ptn["black_piece"_h]->indices.size() * sizeof(unsigned int)
    );

    app->res.al_buffer.load(
        "piece_place1"_h,
        app->res.sound_data["piece_place1"_h]
    );

    app->res.al_buffer.load(
        "piece_place2"_h,
        app->res.sound_data["piece_place2"_h]
    );

    app->res.al_buffer.load(
        "piece_move1"_h,
        app->res.sound_data["piece_move1"_h]
    );

    app->res.al_buffer.load(
        "piece_move2"_h,
        app->res.sound_data["piece_move2"_h]
    );

    app->res.al_buffer.load(
        "piece_take"_h,
        app->res.sound_data["piece_take"_h]
    );

    for (size_t i = 0; i < 9; i++) {
        initialize_piece_no_normal(
            i, app->res.mesh_ptn["white_piece"_h],
            white_piece_diffuse_texture, white_piece_vertex_buffer, white_piece_index_buffer
        );
    }

    for (size_t i = 9; i < 18; i++) {
        initialize_piece_no_normal(
            i, app->res.mesh_ptn["black_piece"_h],
            black_piece_diffuse_texture, black_piece_vertex_buffer, black_piece_index_buffer
        );
    }
}

void StandardGameScene::initialize_piece_no_normal(
        size_t index,
        std::shared_ptr<Mesh<PTN>> mesh,
        std::shared_ptr<gl::Texture> diffuse_texture,
        std::shared_ptr<gl::Buffer> vertex_buffer,
        std::shared_ptr<gl::IndexBuffer> index_buffer) {
    auto& data = app->user_data<Data>();

    const identifier::Id id = identifier::generate_id();
    data.piece_ids[index] = id;

    auto id_buffer = create_id_buffer(
        mesh->vertices.size(), id,
        hs {"piece_id" + std::to_string(index)}
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);

    BufferLayout layout2;
    layout2.add(3, BufferLayout::Float, 1);

    auto vertex_array = app->res.vertex_array.load(hs {"piece" + std::to_string(index)});
    vertex_array->add_buffer(vertex_buffer, layout);
    vertex_array->add_buffer(id_buffer, layout2);
    vertex_array->add_index_buffer(index_buffer);
    gl::VertexArray::unbind();

    auto material_instance = app->res.material_instance.load(
        hs {"piece" + std::to_string(index)},
        app->res.material["tinted_wood"_h]
    );
    material_instance->set_texture("u_material.diffuse", diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess", 4.0f);
    material_instance->set_vec3("u_material.tint", DEFAULT_TINT);
}

void StandardGameScene::initialize_keyboard_controls() {
    auto keyboard_controls = app->res.quad.load("keyboard_controls"_h);

    gl::TextureSpecification specification;
    specification.min_filter = gl::Filter::Linear;
    specification.mag_filter = gl::Filter::Linear;

    auto texture = app->res.texture.load(
        "keyboard_controls_default"_h,
        app->res.texture_data["keyboard_controls_default"_h],
        specification
    );
    app->res.texture.load(
        "keyboard_controls_cross"_h,
        app->res.texture_data["keyboard_controls_cross"_h],
        specification
    );

    keyboard_controls->texture = texture;
}

void StandardGameScene::initialize_light_bulb() {
    auto light_bulb = app->res.quad.load("light_bulb"_h);

    gl::TextureSpecification specification;
    specification.min_filter = gl::Filter::Linear;
    specification.mag_filter = gl::Filter::Linear;

    auto light_bulb_texture = app->res.texture.load(
        "light_bulb"_h,
        "data/textures/light_bulb/light_bulb.png",
        specification
    );

    light_bulb->texture = light_bulb_texture;
}

void StandardGameScene::setup_and_add_model_board() {
    board.model->vertex_array = app->res.vertex_array["board_wood"_h];
    board.model->index_buffer = app->res.index_buffer["board_wood"_h];
    board.model->scale = WORLD_SCALE;
    board.model->material = app->res.material_instance["board_wood"_h];
    board.model->cast_shadow = true;

    app->renderer->add_model(board.model);

    DEB_DEBUG("Setup model board");
}

void StandardGameScene::setup_and_add_model_board_paint() {
    board.paint_model->vertex_array = app->res.vertex_array["board_paint"_h];
    board.paint_model->index_buffer = app->res.index_buffer["board_paint"_h];
    board.paint_model->position = glm::vec3(0.0f, PAINT_Y_POSITION, 0.0f);
    board.paint_model->scale = WORLD_SCALE;
    board.paint_model->material = app->res.material_instance["board_paint"_h];

    app->renderer->add_model(board.paint_model);

    DEB_DEBUG("Setup model board paint");
}

void StandardGameScene::setup_and_add_model_pieces() {
    for (size_t i = 0; i < 9; i++) {
        setup_and_add_model_piece(
            i, WHITE_PIECE_POSITION(i),
            app->res.index_buffer["white_piece"_h]
        );
    }

    for (size_t i = 9; i < 18; i++) {
        setup_and_add_model_piece(
            i, BLACK_PIECE_POSITION(i),
            app->res.index_buffer["black_piece"_h]
        );
    }
}

void StandardGameScene::setup_and_add_model_piece(size_t index, const glm::vec3& position,
        std::shared_ptr<gl::IndexBuffer> index_buffer) {
    auto& data = app->user_data<Data>();

    Piece& piece = board.pieces.at(index);

    piece.model->position = position;
    piece.model->rotation = RANDOM_PIECE_ROTATION();
    piece.model->vertex_array = app->res.vertex_array[hs {"piece" + std::to_string(index)}];
    piece.model->index_buffer = index_buffer;
    piece.model->scale = WORLD_SCALE;
    piece.model->material = app->res.material_instance[hs {"piece" + std::to_string(index)}];
    piece.model->outline_color = std::make_optional<glm::vec3>(1.0f);
    piece.model->id = std::make_optional<identifier::Id>(data.piece_ids[index]);
    piece.model->cast_shadow = true;

    app->renderer->add_model(piece.model);

    piece.source->set_position(position);
    piece.source->set_reference_distance(6.0f);

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

    node.model->vertex_array = app->res.vertex_array[hs {"node" + std::to_string(index)}];
    node.model->index_buffer = app->res.index_buffer["node"_h];
    node.model->position = position;
    node.model->scale = WORLD_SCALE;
    node.model->material = app->res.material_instance[hs {"node" + std::to_string(index)}];
    node.model->id = std::make_optional<identifier::Id>(data.node_ids[index]);

    app->renderer->add_model(node.model);

    DEB_DEBUG("Setup model node {}", index);
}

void StandardGameScene::setup_entities() {
    board = StandardBoard {};
    board.model = app->res.model.load("board"_h);
    board.paint_model = app->res.model.load("board_paint"_h);

    for (size_t i = 0; i < 9; i++) {
        board.pieces[i] = Piece {
            i, PieceType::White,
            app->res.model.load(hs {"piece" + std::to_string(i)}),
            app->res.al_source.load(hs {"piece" + std::to_string(i)})
        };
    }

    for (size_t i = 9; i < 18; i++) {
        board.pieces[i] = Piece {
            i, PieceType::Black,
            app->res.model.load(hs {"piece" + std::to_string(i)}),
            app->res.al_source.load(hs {"piece" + std::to_string(i)})
        };
    }

    for (size_t i = 0; i < 24; i++) {
        board.nodes[i] = Node {
            i, app->res.model.load(hs {"node" + std::to_string(i)})
        };
    }

    DEB_DEBUG("Setup entities");
}

void StandardGameScene::initialize_skybox() {
    if (app->user_data<Data>().options.skybox == game_options::NONE) {
        DEB_DEBUG("Initialized skybox");
        return;
    }

    const std::array<std::shared_ptr<TextureData>, 6> data = {
        app->res.texture_data["skybox_px"_h],
        app->res.texture_data["skybox_nx"_h],
        app->res.texture_data["skybox_py"_h],
        app->res.texture_data["skybox_ny"_h],
        app->res.texture_data["skybox_pz"_h],
        app->res.texture_data["skybox_nz"_h]
    };

    auto texture = app->res.texture_3d.force_load("skybox"_h, data);
    app->renderer->set_skybox(texture);

    DEB_DEBUG("Initialized skybox");
}

void StandardGameScene::initialize_light() {
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
        app->res.quad["light_bulb"_h]->position = LIGHT_FIELD.position;
    } else if (data.options.skybox == game_options::AUTUMN) {
        app->res.quad["light_bulb"_h]->position = LIGHT_AUTUMN.position;
    } else if (data.options.skybox == game_options::NONE) {
        app->res.quad["light_bulb"_h]->position = LIGHT_NONE.position;
    }
#endif

    DEB_DEBUG("Initialized light");
}

void StandardGameScene::setup_camera() {
    auto& data = app->user_data<Data>();

    constexpr float PITCH = 47.0f;
    constexpr float DISTANCE_TO_POINT = 8.0f;

    camera = Camera {};

    camera_controller = PointCameraController {
        &camera,
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

    default_camera_position = camera_controller.get_position();

    camera_controller = PointCameraController {
        &camera,
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

    app->renderer->set_camera_controller(&camera_controller);
    app->openal->get_listener().set_position(camera_controller.get_position());
    app->openal->get_listener().set_look_at_and_up(
        camera_controller.get_point() - camera_controller.get_position(),
        glm::rotate(UP_VECTOR, camera_controller.get_rotation().y, UP_VECTOR)
    );

    DEB_DEBUG("Setup camera");
}

void StandardGameScene::initialize_indicators_textures() {
    gl::TextureSpecification specification;
    specification.min_filter = gl::Filter::Linear;
    specification.mag_filter = gl::Filter::Linear;

    app->res.texture.load(
        "white_indicator"_h,
        app->res.texture_data["white_indicator"_h],
        specification
    );
    app->res.texture.load(
        "black_indicator"_h,
        app->res.texture_data["black_indicator"_h],
        specification
    );
    app->res.texture.load(
        "wait_indicator"_h,
        app->res.texture_data["wait_indicator"_h],
        specification
    );
    app->res.texture.load(
        "computer_thinking_indicator"_h,
        app->res.texture_data["computer_thinking_indicator"_h],
        specification
    );
}

void StandardGameScene::setup_widgets() {
    auto& data = app->user_data<Data>();

    constexpr int LOWEST_RESOLUTION = 288;
    constexpr int HIGHEST_RESOLUTION = 1035;

    auto turn_indicator = app->res.image.load(
        "turn_indicator"_h, app->res.texture["white_indicator"_h]
    );
    turn_indicator->stick(gui::Sticky::SE);
    turn_indicator->offset(30, gui::Relative::Right);
    turn_indicator->offset(30, gui::Relative::Bottom);
    turn_indicator->scale(0.4f, 1.0f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);
    app->gui_renderer->add_widget(turn_indicator);

    auto timer_text = app->res.text.load(
        "timer_text"_h,
        app->res.font["good_dog_plain"_h],
        "00:00", 1.5f, glm::vec3(0.9f)
    );
    timer_text->stick(gui::Sticky::N);
    timer_text->offset(60, gui::Relative::Top);
    timer_text->scale(0.6f, 1.4f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);
    timer_text->set_shadows(true);

    if (!data.options.hide_timer) {
        app->gui_renderer->add_widget(timer_text);
    }

    auto wait_indicator = app->res.image.load(
        "wait_indicator"_h, app->res.texture["wait_indicator"_h]
    );
    wait_indicator->stick(gui::Sticky::NE);
    wait_indicator->offset(25, gui::Relative::Right);
    wait_indicator->offset(55, gui::Relative::Top);
    wait_indicator->scale(0.4f, 1.0f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);

    auto computer_thinking_indicator = app->res.image.load(
        "computer_thinking_indicator"_h, app->res.texture["computer_thinking_indicator"_h]
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
    char time[32];
    timer.get_time_formatted(time);
    app->res.text["timer_text"_h]->set_text(time);
}

void StandardGameScene::update_turn_indicator() {
    if (board.turn == BoardPlayer::White) {
        app->res.image["turn_indicator"_h]->set_image(app->res.texture["white_indicator"_h]);
    } else {
        app->res.image["turn_indicator"_h]->set_image(app->res.texture["black_indicator"_h]);
    }
}

void StandardGameScene::update_wait_indicator() {
    if (!board.next_move) {
        if (!show_wait_indicator) {
            app->gui_renderer->add_widget(app->res.image["wait_indicator"_h]);
            show_wait_indicator = true;
        }
    } else {
        if (show_wait_indicator) {
            app->gui_renderer->remove_widget(app->res.image["wait_indicator"_h]);
            show_wait_indicator = false;
        }
    }
}

void StandardGameScene::update_computer_thinking_indicator() {
    if (game.state == GameState::ComputerThinkingMove) {
        if (!show_computer_thinking_indicator) {
            app->gui_renderer->add_widget(app->res.image["computer_thinking_indicator"_h]);
            show_computer_thinking_indicator = true;
        }
    } else {
        if (show_computer_thinking_indicator) {
            app->gui_renderer->remove_widget(app->res.image["computer_thinking_indicator"_h]);
            show_computer_thinking_indicator = false;
        }
    }
}

void StandardGameScene::update_cursor() {
    auto& data = app->user_data<Data>();

    if (data.options.custom_cursor) {
        if (board.must_take_piece) {
            app->window->set_cursor(data.cross_cursor);

            app->res.quad["keyboard_controls"_h]->texture = app->res.texture["keyboard_controls_cross"_h];
        } else {
            app->window->set_cursor(data.arrow_cursor);

            app->res.quad["keyboard_controls"_h]->texture = app->res.texture["keyboard_controls_default"_h];
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

    auto& data = app->user_data<Data>();

    skybox_loader->start_loading_thread(data.launcher_options.texture_quality, data.options.skybox);
}

void StandardGameScene::change_skybox() {
    const std::array<std::shared_ptr<TextureData>, 6> data = {
        app->res.texture_data["skybox_px"_h],
        app->res.texture_data["skybox_nx"_h],
        app->res.texture_data["skybox_py"_h],
        app->res.texture_data["skybox_ny"_h],
        app->res.texture_data["skybox_pz"_h],
        app->res.texture_data["skybox_nz"_h]
    };

    auto texture = app->res.texture_3d.force_load("skybox"_h, data);
    app->renderer->set_skybox(texture);

    initialize_light();

    app->res.texture_data.release("skybox"_h);
}

void StandardGameScene::set_board_paint_texture() {
    auto& data = app->user_data<Data>();

    board_paint_texture_loader->start_loading_thread(
        data.launcher_options.texture_quality,
        data.options.labeled_board
    );
}

void StandardGameScene::change_board_paint_texture() {
    auto& data = app->user_data<Data>();

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmapping = true;
    specification.bias = -1.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.force_load(
        "board_paint_diffuse"_h,
        app->res.texture_data["board_paint_diffuse"_h],
        specification
    );

    app->res.material_instance["board_paint"_h]->set_texture("u_material.diffuse", diffuse_texture, 0);

    app->res.texture_data.release("board_paint_diffuse"_h);
}

void StandardGameScene::save_game() {
    board.finalize_pieces_state();

    StandardBoardSerialized serialized;
    board.to_serialized(serialized);

    save_load::SavedGame<StandardBoardSerialized> saved_game;
    saved_game.board_serialized = serialized;
    saved_game.camera_controller = camera_controller;
    saved_game.time = timer.get_time();

    time_t current;
    time(&current);
    saved_game.date = ctime(&current);

    saved_game.undo_redo_state = undo_redo_state;
    saved_game.white_player = game.white_player;
    saved_game.black_player = game.black_player;

    try {
        save_load::save_game_to_file(saved_game);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_ERROR("Could not save game: {}", e.what());

        save_load::handle_save_file_not_open_error(app->data().application_name);
    } catch (const save_load::SaveFileError& e) {
        REL_ERROR("Could not save game: {}", e.what());
    }
}

void StandardGameScene::load_game() {
    board.finalize_pieces_state();

    save_load::SavedGame<StandardBoardSerialized> saved_game;

    try {
        save_load::load_game_from_file(saved_game);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_WARN("Could not load game: {}", e.what());

        save_load::handle_save_file_not_open_error(app->data().application_name);

        imgui_layer.show_could_not_load_game = true;
        return;
    } catch (const save_load::SaveFileError& e) {
        REL_WARN("Could not load game: {}", e.what());  // TODO maybe delete file

        imgui_layer.show_could_not_load_game = true;
        return;
    }

    board.from_serialized(saved_game.board_serialized);
    camera_controller = saved_game.camera_controller;
    timer = Timer {app, saved_game.time};
    undo_redo_state = std::move(saved_game.undo_redo_state);
    game.white_player = saved_game.white_player;
    game.black_player = saved_game.black_player;

    // Set camera pointer lost in serialization
    camera_controller.set_camera(&camera);

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

    State current_state = { serialized, camera_controller };
    const State& previous_state = undo_redo_state.undo.back();

    board.from_serialized(previous_state.board_serialized);
    camera_controller = previous_state.camera_controller;

    undo_redo_state.undo.pop_back();
    undo_redo_state.redo.push_back(current_state);

    DEB_DEBUG("Undid move; popped from undo stack and pushed onto redo stack");

    game.state = GameState::MaybeNextPlayer;
    made_first_move = board.not_placed_white_pieces_count + board.not_placed_black_pieces_count != 18;

    if (undo_game_over) {
        timer.start();
    }

    update_cursor();
    update_turn_indicator();
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

    State current_state = { serialized, camera_controller };
    const State& previous_state = undo_redo_state.redo.back();

    board.from_serialized(previous_state.board_serialized);
    camera_controller = previous_state.camera_controller;

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
    update_turn_indicator();
}
