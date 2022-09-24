#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/scenes/standard_game_scene.h"
#include "game/scenes/imgui_layer.h"
#include "game/assets.h"
#include "other/data.h"

using namespace encrypt;

void StandardGameScene::on_start() {
    imgui_layer.update();

    DEB_INFO("Entered game scene");
}

void StandardGameScene::on_stop() {
    imgui_layer.reset();
}

void StandardGameScene::on_awake() {
    imgui_layer = ImGuiLayer<StandardGameScene> {app, this};

    // It's ok to be called multiple times
    LOG_TOTAL_GPU_MEMORY_ALLOCATED();
}

void StandardGameScene::on_update() {

}

void StandardGameScene::on_fixed_update() {

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
    } else if (board.get_phase() == BoardPhase::GameOver) {
        imgui_layer.draw_game_over();
    }

    if (undo_redo_state.undo.size() > 0) {
        imgui_layer.can_undo = true;
    }

    if (undo_redo_state.redo.size() > 0) {
        imgui_layer.can_redo = true;
    }

    if (imgui_layer.show_info && !imgui_layer.show_about) {
        ImGui::PushFont(app->user_data<Data>().imgui_info_font);
        ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoCollapse);
        ImGui::Text("FPS: %f", app->get_fps());
        ImGui::Text("OpenGL: %s", debug_opengl::get_opengl_version());
        ImGui::Text("GLSL: %s", debug_opengl::get_glsl_version());
        ImGui::Text("Vendor: %s", debug_opengl::get_vendor());
        ImGui::Text("Renderer: %s", debug_opengl::get_renderer());
        ImGui::End();
        ImGui::PopFont();
    }

#ifdef PLATFORM_GAME_DEBUG
    imgui_layer.draw_debug();
#endif
}

void StandardGameScene::on_mouse_button_pressed(const MouseButtonPressedEvent& event) {
    if (event.button == input::MouseButton::LEFT) {
        if (board.get_next_move()) {
            // board.press(app->renderer->get_hovered_id());  // FIXME make these calls actually do the thing
        }
    }
}

void StandardGameScene::on_mouse_button_released(const MouseButtonReleasedEvent& event) {
    if (event.button == input::MouseButton::LEFT) {
        if (board.get_next_move() && board.get_is_players_turn()) {
            bool did = false;

            if (board.get_phase() == BoardPhase::PlacePieces) {
                if (board.player_must_take_piece()) {
                    // did = board.take_piece(app->renderer->get_hovered_id());  // FIXME
                } else {
                    // did = board.place_piece(app->renderer->get_hovered_id());  // FIXME
                }
            } else if (board.get_phase() == BoardPhase::MovePieces) {
                if (board.player_must_take_piece()) {
                    // did = board.take_piece(app->renderer->get_hovered_id());  // FIXME
                } else {
                    // board.select_piece(app->renderer->get_hovered_id());  // FIXME
                    // did = board.put_down_piece(app->renderer->get_hovered_id());  // FIXME
                }
            }

            if (did) {
                game.state = GameState::HumanDoingMove;
            }

            if (did && !first_move && !timer.is_running()) {
                timer.start(app->window->get_time());
                first_move = true;
            }

            if (board.get_phase() == BoardPhase::GameOver) {
                timer.stop();
            }

            if (undo_redo_state.redo.empty()) {
                imgui_layer.can_redo = false;
            }
        }

        // board.release();  // FIXME

        if (show_keyboard_controls) {
            app->renderer->remove_quad(keyboard.quad);
            show_keyboard_controls = false;
        }
    }
}

void StandardGameScene::on_key_pressed(const KeyPressedEvent& event) {
    switch (event.key) {
        case input::Key::UP:
        case input::Key::DOWN:
        case input::Key::LEFT:
        case input::Key::RIGHT:
        case input::Key::ENTER:
            if (!show_keyboard_controls) {
                app->renderer->add_quad(keyboard.quad);
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
            if (board.get_next_move() && board.get_is_players_turn()) {
                const bool did = keyboard.press(first_move);

                if (did) {
                    game.state = GameState::HumanDoingMove;
                }

                if (did && !first_move && !timer.is_running()) {
                    timer.start(app->window->get_time());
                    first_move = true;
                }

                if (board.get_phase() == BoardPhase::GameOver) {
                    timer.stop();
                }

                if (undo_redo_state.redo.empty()) {
                    imgui_layer.can_redo = false;
                }
            }
            // board.release();  // FIXME
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

    auto vertices = app->res.buffer.load(
        "board_wood_mesh_vertices"_h,
        app->res.mesh_ptnt["board_wood_mesh"_h]->vertices.data(),
        app->res.mesh_ptnt["board_wood_mesh"_h]->vertices.size() * sizeof(PTNT)
    );

    auto indices = app->res.index_buffer.load(
        "board_wood_mesh_indices"_h,
        app->res.mesh_ptnt["board_wood_mesh"_h]->indices.data(),
        app->res.mesh_ptnt["board_wood_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);
    layout.add(3, BufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("board_wood_vertex_array"_h);
    vertex_array->add_buffer(vertices, layout);
    vertex_array->add_index_buffer(indices);

    VertexArray::unbind();

    TextureSpecification specification;
    specification.mag_filter = Filter::Linear;
    specification.mipmapping = true;
    specification.bias = -2.0f;
    specification.anisotropic_filtering = data.options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.load(
        "board_wood_diffuse_texture"_h,
        app->res.texture_data["board_wood_diff_texture"_h], specification
    );

    auto normal_texture = app->res.texture.load(
        "board_normal_texture"_h,
        app->res.texture_data["board_norm_texture"_h], specification
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

    auto vertices = app->res.buffer.load(
        "board_paint_mesh_vertices"_h,
        app->res.mesh_ptnt["board_paint_mesh"_h]->vertices.data(),
        app->res.mesh_ptnt["board_paint_mesh"_h]->vertices.size() * sizeof(PTNT)
    );

    auto indices = app->res.index_buffer.load(
        "board_paint_mesh_indices"_h,
        app->res.mesh_ptnt["board_paint_mesh"_h]->indices.data(),
        app->res.mesh_ptnt["board_paint_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);
    layout.add(3, BufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("board_paint_vertex_array"_h);
    vertex_array->add_buffer(vertices, layout);
    vertex_array->add_index_buffer(indices);

    VertexArray::unbind();

    TextureSpecification specification;
    specification.mag_filter = Filter::Linear;
    specification.mipmapping = true;
    specification.bias = -1.0f;
    specification.anisotropic_filtering = data.options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.load(
        "board_paint_diffuse_texture"_h,
        app->res.texture_data["board_paint_diff_texture"_h], specification
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
    specification.anisotropic_filtering = data.options.anisotropic_filtering;

    auto white_piece_diffuse_texture = app->res.texture.load(
        "white_piece_diffuse_texture"_h,
        app->res.texture_data["white_piece_diff_texture"_h],
        specification
    );

    auto black_piece_diffuse_texture = app->res.texture.load(
        "black_piece_diffuse_texture"_h,
        app->res.texture_data["black_piece_diff_texture"_h],
        specification
    );

    auto piece_normal_texture = app->res.texture.load(
        "piece_normal_texture"_h,
        app->res.texture_data["piece_norm_texture"_h],
        specification
    );

    auto material = app->res.material.load("tinted_wood_material"_h, shader);
    material->add_texture("u_material.diffuse");
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular");
    material->add_uniform(Material::Uniform::Float, "u_material.shininess");
    material->add_texture("u_material.normal");
    material->add_uniform(Material::Uniform::Vec3, "u_material.tint");

    auto white_piece_vertices = app->res.buffer.load(
        "white_piece_vertices"_h,
        app->res.mesh_ptnt["white_piece_mesh"_h]->vertices.data(),
        app->res.mesh_ptnt["white_piece_mesh"_h]->vertices.size() * sizeof(PTNT)
    );

    auto white_piece_indices = app->res.index_buffer.load(
        "white_piece_indices"_h,
        app->res.mesh_ptnt["white_piece_mesh"_h]->indices.data(),
        app->res.mesh_ptnt["white_piece_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    auto black_piece_vertices = app->res.buffer.load(
        "white_piece_vertices"_h,
        app->res.mesh_ptnt["black_piece_mesh"_h]->vertices.data(),
        app->res.mesh_ptnt["black_piece_mesh"_h]->vertices.size() * sizeof(PTNT)
    );

    auto black_piece_indices = app->res.index_buffer.load(
        "black_piece_indices"_h,
        app->res.mesh_ptnt["black_piece_mesh"_h]->indices.data(),
        app->res.mesh_ptnt["black_piece_mesh"_h]->indices.size() * sizeof(unsigned int)
    );

    for (size_t i = 0; i < 9; i++) {
        initialize_rendering_piece(
            i, PieceType::White, app->res.mesh_ptnt["white_piece_mesh"_h],
            white_piece_diffuse_texture, white_piece_vertices, white_piece_indices
        );
    }
    for (size_t i = 9; i < 18; i++) {
        initialize_rendering_piece(
            i, PieceType::Black, app->res.mesh_ptnt["black_piece_mesh"_h],
            black_piece_diffuse_texture, black_piece_vertices, black_piece_indices
        );
    }
}

void StandardGameScene::initialize_rendering_piece(
        size_t index, PieceType type,
        std::shared_ptr<Mesh<PTNT>> mesh,
        std::shared_ptr<Texture> diffuse_texture,
        std::shared_ptr<Buffer> vertices,
        std::shared_ptr<IndexBuffer> indices) {
    const hover::Id id = hover::generate_id();
    // app->data.pieces_id[index] = id;

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
    vertex_array->add_buffer(vertices, layout);
    vertex_array->add_buffer(id_buffer, layout2);
    vertex_array->add_index_buffer(indices);

    VertexArray::unbind();

    auto piece_material_instance = app->res.material_instance.load(
        hs {"piece_material_instance" + std::to_string(index)},
        app->res.material["tinted_wood_material"_h]
    );

    piece_material_instance->set_texture("u_material.diffuse", diffuse_texture, 0);
    piece_material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    piece_material_instance->set_float("u_material.shininess", 4.0f);
    piece_material_instance->set_texture("u_material.normal", app->res.texture["piece_normal_texture"_h], 1);
    piece_material_instance->set_vec3("u_material.tint", glm::vec3(1.0f));
}
