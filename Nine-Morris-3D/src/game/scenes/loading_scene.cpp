#include <engine/engine_application.h>
#include <engine/engine_audio.h>
#include <engine/engine_graphics.h>
#include <engine/engine_other.h>

#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "game/scenes/loading_scene.h"
#include "game/assets_load.h"
#include "game/assets.h"
#include "game/point_camera_controller.h"
#include "game/undo_redo_state.h"
#include "game/save_load.h"
#include "game/timer.h"
#include "launcher/launcher_options.h"
#include "other/data.h"
#include "other/constants.h"

void LoadingScene::on_start() {
    auto& data = app->user_data<Data>();

    load_splash_screen_texture();

    loader = std::make_unique<assets_load::AllStartLoader>(
        [this, &data]() {
            app->change_scene(scene_int_to_id(data.options.scene));
        }
    );

    loader->start_loading_thread(
        data.launcher_options.normal_mapping,
        data.launcher_options.texture_quality,
        data.options.labeled_board,
        data.options.skybox
    );

    setup_widgets();

    auto background = objects.add<gui::Image>("background"_H, app->res.texture["splash_screen"_H]);
    scene_list.add(background);

    loading_animation.previous_seconds = app->window->get_time();
}

void LoadingScene::on_stop() {
    DEB_INFO("Done loading assets; initializing the rest of the game...");

    if (app->running) {
        // Stop initializing, if the user closed the window
        initialize_game();
    }

    loader.reset();
}

void LoadingScene::on_update() {
    float width, height, x_pos, y_pos;
    app->gui_renderer->quad_center(width, height, x_pos, y_pos);

    auto background = objects.get<gui::Image>("background"_H);
    background->set_position(glm::vec2(x_pos, y_pos));
    background->set_size(glm::vec2(width, height));

    update_loading_animation();

    loader->update(app);
}

void LoadingScene::setup_widgets() {
    static constexpr int LOWEST_RESOLUTION = 288;
    static constexpr int HIGHEST_RESOLUTION = 1035;

    auto loading_text = objects.add<gui::Text>(
        "loading_text"_H,
        app->res.font["good_dog_plain"_H],
        "Loading",
        1.5f,
        glm::vec3(0.75f)
    );
    loading_text->stick(gui::Sticky::SE);
    loading_text->offset(20, gui::Relative::Right);
    loading_text->offset(20, gui::Relative::Bottom);
    loading_text->scale(0.4f, 1.3f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);

    const auto size = loading_text->get_actual_size();
    loading_text->fake_size(glm::vec2(size.x + 40.0f, size.y));
    loading_text->set_shadows(true);

    scene_list.add(loading_text);
}

void LoadingScene::load_splash_screen_texture() {
    using namespace assets;
    using namespace encrypt;
    using namespace file_system;

    gl::TextureSpecification specification;

    app->res.texture.load("splash_screen"_H, encr(path_for_assets(SPLASH_SCREEN_TEXTURE)), specification);
}

void LoadingScene::update_loading_animation() {
    const double current_seconds = app->window->get_time();
    const double elapsed_seconds = current_seconds - loading_animation.previous_seconds;
    loading_animation.previous_seconds = current_seconds;

    loading_animation.total_time += elapsed_seconds;

    if (loading_animation.total_time > 0.4) {
        loading_animation.total_time = 0.0;

        std::string text = "Loading";
        text.append(loading_animation.dots, '.');

        objects.get<gui::Text>("loading_text"_H)->set_text(text);

        loading_animation.dots++;
        loading_animation.dots %= 4;
    }
}

hs LoadingScene::scene_int_to_id(int scene) {  // FIXME find a better way
    switch (scene) {
        case 0:
            return "standard_game"_H;
        case 1:
            return "jump_variant"_H;
        case 2:
            return "jump_plus_variant"_H;
        default:
            REL_CRITICAL("Invalid scene number, exiting...");
            application_exit::panic();
    }

    return {};
}

void LoadingScene::initialize_board() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_wood"_H,
        encrypt::encr(file_system::path_for_assets(assets::BOARD_VERTEX_SHADER)),
        encrypt::encr(file_system::path_for_assets(assets::BOARD_FRAGMENT_SHADER)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
            "u_material.normal"
        },
        std::initializer_list {
            app->renderer->get_storage().projection_view_uniform_block,
            app->renderer->get_storage().light_view_uniform_block,
            app->renderer->get_storage().light_space_uniform_block,
            app->renderer->get_storage().light_uniform_block
        }
    );

    auto vertex_buffer = app->res.vertex_buffer.load(
        "board_wood"_H,
        app->res.mesh["board_wood"_H]->get_vertices(),
        app->res.mesh["board_wood"_H]->get_vertices_size()
    );

    auto index_buffer = app->res.index_buffer.load(
        "board_wood"_H,
        app->res.mesh["board_wood"_H]->get_indices(),
        app->res.mesh["board_wood"_H]->get_indices_size()
    );

    VertexBufferLayout layout = VertexBufferLayout {}
        .add(0, VertexBufferLayout::Float, 3)
        .add(1, VertexBufferLayout::Float, 2)
        .add(2, VertexBufferLayout::Float, 3)
        .add(3, VertexBufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("board_wood"_H);
    vertex_array->begin_definition()
        .add_buffer(vertex_buffer, layout)
        .add_index_buffer(index_buffer)
        .end_definition();

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -2.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.load(
        "board_wood_diffuse"_H,
        app->res.texture_data["board_wood_diffuse"_H],
        specification
    );

    auto normal_texture = app->res.texture.load(
        "board_normal"_H,
        app->res.texture_data["board_normal"_H],
        specification
    );

    auto material = app->res.material.load("wood"_H, shader);
    material->add_texture("u_material.diffuse"_H);
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
    material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);
    material->add_texture("u_material.normal"_H);

    auto material_instance = app->res.material_instance.load("board_wood"_H, material);
    material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 4.0f);
    material_instance->set_texture("u_material.normal"_H, normal_texture, 1);
}

void LoadingScene::initialize_board_paint() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_paint"_H,
        encrypt::encr(file_system::path_for_assets(assets::BOARD_PAINT_VERTEX_SHADER)),
        encrypt::encr(file_system::path_for_assets(assets::BOARD_PAINT_FRAGMENT_SHADER)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
            "u_material.normal"
        },
        std::initializer_list {
            app->renderer->get_storage().projection_view_uniform_block,
            app->renderer->get_storage().light_view_uniform_block,
            app->renderer->get_storage().light_space_uniform_block,
            app->renderer->get_storage().light_uniform_block
        }
    );

    auto vertex_buffer = app->res.vertex_buffer.load(
        "board_paint"_H,
        app->res.mesh["board_paint"_H]->get_vertices(),
        app->res.mesh["board_paint"_H]->get_vertices_size()
    );

    auto index_buffer = app->res.index_buffer.load(
        "board_paint"_H,
        app->res.mesh["board_paint"_H]->get_indices(),
        app->res.mesh["board_paint"_H]->get_indices_size()
    );

    VertexBufferLayout layout = VertexBufferLayout {}
        .add(0, VertexBufferLayout::Float, 3)
        .add(1, VertexBufferLayout::Float, 2)
        .add(2, VertexBufferLayout::Float, 3)
        .add(3, VertexBufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("board_paint"_H);
    vertex_array->begin_definition()
        .add_buffer(vertex_buffer, layout)
        .add_index_buffer(index_buffer)
        .end_definition();

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -1.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.load(
        "board_paint_diffuse"_H,
        app->res.texture_data["board_paint_diffuse"_H],
        specification
    );

    auto material = app->res.material.load("board_paint"_H, shader);
    material->add_texture("u_material.diffuse"_H);
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
    material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);
    material->add_texture("u_material.normal"_H);

    auto material_instance = app->res.material_instance.load("board_paint"_H, material);
    material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 4.0f);
    material_instance->set_texture("u_material.normal"_H, app->res.texture["board_normal"_H], 1);
}

void LoadingScene::initialize_pieces() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "piece"_H,
        encrypt::encr(file_system::path_for_assets(assets::PIECE_VERTEX_SHADER)),
        encrypt::encr(file_system::path_for_assets(assets::PIECE_FRAGMENT_SHADER)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
            "u_material.normal",
            "u_material.tint"
        },
        std::initializer_list {
            app->renderer->get_storage().projection_view_uniform_block,
            app->renderer->get_storage().light_view_uniform_block,
            app->renderer->get_storage().light_space_uniform_block,
            app->renderer->get_storage().light_uniform_block,
        }
    );

    auto white_piece_vertex_buffer = app->res.vertex_buffer.load(
        "white_piece"_H,
        app->res.mesh["white_piece"_H]->get_vertices(),
        app->res.mesh["white_piece"_H]->get_vertices_size()
    );

    auto white_piece_index_buffer = app->res.index_buffer.load(
        "white_piece"_H,
        app->res.mesh["white_piece"_H]->get_indices(),
        app->res.mesh["white_piece"_H]->get_indices_size()
    );

    auto black_piece_vertex_buffer = app->res.vertex_buffer.load(
        "black_piece"_H,
        app->res.mesh["black_piece"_H]->get_vertices(),
        app->res.mesh["black_piece"_H]->get_vertices_size()
    );

    auto black_piece_index_buffer = app->res.index_buffer.load(
        "black_piece"_H,
        app->res.mesh["black_piece"_H]->get_indices(),
        app->res.mesh["black_piece"_H]->get_indices_size()
    );

    VertexBufferLayout layout = VertexBufferLayout {}
        .add(0, VertexBufferLayout::Float, 3)
        .add(1, VertexBufferLayout::Float, 2)
        .add(2, VertexBufferLayout::Float, 3)
        .add(3, VertexBufferLayout::Float, 3);

    auto white_piece_vertex_array = app->res.vertex_array.load("white_piece"_H);
    white_piece_vertex_array->begin_definition()
        .add_buffer(white_piece_vertex_buffer, layout)
        .add_index_buffer(white_piece_index_buffer)
        .end_definition();

    auto black_piece_vertex_array = app->res.vertex_array.load("black_piece"_H);
    black_piece_vertex_array->begin_definition()
        .add_buffer(black_piece_vertex_buffer, layout)
        .add_index_buffer(black_piece_index_buffer)
        .end_definition();

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -1.5f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto white_piece_diffuse_texture = app->res.texture.load(
        "white_piece_diffuse"_H,
        app->res.texture_data["white_piece_diffuse"_H],
        specification
    );

    auto black_piece_diffuse_texture = app->res.texture.load(
        "black_piece_diffuse"_H,
        app->res.texture_data["black_piece_diffuse"_H],
        specification
    );

    auto piece_normal_texture = app->res.texture.load(
        "piece_normal"_H,
        app->res.texture_data["piece_normal"_H],
        specification
    );

    auto material = app->res.material.load("tinted_wood"_H, shader);
    material->add_texture("u_material.diffuse"_H);
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
    material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);
    material->add_texture("u_material.normal"_H);
    material->add_uniform(Material::Uniform::Vec3, "u_material.tint"_H);

    app->res.al_buffer.load(
        "piece_place1"_H,
        app->res.sound_data["piece_place1"_H]
    );

    app->res.al_buffer.load(
        "piece_place2"_H,
        app->res.sound_data["piece_place2"_H]
    );

    app->res.al_buffer.load(
        "piece_move1"_H,
        app->res.sound_data["piece_move1"_H]
    );

    app->res.al_buffer.load(
        "piece_move2"_H,
        app->res.sound_data["piece_move2"_H]
    );

    app->res.al_buffer.load(
        "piece_take"_H,
        app->res.sound_data["piece_take"_H]
    );
}

void LoadingScene::initialize_node(size_t index) {
    auto material_instance = app->res.material_instance.load(
        hs("node" + std::to_string(index)),
        app->res.material["basic"_H]
    );
    material_instance->set_vec4("u_color"_H, glm::vec4(0.0f));
}

void LoadingScene::initialize_nodes() {
    auto shader = app->res.shader.load(
        "node"_H,
        encrypt::encr(file_system::path_for_assets(assets::NODE_VERTEX_SHADER)),
        encrypt::encr(file_system::path_for_assets(assets::NODE_FRAGMENT_SHADER)),
        std::vector<std::string> { "u_model_matrix", "u_color" },
        std::initializer_list {
            app->renderer->get_storage().projection_view_uniform_block
        }
    );

    auto vertex_buffer = app->res.vertex_buffer.load(
        "node"_H,
        app->res.mesh["node"_H]->get_vertices(),
        app->res.mesh["node"_H]->get_vertices_size()
    );

    auto index_buffer = app->res.index_buffer.load(
        "node"_H,
        app->res.mesh["node"_H]->get_indices(),
        app->res.mesh["node"_H]->get_indices_size()
    );

    VertexBufferLayout layout = VertexBufferLayout {}
        .add(0, VertexBufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("node"_H);
    vertex_array->begin_definition()
        .add_buffer(vertex_buffer, layout)
        .add_index_buffer(index_buffer)
        .end_definition();

    auto material = app->res.material.load("basic"_H, shader);
    material->add_uniform(Material::Uniform::Vec4, "u_color"_H);

    for (size_t i = 0; i < 24; i++) {
        initialize_node(i);
    }
}

void LoadingScene::initialize_board_no_normal() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_wood"_H,
        encrypt::encr(file_system::path_for_assets(assets::BOARD_VERTEX_SHADER_NO_NORMAL)),
        encrypt::encr(file_system::path_for_assets(assets::BOARD_FRAGMENT_SHADER_NO_NORMAL)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess"
        },
        std::initializer_list {
            app->renderer->get_storage().projection_view_uniform_block,
            app->renderer->get_storage().light_view_uniform_block,
            app->renderer->get_storage().light_space_uniform_block,
            app->renderer->get_storage().light_uniform_block,
        }
    );

    auto vertex_buffer = app->res.vertex_buffer.load(
        "board_wood"_H,
        app->res.mesh["board_wood"_H]->get_vertices(),
        app->res.mesh["board_wood"_H]->get_vertices_size()
    );

    auto index_buffer = app->res.index_buffer.load(
        "board_wood"_H,
        app->res.mesh["board_wood"_H]->get_indices(),
        app->res.mesh["board_wood"_H]->get_indices_size()
    );

    VertexBufferLayout layout = VertexBufferLayout {}
        .add(0, VertexBufferLayout::Float, 3)
        .add(1, VertexBufferLayout::Float, 2)
        .add(2, VertexBufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("board_wood"_H);
    vertex_array->begin_definition()
        .add_buffer(vertex_buffer, layout)
        .add_index_buffer(index_buffer)
        .end_definition();

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -2.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.load(
        "board_wood_diffuse"_H,
        app->res.texture_data["board_wood_diffuse"_H],
        specification
    );

    auto material = app->res.material.load("wood"_H, shader);
    material->add_texture("u_material.diffuse"_H);
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
    material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);

    auto material_instance = app->res.material_instance.load("board_wood"_H, material);
    material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 4.0f);
}

void LoadingScene::initialize_board_paint_no_normal() {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_paint"_H,
        encrypt::encr(file_system::path_for_assets(assets::BOARD_PAINT_VERTEX_SHADER_NO_NORMAL)),
        encrypt::encr(file_system::path_for_assets(assets::BOARD_PAINT_FRAGMENT_SHADER_NO_NORMAL)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess"
        },
        std::initializer_list {
            app->renderer->get_storage().projection_view_uniform_block,
            app->renderer->get_storage().light_view_uniform_block,
            app->renderer->get_storage().light_space_uniform_block,
            app->renderer->get_storage().light_uniform_block
        }
    );

    auto vertex_buffer = app->res.vertex_buffer.load(
        "board_paint"_H,
        app->res.mesh["board_paint"_H]->get_vertices(),
        app->res.mesh["board_paint"_H]->get_vertices_size()
    );

    auto index_buffer = app->res.index_buffer.load(
        "board_paint"_H,
        app->res.mesh["board_paint"_H]->get_indices(),
        app->res.mesh["board_paint"_H]->get_indices_size()
    );

    VertexBufferLayout layout = VertexBufferLayout {}
        .add(0, VertexBufferLayout::Float, 3)
        .add(1, VertexBufferLayout::Float, 2)
        .add(2, VertexBufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("board_paint"_H);
    vertex_array->begin_definition()
        .add_buffer(vertex_buffer, layout)
        .add_index_buffer(index_buffer)
        .end_definition();

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -1.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.load(
        "board_paint_diffuse"_H,
        app->res.texture_data["board_paint_diffuse"_H],
        specification
    );

    auto material = app->res.material.load("board_paint"_H, shader);
    material->add_texture("u_material.diffuse"_H);
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
    material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);

    auto material_instance = app->res.material_instance.load("board_paint"_H, material);
    material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 4.0f);
}

void LoadingScene::initialize_pieces_no_normal() {
    auto& data = app->user_data<Data>();

    // FIXME maybe should give another name
    auto shader = app->res.shader.load(
        "piece"_H,
        encrypt::encr(file_system::path_for_assets(assets::PIECE_VERTEX_SHADER_NO_NORMAL)),
        encrypt::encr(file_system::path_for_assets(assets::PIECE_FRAGMENT_SHADER_NO_NORMAL)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
            "u_material.tint"
        },
        std::initializer_list {
            app->renderer->get_storage().projection_view_uniform_block,
            app->renderer->get_storage().light_view_uniform_block,
            app->renderer->get_storage().light_space_uniform_block,
            app->renderer->get_storage().light_uniform_block
        }
    );

    auto white_piece_vertex_buffer = app->res.vertex_buffer.load(
        "white_piece"_H,
        app->res.mesh["white_piece"_H]->get_vertices(),
        app->res.mesh["white_piece"_H]->get_vertices_size()
    );

    auto white_piece_index_buffer = app->res.index_buffer.load(
        "white_piece"_H,
        app->res.mesh["white_piece"_H]->get_indices(),
        app->res.mesh["white_piece"_H]->get_indices_size()
    );

    auto black_piece_vertex_buffer = app->res.vertex_buffer.load(
        "black_piece"_H,
        app->res.mesh["black_piece"_H]->get_vertices(),
        app->res.mesh["black_piece"_H]->get_vertices_size()
    );

    auto black_piece_index_buffer = app->res.index_buffer.load(
        "black_piece"_H,
        app->res.mesh["black_piece"_H]->get_indices(),
        app->res.mesh["black_piece"_H]->get_indices_size()
    );

    VertexBufferLayout layout = VertexBufferLayout {}
        .add(0, VertexBufferLayout::Float, 3)
        .add(1, VertexBufferLayout::Float, 2)
        .add(2, VertexBufferLayout::Float, 3);

    auto white_piece_vertex_array = app->res.vertex_array.load("white_piece"_H);
    white_piece_vertex_array->begin_definition()
        .add_buffer(white_piece_vertex_buffer, layout)
        .add_index_buffer(white_piece_index_buffer)
        .end_definition();

    auto black_piece_vertex_array = app->res.vertex_array.load("black_piece"_H);
    black_piece_vertex_array->begin_definition()
        .add_buffer(black_piece_vertex_buffer, layout)
        .add_index_buffer(black_piece_index_buffer)
        .end_definition();

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -1.5f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto white_piece_diffuse_texture = app->res.texture.load(
        "white_piece_diffuse"_H,
        app->res.texture_data["white_piece_diffuse"_H],
        specification
    );

    auto black_piece_diffuse_texture = app->res.texture.load(
        "black_piece_diffuse"_H,
        app->res.texture_data["black_piece_diffuse"_H],
        specification
    );

    auto material = app->res.material.load("tinted_wood"_H, shader);
    material->add_texture("u_material.diffuse"_H);
    material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
    material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);
    material->add_uniform(Material::Uniform::Vec3, "u_material.tint"_H);

    app->res.al_buffer.load(
        "piece_place1"_H,
        app->res.sound_data["piece_place1"_H]
    );

    app->res.al_buffer.load(
        "piece_place2"_H,
        app->res.sound_data["piece_place2"_H]
    );

    app->res.al_buffer.load(
        "piece_move1"_H,
        app->res.sound_data["piece_move1"_H]
    );

    app->res.al_buffer.load(
        "piece_move2"_H,
        app->res.sound_data["piece_move2"_H]
    );

    app->res.al_buffer.load(
        "piece_take"_H,
        app->res.sound_data["piece_take"_H]
    );
}

void LoadingScene::initialize_skybox() {
    if (app->user_data<Data>().options.skybox == game_options::NONE) {
        DEB_DEBUG("Initialized skybox");
        return;
    }

    const std::array<std::shared_ptr<TextureData>, 6> data = {
        app->res.texture_data["skybox_px"_H],
        app->res.texture_data["skybox_nx"_H],
        app->res.texture_data["skybox_py"_H],
        app->res.texture_data["skybox_ny"_H],
        app->res.texture_data["skybox_pz"_H],
        app->res.texture_data["skybox_nz"_H]
    };

    auto texture = app->res.texture_3d.force_load("skybox"_H, data);
    app->renderer->set_skybox(texture);

    DEB_DEBUG("Initialized skybox");
}

void LoadingScene::initialize_indicators() {
    gl::TextureSpecification specification;

    app->res.texture.load(
        "white_indicator"_H,
        app->res.texture_data["white_indicator"_H],
        specification
    );
    app->res.texture.load(
        "black_indicator"_H,
        app->res.texture_data["black_indicator"_H],
        specification
    );
    app->res.texture.load(
        "wait_indicator"_H,
        app->res.texture_data["wait_indicator"_H],
        specification
    );
    app->res.texture.load(
        "computer_thinking_indicator"_H,
        app->res.texture_data["computer_thinking_indicator"_H],
        specification
    );
}

void LoadingScene::change_board_paint_texture() {
    auto& data = app->user_data<Data>();

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -1.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.force_load(
        "board_paint_diffuse"_H,
        app->res.texture_data["board_paint_diffuse"_H],
        specification
    );

    app->res.material_instance["board_paint"_H]->set_texture("u_material.diffuse"_H, diffuse_texture, 0);

    app->res.texture_data.release("board_paint_diffuse"_H);
}

void LoadingScene::initialize_ids() {
    auto& data = app->user_data<Data>();

    for (size_t i = 0; i < MAX_NODES; i++) {
        data.node_ids[i] = identifier::generate_id();
    }

    for (size_t i = 0; i < MAX_PIECES; i++) {
        data.piece_ids[i] = identifier::generate_id();
    }
}

void LoadingScene::initialize_keyboard_controls() {
    gl::TextureSpecification specification;

    app->res.texture.load(
        "keyboard_controls_default"_H,
        app->res.texture_data["keyboard_controls_default"_H],
        specification
    );
    app->res.texture.load(
        "keyboard_controls_cross"_H,
        app->res.texture_data["keyboard_controls_cross"_H],
        specification
    );
}

void LoadingScene::initialize_light_bulb() {
    gl::TextureSpecification specification;

    app->res.texture.load(
        "light_bulb"_H,
        "data/textures/light_bulb/light_bulb.png",
        specification
    );
}

void LoadingScene::initialize_light() {
    auto& data = app->user_data<Data>();

    if (data.options.skybox == game_options::FIELD) {
        app->renderer->directional_light = LIGHT_FIELD;
        app->renderer->light_space = SHADOWS_FIELD;
    } else if (data.options.skybox == game_options::AUTUMN) {
        app->renderer->directional_light = LIGHT_AUTUMN;
        app->renderer->light_space = SHADOWS_AUTUMN;
    } else if (data.options.skybox == game_options::NONE) {
        app->renderer->directional_light = LIGHT_NONE;
        app->renderer->light_space = SHADOWS_NONE;
    } else {
        ASSERT(false, "Invalid skybox");
    }

    DEB_DEBUG("Initialized directional_light");
}

void LoadingScene::initialize_game() {
    auto& data = app->user_data<Data>();

    initialize_ids();

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

#ifdef NM3D_PLATFORM_DEBUG
    initialize_light_bulb();
#endif
    initialize_light();
    initialize_skybox();
    initialize_keyboard_controls();
    initialize_indicators();

    auto track = app->res.music_track.load("music"_H, app->res.sound_data["music"_H]);
    data.current_music_track = track;

    if (data.options.enable_music) {
        music::play_music_track(track);
    }
}
