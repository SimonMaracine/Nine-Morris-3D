#include "engine/public/application_base.h"
#include <engine/public/audio.h>
#include <engine/public/graphics.h>
#include <engine/public/other.h>
#include <engine/public/external/resmanager.h>
#include <engine/public/external/glm.h>

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
    auto& data = ctx->data<Data>();

    load_splash_screen_texture();

    loader = std::make_unique<assets_load::AllStartLoader>(
        [this, &data]() {
            ctx->change_scene(scene_int_to_id(data.options.scene));
        }
    );

    loader->start_loading_thread(
        data.launcher_options.normal_mapping,
        data.launcher_options.texture_quality,
        data.options.labeled_board,
        data.options.skybox
    );

    setup_widgets();

    auto background = objects.add<sm::gui::Image>("background"_H, ctx->res.texture["splash_screen"_H]);
    scene_list.add(background);

    loading_animation.previous_seconds = ctx->window->get_time();
}

void LoadingScene::on_stop() {
    LOG_INFO("Done loading assets; initializing the rest of the game...");

    if (ctx->running) {
        // Stop initializing, if the user closed the window
        initialize_game();
    }

    loader.reset();
}

void LoadingScene::on_update() {
    float width, height, x_pos, y_pos;
    ctx->r2d->quad_center(width, height, x_pos, y_pos);

    auto background = objects.get<sm::gui::Image>("background"_H);
    background->set_position(glm::vec2(x_pos, y_pos));
    background->set_size(glm::vec2(width, height));

    update_loading_animation();

    loader->update(ctx);
}

void LoadingScene::setup_widgets() {
    static constexpr int LOWEST_RESOLUTION = 288;
    static constexpr int HIGHEST_RESOLUTION = 1035;

    auto loading_text = objects.add<sm::gui::Text>(
        "loading_text"_H,
        ctx->res.font["good_dog_plain"_H],
        "Loading",
        1.5f,
        glm::vec3(0.75f)
    );
    loading_text->stick(sm::gui::Sticky::SE);
    loading_text->offset(20, sm::gui::Relative::Right);
    loading_text->offset(20, sm::gui::Relative::Bottom);
    loading_text->scale(0.4f, 1.3f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);

    const auto size = loading_text->get_actual_size();
    loading_text->fake_size(glm::vec2(size.x + 40.0f, size.y));
    loading_text->set_shadows(true);

    scene_list.add(loading_text);
}

void LoadingScene::load_splash_screen_texture() {
    using namespace sm::file_system;
    using namespace assets;

    sm::gl::TextureSpecification specification;
    ctx->res.texture.load("splash_screen"_H, sm::Encrypt::encr(path_for_assets(SPLASH_SCREEN_TEXTURE)), specification);
}

void LoadingScene::update_loading_animation() {
    const double current_seconds = ctx->window->get_time();
    const double elapsed_seconds = current_seconds - loading_animation.previous_seconds;
    loading_animation.previous_seconds = current_seconds;

    loading_animation.total_time += elapsed_seconds;

    if (loading_animation.total_time > 0.4) {
        loading_animation.total_time = 0.0;

        std::string text = "Loading";
        text.append(loading_animation.dots, '.');

        objects.get<sm::gui::Text>("loading_text"_H)->set_text(text);

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
            LOG_DIST_CRITICAL("Invalid scene number");
            sm::panic();
    }

    return {};
}

void LoadingScene::initialize_board() {
    auto& data = ctx->data<Data>();

    auto shader = ctx->res.shader.load(
        "board_wood"_H,
        sm::Encrypt::encr(sm::file_system::path_for_assets(assets::BOARD_VERTEX_SHADER)),
        sm::Encrypt::encr(sm::file_system::path_for_assets(assets::BOARD_FRAGMENT_SHADER)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
            "u_material.normal"
        },
        std::initializer_list {
            ctx->r3d->get_storage().projection_view_uniform_block,
            ctx->r3d->get_storage().light_view_uniform_block,
            ctx->r3d->get_storage().light_space_uniform_block,
            ctx->r3d->get_storage().light_uniform_block
        }
    );

    auto vertex_buffer = ctx->res.vertex_buffer.load(
        "board_wood"_H,
        ctx->res.mesh["board_wood"_H]->get_vertices(),
        ctx->res.mesh["board_wood"_H]->get_vertices_size()
    );

    auto index_buffer = ctx->res.index_buffer.load(
        "board_wood"_H,
        ctx->res.mesh["board_wood"_H]->get_indices(),
        ctx->res.mesh["board_wood"_H]->get_indices_size()
    );

    sm::VertexBufferLayout layout = sm::VertexBufferLayout {}
        .add(0, sm::VertexBufferLayout::Float, 3)
        .add(1, sm::VertexBufferLayout::Float, 2)
        .add(2, sm::VertexBufferLayout::Float, 3)
        .add(3, sm::VertexBufferLayout::Float, 3);

    auto vertex_array = ctx->res.vertex_array.load("board_wood"_H);
    vertex_array->begin_definition()
        .add_buffer(vertex_buffer, layout)
        .add_index_buffer(index_buffer)
        .end_definition();

    sm::gl::TextureSpecification specification;
    specification.mag_filter = sm::gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -2.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = ctx->res.texture.load(
        "board_wood_diffuse"_H,
        ctx->res.texture_data["board_wood_diffuse"_H],
        specification
    );

    auto normal_texture = ctx->res.texture.load(
        "board_normal"_H,
        ctx->res.texture_data["board_normal"_H],
        specification
    );

    auto material = ctx->res.material.load("wood"_H, shader);
    material->add_texture("u_material.diffuse"_H);
    material->add_uniform(sm::Material::Uniform::Vec3, "u_material.specular"_H);
    material->add_uniform(sm::Material::Uniform::Float, "u_material.shininess"_H);
    material->add_texture("u_material.normal"_H);

    auto material_instance = ctx->res.material_instance.load("board_wood"_H, material);
    material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 4.0f);
    material_instance->set_texture("u_material.normal"_H, normal_texture, 1);
}

void LoadingScene::initialize_board_paint() {
    auto& data = ctx->data<Data>();

    auto shader = ctx->res.shader.load(
        "board_paint"_H,
        sm::Encrypt::encr(sm::file_system::path_for_assets(assets::BOARD_PAINT_VERTEX_SHADER)),
        sm::Encrypt::encr(sm::file_system::path_for_assets(assets::BOARD_PAINT_FRAGMENT_SHADER)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
            "u_material.normal"
        },
        std::initializer_list {
            ctx->r3d->get_storage().projection_view_uniform_block,
            ctx->r3d->get_storage().light_view_uniform_block,
            ctx->r3d->get_storage().light_space_uniform_block,
            ctx->r3d->get_storage().light_uniform_block
        }
    );

    auto vertex_buffer = ctx->res.vertex_buffer.load(
        "board_paint"_H,
        ctx->res.mesh["board_paint"_H]->get_vertices(),
        ctx->res.mesh["board_paint"_H]->get_vertices_size()
    );

    auto index_buffer = ctx->res.index_buffer.load(
        "board_paint"_H,
        ctx->res.mesh["board_paint"_H]->get_indices(),
        ctx->res.mesh["board_paint"_H]->get_indices_size()
    );

    sm::VertexBufferLayout layout = sm::VertexBufferLayout {}
        .add(0, sm::VertexBufferLayout::Float, 3)
        .add(1, sm::VertexBufferLayout::Float, 2)
        .add(2, sm::VertexBufferLayout::Float, 3)
        .add(3, sm::VertexBufferLayout::Float, 3);

    auto vertex_array = ctx->res.vertex_array.load("board_paint"_H);
    vertex_array->begin_definition()
        .add_buffer(vertex_buffer, layout)
        .add_index_buffer(index_buffer)
        .end_definition();

    sm::gl::TextureSpecification specification;
    specification.mag_filter = sm::gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -1.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = ctx->res.texture.load(
        "board_paint_diffuse"_H,
        ctx->res.texture_data["board_paint_diffuse"_H],
        specification
    );

    auto material = ctx->res.material.load("board_paint"_H, shader);
    material->add_texture("u_material.diffuse"_H);
    material->add_uniform(sm::Material::Uniform::Vec3, "u_material.specular"_H);
    material->add_uniform(sm::Material::Uniform::Float, "u_material.shininess"_H);
    material->add_texture("u_material.normal"_H);

    auto material_instance = ctx->res.material_instance.load("board_paint"_H, material);
    material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 4.0f);
    material_instance->set_texture("u_material.normal"_H, ctx->res.texture["board_normal"_H], 1);
}

void LoadingScene::initialize_pieces() {
    auto& data = ctx->data<Data>();

    auto shader = ctx->res.shader.load(
        "piece"_H,
        sm::Encrypt::encr(sm::file_system::path_for_assets(assets::PIECE_VERTEX_SHADER)),
        sm::Encrypt::encr(sm::file_system::path_for_assets(assets::PIECE_FRAGMENT_SHADER)),
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
            ctx->r3d->get_storage().projection_view_uniform_block,
            ctx->r3d->get_storage().light_view_uniform_block,
            ctx->r3d->get_storage().light_space_uniform_block,
            ctx->r3d->get_storage().light_uniform_block,
        }
    );

    auto white_piece_vertex_buffer = ctx->res.vertex_buffer.load(
        "white_piece"_H,
        ctx->res.mesh["white_piece"_H]->get_vertices(),
        ctx->res.mesh["white_piece"_H]->get_vertices_size()
    );

    auto white_piece_index_buffer = ctx->res.index_buffer.load(
        "white_piece"_H,
        ctx->res.mesh["white_piece"_H]->get_indices(),
        ctx->res.mesh["white_piece"_H]->get_indices_size()
    );

    auto black_piece_vertex_buffer = ctx->res.vertex_buffer.load(
        "black_piece"_H,
        ctx->res.mesh["black_piece"_H]->get_vertices(),
        ctx->res.mesh["black_piece"_H]->get_vertices_size()
    );

    auto black_piece_index_buffer = ctx->res.index_buffer.load(
        "black_piece"_H,
        ctx->res.mesh["black_piece"_H]->get_indices(),
        ctx->res.mesh["black_piece"_H]->get_indices_size()
    );

    sm::VertexBufferLayout layout = sm::VertexBufferLayout {}
        .add(0, sm::VertexBufferLayout::Float, 3)
        .add(1, sm::VertexBufferLayout::Float, 2)
        .add(2, sm::VertexBufferLayout::Float, 3)
        .add(3, sm::VertexBufferLayout::Float, 3);

    auto white_piece_vertex_array = ctx->res.vertex_array.load("white_piece"_H);
    white_piece_vertex_array->begin_definition()
        .add_buffer(white_piece_vertex_buffer, layout)
        .add_index_buffer(white_piece_index_buffer)
        .end_definition();

    auto black_piece_vertex_array = ctx->res.vertex_array.load("black_piece"_H);
    black_piece_vertex_array->begin_definition()
        .add_buffer(black_piece_vertex_buffer, layout)
        .add_index_buffer(black_piece_index_buffer)
        .end_definition();

    sm::gl::TextureSpecification specification;
    specification.mag_filter = sm::gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -1.5f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto white_piece_diffuse_texture = ctx->res.texture.load(
        "white_piece_diffuse"_H,
        ctx->res.texture_data["white_piece_diffuse"_H],
        specification
    );

    auto black_piece_diffuse_texture = ctx->res.texture.load(
        "black_piece_diffuse"_H,
        ctx->res.texture_data["black_piece_diffuse"_H],
        specification
    );

    auto piece_normal_texture = ctx->res.texture.load(
        "piece_normal"_H,
        ctx->res.texture_data["piece_normal"_H],
        specification
    );

    auto material = ctx->res.material.load("tinted_wood"_H, shader);
    material->add_texture("u_material.diffuse"_H);
    material->add_uniform(sm::Material::Uniform::Vec3, "u_material.specular"_H);
    material->add_uniform(sm::Material::Uniform::Float, "u_material.shininess"_H);
    material->add_texture("u_material.normal"_H);
    material->add_uniform(sm::Material::Uniform::Vec3, "u_material.tint"_H);

    ctx->res.al_buffer.load(
        "piece_place1"_H,
        ctx->res.sound_data["piece_place1"_H]
    );

    ctx->res.al_buffer.load(
        "piece_place2"_H,
        ctx->res.sound_data["piece_place2"_H]
    );

    ctx->res.al_buffer.load(
        "piece_move1"_H,
        ctx->res.sound_data["piece_move1"_H]
    );

    ctx->res.al_buffer.load(
        "piece_move2"_H,
        ctx->res.sound_data["piece_move2"_H]
    );

    ctx->res.al_buffer.load(
        "piece_take"_H,
        ctx->res.sound_data["piece_take"_H]
    );
}

void LoadingScene::initialize_node(size_t index) {
    auto material_instance = ctx->res.material_instance.load(
        hs("node" + std::to_string(index)),
        ctx->res.material["basic"_H]
    );
    material_instance->set_vec4("u_color"_H, glm::vec4(0.0f));
}

void LoadingScene::initialize_nodes() {
    auto shader = ctx->res.shader.load(
        "node"_H,
        sm::Encrypt::encr(sm::file_system::path_for_assets(assets::NODE_VERTEX_SHADER)),
        sm::Encrypt::encr(sm::file_system::path_for_assets(assets::NODE_FRAGMENT_SHADER)),
        std::vector<std::string> { "u_model_matrix", "u_color" },
        std::initializer_list {
            ctx->r3d->get_storage().projection_view_uniform_block
        }
    );

    auto vertex_buffer = ctx->res.vertex_buffer.load(
        "node"_H,
        ctx->res.mesh["node"_H]->get_vertices(),
        ctx->res.mesh["node"_H]->get_vertices_size()
    );

    auto index_buffer = ctx->res.index_buffer.load(
        "node"_H,
        ctx->res.mesh["node"_H]->get_indices(),
        ctx->res.mesh["node"_H]->get_indices_size()
    );

    sm::VertexBufferLayout layout = sm::VertexBufferLayout {}
        .add(0, sm::VertexBufferLayout::Float, 3);

    auto vertex_array = ctx->res.vertex_array.load("node"_H);
    vertex_array->begin_definition()
        .add_buffer(vertex_buffer, layout)
        .add_index_buffer(index_buffer)
        .end_definition();

    auto material = ctx->res.material.load("basic"_H, shader);
    material->add_uniform(sm::Material::Uniform::Vec4, "u_color"_H);

    for (size_t i = 0; i < 24; i++) {
        initialize_node(i);
    }
}

void LoadingScene::initialize_board_no_normal() {
    auto& data = ctx->data<Data>();

    auto shader = ctx->res.shader.load(
        "board_wood"_H,
        sm::Encrypt::encr(sm::file_system::path_for_assets(assets::BOARD_VERTEX_SHADER_NO_NORMAL)),
        sm::Encrypt::encr(sm::file_system::path_for_assets(assets::BOARD_FRAGMENT_SHADER_NO_NORMAL)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess"
        },
        std::initializer_list {
            ctx->r3d->get_storage().projection_view_uniform_block,
            ctx->r3d->get_storage().light_view_uniform_block,
            ctx->r3d->get_storage().light_space_uniform_block,
            ctx->r3d->get_storage().light_uniform_block,
        }
    );

    auto vertex_buffer = ctx->res.vertex_buffer.load(
        "board_wood"_H,
        ctx->res.mesh["board_wood"_H]->get_vertices(),
        ctx->res.mesh["board_wood"_H]->get_vertices_size()
    );

    auto index_buffer = ctx->res.index_buffer.load(
        "board_wood"_H,
        ctx->res.mesh["board_wood"_H]->get_indices(),
        ctx->res.mesh["board_wood"_H]->get_indices_size()
    );

    sm::VertexBufferLayout layout = sm::VertexBufferLayout {}
        .add(0, sm::VertexBufferLayout::Float, 3)
        .add(1, sm::VertexBufferLayout::Float, 2)
        .add(2, sm::VertexBufferLayout::Float, 3);

    auto vertex_array = ctx->res.vertex_array.load("board_wood"_H);
    vertex_array->begin_definition()
        .add_buffer(vertex_buffer, layout)
        .add_index_buffer(index_buffer)
        .end_definition();

    sm::gl::TextureSpecification specification;
    specification.mag_filter = sm::gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -2.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = ctx->res.texture.load(
        "board_wood_diffuse"_H,
        ctx->res.texture_data["board_wood_diffuse"_H],
        specification
    );

    auto material = ctx->res.material.load("wood"_H, shader);
    material->add_texture("u_material.diffuse"_H);
    material->add_uniform(sm::Material::Uniform::Vec3, "u_material.specular"_H);
    material->add_uniform(sm::Material::Uniform::Float, "u_material.shininess"_H);

    auto material_instance = ctx->res.material_instance.load("board_wood"_H, material);
    material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 4.0f);
}

void LoadingScene::initialize_board_paint_no_normal() {
    auto& data = ctx->data<Data>();

    auto shader = ctx->res.shader.load(
        "board_paint"_H,
        sm::Encrypt::encr(sm::file_system::path_for_assets(assets::BOARD_PAINT_VERTEX_SHADER_NO_NORMAL)),
        sm::Encrypt::encr(sm::file_system::path_for_assets(assets::BOARD_PAINT_FRAGMENT_SHADER_NO_NORMAL)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess"
        },
        std::initializer_list {
            ctx->r3d->get_storage().projection_view_uniform_block,
            ctx->r3d->get_storage().light_view_uniform_block,
            ctx->r3d->get_storage().light_space_uniform_block,
            ctx->r3d->get_storage().light_uniform_block
        }
    );

    auto vertex_buffer = ctx->res.vertex_buffer.load(
        "board_paint"_H,
        ctx->res.mesh["board_paint"_H]->get_vertices(),
        ctx->res.mesh["board_paint"_H]->get_vertices_size()
    );

    auto index_buffer = ctx->res.index_buffer.load(
        "board_paint"_H,
        ctx->res.mesh["board_paint"_H]->get_indices(),
        ctx->res.mesh["board_paint"_H]->get_indices_size()
    );

    sm::VertexBufferLayout layout = sm::VertexBufferLayout {}
        .add(0, sm::VertexBufferLayout::Float, 3)
        .add(1, sm::VertexBufferLayout::Float, 2)
        .add(2, sm::VertexBufferLayout::Float, 3);

    auto vertex_array = ctx->res.vertex_array.load("board_paint"_H);
    vertex_array->begin_definition()
        .add_buffer(vertex_buffer, layout)
        .add_index_buffer(index_buffer)
        .end_definition();

    sm::gl::TextureSpecification specification;
    specification.mag_filter = sm::gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -1.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = ctx->res.texture.load(
        "board_paint_diffuse"_H,
        ctx->res.texture_data["board_paint_diffuse"_H],
        specification
    );

    auto material = ctx->res.material.load("board_paint"_H, shader);
    material->add_texture("u_material.diffuse"_H);
    material->add_uniform(sm::Material::Uniform::Vec3, "u_material.specular"_H);
    material->add_uniform(sm::Material::Uniform::Float, "u_material.shininess"_H);

    auto material_instance = ctx->res.material_instance.load("board_paint"_H, material);
    material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 4.0f);
}

void LoadingScene::initialize_pieces_no_normal() {
    auto& data = ctx->data<Data>();

    // FIXME maybe should give another name
    auto shader = ctx->res.shader.load(
        "piece"_H,
        sm::Encrypt::encr(sm::file_system::path_for_assets(assets::PIECE_VERTEX_SHADER_NO_NORMAL)),
        sm::Encrypt::encr(sm::file_system::path_for_assets(assets::PIECE_FRAGMENT_SHADER_NO_NORMAL)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess",
            "u_material.tint"
        },
        std::initializer_list {
            ctx->r3d->get_storage().projection_view_uniform_block,
            ctx->r3d->get_storage().light_view_uniform_block,
            ctx->r3d->get_storage().light_space_uniform_block,
            ctx->r3d->get_storage().light_uniform_block
        }
    );

    auto white_piece_vertex_buffer = ctx->res.vertex_buffer.load(
        "white_piece"_H,
        ctx->res.mesh["white_piece"_H]->get_vertices(),
        ctx->res.mesh["white_piece"_H]->get_vertices_size()
    );

    auto white_piece_index_buffer = ctx->res.index_buffer.load(
        "white_piece"_H,
        ctx->res.mesh["white_piece"_H]->get_indices(),
        ctx->res.mesh["white_piece"_H]->get_indices_size()
    );

    auto black_piece_vertex_buffer = ctx->res.vertex_buffer.load(
        "black_piece"_H,
        ctx->res.mesh["black_piece"_H]->get_vertices(),
        ctx->res.mesh["black_piece"_H]->get_vertices_size()
    );

    auto black_piece_index_buffer = ctx->res.index_buffer.load(
        "black_piece"_H,
        ctx->res.mesh["black_piece"_H]->get_indices(),
        ctx->res.mesh["black_piece"_H]->get_indices_size()
    );

    sm::VertexBufferLayout layout = sm::VertexBufferLayout {}
        .add(0, sm::VertexBufferLayout::Float, 3)
        .add(1, sm::VertexBufferLayout::Float, 2)
        .add(2, sm::VertexBufferLayout::Float, 3);

    auto white_piece_vertex_array = ctx->res.vertex_array.load("white_piece"_H);
    white_piece_vertex_array->begin_definition()
        .add_buffer(white_piece_vertex_buffer, layout)
        .add_index_buffer(white_piece_index_buffer)
        .end_definition();

    auto black_piece_vertex_array = ctx->res.vertex_array.load("black_piece"_H);
    black_piece_vertex_array->begin_definition()
        .add_buffer(black_piece_vertex_buffer, layout)
        .add_index_buffer(black_piece_index_buffer)
        .end_definition();

    sm::gl::TextureSpecification specification;
    specification.mag_filter = sm::gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -1.5f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto white_piece_diffuse_texture = ctx->res.texture.load(
        "white_piece_diffuse"_H,
        ctx->res.texture_data["white_piece_diffuse"_H],
        specification
    );

    auto black_piece_diffuse_texture = ctx->res.texture.load(
        "black_piece_diffuse"_H,
        ctx->res.texture_data["black_piece_diffuse"_H],
        specification
    );

    auto material = ctx->res.material.load("tinted_wood"_H, shader);
    material->add_texture("u_material.diffuse"_H);
    material->add_uniform(sm::Material::Uniform::Vec3, "u_material.specular"_H);
    material->add_uniform(sm::Material::Uniform::Float, "u_material.shininess"_H);
    material->add_uniform(sm::Material::Uniform::Vec3, "u_material.tint"_H);

    ctx->res.al_buffer.load(
        "piece_place1"_H,
        ctx->res.sound_data["piece_place1"_H]
    );

    ctx->res.al_buffer.load(
        "piece_place2"_H,
        ctx->res.sound_data["piece_place2"_H]
    );

    ctx->res.al_buffer.load(
        "piece_move1"_H,
        ctx->res.sound_data["piece_move1"_H]
    );

    ctx->res.al_buffer.load(
        "piece_move2"_H,
        ctx->res.sound_data["piece_move2"_H]
    );

    ctx->res.al_buffer.load(
        "piece_take"_H,
        ctx->res.sound_data["piece_take"_H]
    );
}

void LoadingScene::initialize_skybox() {
    if (ctx->data<Data>().options.skybox == game_options::NONE) {
        LOG_DEBUG("Initialized skybox");
        return;
    }

    const std::array<std::shared_ptr<sm::TextureData>, 6> data = {
        ctx->res.texture_data["skybox_px"_H],
        ctx->res.texture_data["skybox_nx"_H],
        ctx->res.texture_data["skybox_py"_H],
        ctx->res.texture_data["skybox_ny"_H],
        ctx->res.texture_data["skybox_pz"_H],
        ctx->res.texture_data["skybox_nz"_H]
    };

    auto texture = ctx->res.texture_3d.force_load("skybox"_H, data);
    ctx->r3d->set_skybox(texture);

    LOG_DEBUG("Initialized skybox");
}

void LoadingScene::initialize_indicators() {
    sm::gl::TextureSpecification specification;

    ctx->res.texture.load(
        "white_indicator"_H,
        ctx->res.texture_data["white_indicator"_H],
        specification
    );
    ctx->res.texture.load(
        "black_indicator"_H,
        ctx->res.texture_data["black_indicator"_H],
        specification
    );
    ctx->res.texture.load(
        "wait_indicator"_H,
        ctx->res.texture_data["wait_indicator"_H],
        specification
    );
    ctx->res.texture.load(
        "computer_thinking_indicator"_H,
        ctx->res.texture_data["computer_thinking_indicator"_H],
        specification
    );
}

void LoadingScene::change_board_paint_texture() {
    auto& data = ctx->data<Data>();

    sm::gl::TextureSpecification specification;
    specification.mag_filter = sm::gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -1.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = ctx->res.texture.force_load(
        "board_paint_diffuse"_H,
        ctx->res.texture_data["board_paint_diffuse"_H],
        specification
    );

    ctx->res.material_instance["board_paint"_H]->set_texture("u_material.diffuse"_H, diffuse_texture, 0);

    ctx->res.texture_data.release("board_paint_diffuse"_H);
}

void LoadingScene::initialize_ids() {
    auto& data = ctx->data<Data>();

    for (size_t i = 0; i < MAX_NODES; i++) {
        data.node_ids[i] = ctx->idt.generate();
    }

    for (size_t i = 0; i < MAX_PIECES; i++) {
        data.piece_ids[i] = ctx->idt.generate();
    }
}

void LoadingScene::initialize_keyboard_controls() {
    sm::gl::TextureSpecification specification;

    ctx->res.texture.load(
        "keyboard_controls_default"_H,
        ctx->res.texture_data["keyboard_controls_default"_H],
        specification
    );
    ctx->res.texture.load(
        "keyboard_controls_cross"_H,
        ctx->res.texture_data["keyboard_controls_cross"_H],
        specification
    );
}

void LoadingScene::initialize_light_bulb() {
    sm::gl::TextureSpecification specification;

    ctx->res.texture.load(
        "light_bulb"_H,
        "data/textures/light_bulb/light_bulb.png",
        specification
    );
}

void LoadingScene::initialize_light() {
    auto& data = ctx->data<Data>();

    if (data.options.skybox == game_options::FIELD) {
        ctx->r3d->directional_light = LIGHT_FIELD;
        ctx->r3d->light_space = SHADOWS_FIELD;
    } else if (data.options.skybox == game_options::AUTUMN) {
        ctx->r3d->directional_light = LIGHT_AUTUMN;
        ctx->r3d->light_space = SHADOWS_AUTUMN;
    } else if (data.options.skybox == game_options::NONE) {
        ctx->r3d->directional_light = LIGHT_NONE;
        ctx->r3d->light_space = SHADOWS_NONE;
    } else {
        ASSERT(false, "Invalid skybox");
    }

    LOG_DEBUG("Initialized directional_light");
}

void LoadingScene::initialize_game() {
    auto& data = ctx->data<Data>();

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

#ifndef NM3D_PLATFORM_DISTRIBUTION
    initialize_light_bulb();
#endif
    initialize_light();
    initialize_skybox();
    initialize_keyboard_controls();
    initialize_indicators();

    auto track = ctx->res.music_track.load("music"_H, ctx->res.sound_data["music"_H]);
    data.current_music_track = track;

    if (data.options.enable_music) {
        sm::music::play_music_track(track);
    }
}
