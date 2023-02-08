#pragma once

#include <engine/engine_application.h>
#include <engine/engine_audio.h>
#include <engine/engine_graphics.h>
#include <engine/engine_other.h>

#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "game/point_camera_controller.h"
#include "game/undo_redo_state.h"
#include "game/save_load.h"
#include "game/timer.h"
#include "game/assets.h"
#include "other/data.h"
#include "other/constants.h"

inline void initialize_board(Application* app) {
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

inline void initialize_board_paint(Application* app) {
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

inline void initialize_pieces(Application* app) {
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

inline void initialize_piece(Application* app, Index index, std::shared_ptr<gl::Texture> diffuse_texture) {
    auto material_instance = app->res.material_instance.load(
        hs("piece" + std::to_string(index)),
        app->res.material["tinted_wood"_H]
    );
    material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 4.0f);
    material_instance->set_texture("u_material.normal"_H, app->res.texture["piece_normal"_H], 1);
    material_instance->set_vec3("u_material.tint"_H, DEFAULT_TINT);
}

inline void initialize_node(Application* app, Index index) {
    auto material_instance = app->res.material_instance.load(
        hs("node" + std::to_string(index)),
        app->res.material["basic"_H]
    );
    material_instance->set_vec4("u_color"_H, glm::vec4(0.0f));
}

inline void initialize_nodes(Application* app) {
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
        initialize_node(app, i);
    }
}

inline void initialize_board_no_normal(Application* app) {
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

inline void initialize_board_paint_no_normal(Application* app) {
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

inline void initialize_pieces_no_normal(Application* app) {
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

inline void initialize_piece_no_normal(Application* app, Index index, std::shared_ptr<gl::Texture> diffuse_texture) {
    auto material_instance = app->res.material_instance.load(
        hs("piece" + std::to_string(index)),
        app->res.material["tinted_wood"_H]
    );
    material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 4.0f);
    material_instance->set_vec3("u_material.tint"_H, DEFAULT_TINT);
}

inline void initialize_skybox(Application* app) {
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

inline void initialize_indicators_textures(Application* app) {
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

inline void change_board_paint_texture(Application* app) {
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

inline void initialize_ids(Application* app) {
    auto& data = app->user_data<Data>();

    for (size_t i = 0; i < MAX_NODES; i++) {
        data.node_ids[i] = identifier::generate_id();
    }

    for (size_t i = 0; i < MAX_PIECES; i++) {
        data.piece_ids[i] = identifier::generate_id();
    }
}

inline void release_piece_material_instances(Application* app) {
    for (size_t i = 0; i < MAX_PIECES; i++) {
        app->res.material_instance.release(hs("piece" + std::to_string(i)));
    }
}

template<typename S>
void setup_and_add_model_board(Application* app, S* scene) {
    auto& board = scene->board;

    board.model->scale = WORLD_SCALE;
    board.model->vertex_array = app->res.vertex_array["board_wood"_H];
    board.model->index_buffer = app->res.index_buffer["board_wood"_H];
    board.model->material = app->res.material_instance["board_wood"_H];
    board.model->cast_shadow = true;
    board.model->bounding_box = std::make_optional<BoundingBox>();
    board.model->bounding_box->id = identifier::null;
    board.model->bounding_box->size = BOARD_BOUNDING_BOX;
    board.model->bounding_box->sort = false;

    app->renderer->add_model(board.model);

    DEB_DEBUG("Setup model board");
}

template<typename S>
void setup_and_add_model_board_paint(Application* app, S* scene) {
    const auto& board = scene->board;

    board.paint_model->position = glm::vec3(0.0f, PAINT_Y_POSITION, 0.0f);
    board.paint_model->scale = WORLD_SCALE;
    board.paint_model->vertex_array = app->res.vertex_array["board_paint"_H];
    board.paint_model->index_buffer = app->res.index_buffer["board_paint"_H];
    board.paint_model->material = app->res.material_instance["board_paint"_H];

    app->renderer->add_model(board.paint_model);

    DEB_DEBUG("Setup model board paint");
}

template<typename S>
void setup_and_add_model_piece(Application* app, S* scene, Index index, const glm::vec3& position) {
    auto& data = app->user_data<Data>();

    const Piece& piece = scene->board.pieces.at(index);

    const auto id = piece.type == PieceType::White ? "white_piece"_H : "black_piece"_H;

    piece.model->position = position;
    piece.model->rotation = RANDOM_PIECE_ROTATION();
    piece.model->scale = WORLD_SCALE;
    piece.model->vertex_array = app->res.vertex_array[id];
    piece.model->index_buffer = app->res.index_buffer[id];
    piece.model->material = app->res.material_instance[hs("piece" + std::to_string(index))];
    piece.model->outline_color = std::make_optional<glm::vec3>(1.0f);
    piece.model->cast_shadow = true;
    piece.model->bounding_box = std::make_optional<BoundingBox>();
    piece.model->bounding_box->id = data.piece_ids[index];
    piece.model->bounding_box->size = PIECE_BOUNDING_BOX;

    app->renderer->add_model(piece.model);

    piece.source->set_position(position);
    piece.source->set_reference_distance(6.0f);

    DEB_DEBUG("Setup model piece {}", index);
}

template<typename S>
void setup_and_add_model_nodes(Application* app, S* scene) {
    for (size_t i = 0; i < MAX_NODES; i++) {
        setup_and_add_model_node(app, scene, i, NODE_POSITIONS[i]);
    }
}

template<typename S>
void setup_and_add_model_node(Application* app, S* scene, size_t index, const glm::vec3& position) {
    auto& data = app->user_data<Data>();

    const Node& node = scene->board.nodes.at(index);

    node.model->position = position;
    node.model->scale = WORLD_SCALE;
    node.model->vertex_array = app->res.vertex_array["node"_H];
    node.model->index_buffer = app->res.index_buffer["node"_H];
    node.model->material = app->res.material_instance[hs("node" + std::to_string(index))];
    node.model->bounding_box = std::make_optional<BoundingBox>();
    node.model->bounding_box->id = data.node_ids[index];
    node.model->bounding_box->size = NODE_BOUNDING_BOX;

    app->renderer->add_model(node.model);

    DEB_DEBUG("Setup model node {}", index);
}

template<typename S>
void setup_camera(Application* app, S* scene) {
    auto& data = app->user_data<Data>();

    static constexpr float PITCH = 47.0f;
    static constexpr float DISTANCE_TO_POINT = 8.0f;

    scene->camera = Camera {};

    scene->camera_controller = PointCameraController {
        &scene->camera,
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

    scene->default_camera_position = scene->camera_controller.get_position();

    scene->camera_controller = PointCameraController {
        &scene->camera,
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

    app->renderer->set_camera_controller(&scene->camera_controller);
    update_listener(app, scene);

    DEB_DEBUG("Setup camera");
}

template<typename S>
void update_listener(Application* app, const S* scene) {
    app->openal->get_listener().set_position(scene->camera_controller.get_position());
    app->openal->get_listener().set_look_at_and_up(
        scene->camera_controller.get_point() - scene->camera_controller.get_position(),
        glm::rotate(UP_VECTOR, scene->camera_controller.get_rotation().y, UP_VECTOR)
    );
}

template<typename S>
void update_cursor(Application* app, const S* scene) {
    auto& data = app->user_data<Data>();

    if (data.options.custom_cursor) {
        if (scene->board.must_take_piece) {
            app->window->set_cursor(data.cross_cursor);

            scene->scene.quad["keyboard_controls"_H]->texture = app->res.texture["keyboard_controls_cross"_H];
        } else {
            app->window->set_cursor(data.arrow_cursor);

            scene->scene.quad["keyboard_controls"_H]->texture = app->res.texture["keyboard_controls_default"_H];
        }
    }
}

template<typename S>
void set_skybox(Application* app, const S* scene, Skybox skybox) {
    if (skybox == Skybox::None) {
        app->renderer->set_skybox(nullptr);
        return;
    }

    auto& data = app->user_data<Data>();

    scene->skybox_loader->start_loading_thread(data.launcher_options.texture_quality, data.options.skybox);
}

template<typename S>
void set_board_paint_texture(Application* app, const S* scene) {
    auto& data = app->user_data<Data>();

    scene->board_paint_texture_loader->start_loading_thread(
        data.launcher_options.texture_quality,
        data.options.labeled_board
    );
}

template<typename S>
void update_turn_indicator(Application* app, const S* scene) {
    if (scene->board.turn == BoardPlayer::White) {
        scene->scene.image["turn_indicator"_H]->set_image(app->res.texture["white_indicator"_H]);
    } else {
        scene->scene.image["turn_indicator"_H]->set_image(app->res.texture["black_indicator"_H]);
    }
}

template<typename S>
void update_wait_indicator(Application* app, S* scene) {
    if (!scene->board.next_move) {
        if (!scene->show_wait_indicator) {
            app->gui_renderer->add_widget(scene->scene.image["wait_indicator"_H].get());
            scene->show_wait_indicator = true;
        }
    } else {
        if (scene->show_wait_indicator) {
            app->gui_renderer->remove_widget(scene->scene.image["wait_indicator"_H].get());
            scene->show_wait_indicator = false;
        }
    }
}

template<typename S>
void update_computer_thinking_indicator(Application* app, S* scene) {
    if (scene->game.state == GameState::ComputerThinkingMove) {
        if (!scene->show_computer_thinking_indicator) {
            app->gui_renderer->add_widget(scene->scene.image["computer_thinking_indicator"_H].get());
            scene->show_computer_thinking_indicator = true;
        }
    } else {
        if (scene->show_computer_thinking_indicator) {
            app->gui_renderer->remove_widget(scene->scene.image["computer_thinking_indicator"_H].get());
            scene->show_computer_thinking_indicator = false;
        }
    }
}

template<typename S>
void update_timer_text(const S* scene) {
    char time[32];
    scene->timer.get_time_formatted(time);
    scene->scene.text["timer_text"_H]->set_text(time);
}

template<typename S>
void update_after_human_move(Application* app, S* scene, bool did_action, bool switched_turn, bool must_take_or_took_piece) {
    if (did_action) {
        scene->game.state = GameState::HumanDoingMove;
    }

    if (did_action && !scene->made_first_move && !scene->timer.is_running()) {
        scene->timer.start();
        scene->made_first_move = true;
    }

    if (scene->board.phase == BoardPhase::GameOver) {
        scene->timer.stop();
    }

    if (switched_turn) {
        update_turn_indicator(app, scene);
    }

    if (must_take_or_took_piece) {
        update_cursor(app, scene);
    }

    scene->imgui_layer.can_undo = scene->undo_redo_state.undo.size() > 0;
    scene->imgui_layer.can_redo = scene->undo_redo_state.redo.size() > 0;
}

template<typename S>
void update_after_computer_move(Application* app, S* scene, bool switched_turn) {
    scene->game.state = GameState::ComputerDoingMove;

    if (!scene->made_first_move && !scene->timer.is_running()) {
        scene->timer.start();
        scene->made_first_move = true;
    }

    if (scene->board.phase == BoardPhase::GameOver) {
        scene->timer.stop();
    }

    if (switched_turn) {
        update_turn_indicator(app, scene);
    }

    scene->imgui_layer.can_undo = scene->undo_redo_state.undo.size() > 0;
    scene->imgui_layer.can_redo = scene->undo_redo_state.redo.size() > 0;
}

template<typename S>
void update_game_state(Application* app, S* scene) {
    switch (scene->game.state) {
        case GameState::MaybeNextPlayer:
            switch (scene->board.turn) {
                case BoardPlayer::White:
                    switch (scene->game.white_player) {
                        case GamePlayer::None:
                            ASSERT(false, "Player must not be None");
                            break;
                        case GamePlayer::Human:
                            scene->game.state = GameState::HumanBeginMove;
                            break;
                        case GamePlayer::Computer:
                            scene->game.state = GameState::ComputerBeginMove;
                            break;
                    }
                    break;
                case BoardPlayer::Black:
                    switch (scene->game.black_player) {
                        case GamePlayer::None:
                            ASSERT(false, "Player must not be None");
                            break;
                        case GamePlayer::Human:
                            scene->game.state = GameState::HumanBeginMove;
                            break;
                        case GamePlayer::Computer:
                            scene->game.state = GameState::ComputerBeginMove;
                            break;
                    }
                    break;
            }
            break;
        case GameState::HumanBeginMove:
            scene->game.begin_human_move();
            scene->game.state = GameState::HumanThinkingMove;
            break;
        case GameState::HumanThinkingMove:
            break;
        case GameState::HumanDoingMove:
            if (scene->board.next_move) {
                scene->game.state = GameState::HumanEndMove;
            }
            break;
        case GameState::HumanEndMove:
            scene->game.end_human_move();
            scene->game.state = GameState::MaybeNextPlayer;
            break;
        case GameState::ComputerBeginMove:
            scene->game.begin_computer_move();
            scene->game.state = GameState::ComputerThinkingMove;
            break;
        case GameState::ComputerThinkingMove:
            if (!scene->minimax_thread.is_running()) {
                scene->minimax_thread.join();

                const bool switched_turn = scene->game.end_computer_move();

                update_after_computer_move(app, scene, switched_turn);
            }
            break;
        case GameState::ComputerDoingMove:
            if (scene->board.next_move) {
                scene->game.state = GameState::ComputerEndMove;
            }
            break;
        case GameState::ComputerEndMove:
            scene->game.state = GameState::MaybeNextPlayer;
            break;
    }
}

template<typename S>
void setup_piece_on_node(Application* app, S* scene, Index index, Index node_index) {
    setup_and_add_model_piece(
        app,
        scene,
        index,
        PIECE_INDEX_POSITION(node_index)
    );

    scene->board.pieces.at(index).node_index = node_index;
    scene->board.nodes.at(node_index).piece_index = index;
}

template<typename S>
void update_all_imgui(Application* app, S* scene) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(app->data().width, app->data().height);
    io.DeltaTime = app->get_delta();

    auto& imgui_layer = scene->imgui_layer;

    if (scene->board.phase == BoardPhase::GameOver && scene->board.next_move) {
        imgui_layer.window = WindowImGui::ShowGameOver;
    }

    imgui_layer.draw_menu_bar();

    switch (imgui_layer.window) {
        case WindowImGui::None:
            // Do nothing
            break;
        case WindowImGui::ShowAbout:
            imgui_layer.draw_about();
            break;
        case WindowImGui::ShowCouldNotLoadGame:
            imgui_layer.draw_could_not_load_game();
            break;
        case WindowImGui::ShowNoLastGame:
            imgui_layer.draw_no_last_game();
            break;
        case WindowImGui::ShowGameOver:
            imgui_layer.draw_game_over();
            break;
        case WindowImGui::ShowRulesStandardGame:
            imgui_layer.draw_rules_standard_game();
            break;
        case WindowImGui::ShowRulesJumpVariant:
            imgui_layer.draw_rules_jump_variant();
            break;
        case WindowImGui::ShowRulesJumpPlusVariant:
            imgui_layer.draw_rules_jump_plus_variant();
            break;
    }

    if (imgui_layer.show_info) {
        imgui_layer.draw_info();
    }

#ifdef NM3D_PLATFORM_DEBUG
    imgui_layer.draw_debug();
#endif
}

template<typename S, typename B>
void save_game_generic(S* scene) {
    scene->board.finalize_pieces_state();

    B board_serialized;
    scene->board.to_serialized(board_serialized);

    save_load::SavedGame<B> saved_game;
    saved_game.board_serialized = board_serialized;
    saved_game.camera_controller = scene->camera_controller;
    saved_game.time = scene->timer.get_time();

    time_t current;
    time(&current);
    saved_game.date = ctime(&current);

    saved_game.undo_redo_state = scene->undo_redo_state;
    saved_game.white_player = scene->game.white_player;
    saved_game.black_player = scene->game.black_player;

    try {
        save_load::save_game_to_file(saved_game, scene->save_game_file_name);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_WARNING("Could not save game: {}", e.what());

        save_load::handle_save_file_not_open_error();
    } catch (const save_load::SaveFileError& e) {
        REL_WARNING("Could not save game: {}", e.what());
    }
}

template<typename S, typename B>
void load_game_generic(Application* app, S* scene) {
    scene->board.finalize_pieces_state();

    save_load::SavedGame<B> saved_game;

    try {
        save_load::load_game_from_file(saved_game, scene->save_game_file_name);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_WARNING("Could not load game: {}", e.what());

        save_load::handle_save_file_not_open_error();

        scene->imgui_layer.window = WindowImGui::ShowCouldNotLoadGame;
        return;
    } catch (const save_load::SaveFileError& e) {
        REL_WARNING("Could not load game: {}", e.what());  // TODO maybe delete file

        scene->imgui_layer.window = WindowImGui::ShowCouldNotLoadGame;
        return;
    }

    scene->board.from_serialized(saved_game.board_serialized);
    scene->camera_controller = saved_game.camera_controller;
    scene->timer = Timer {app, saved_game.time};
    scene->undo_redo_state = std::move(saved_game.undo_redo_state);
    scene->game.white_player = saved_game.white_player;
    scene->game.black_player = saved_game.black_player;

    // Set camera pointer lost in serialization
    scene->camera_controller.set_camera(&scene->camera);

    scene->made_first_move = false;

    update_cursor(app, scene);
    update_turn_indicator(app, scene);
}

template<typename S, typename B>
void undo_generic(Application* app, S* scene) {
    ASSERT(!scene->undo_redo_state.undo.empty(), "Undo history must not be empty");

    if (!scene->board.next_move) {
        DEB_WARNING("Cannot undo when pieces are in air");
        return;
    }

    const bool undo_game_over = scene->board.phase == BoardPhase::None;

    using State = typename UndoRedoState<B>::State;

    B board_serialized;
    scene->board.to_serialized(board_serialized);

    State current_state = { board_serialized, scene->camera_controller };
    const State& previous_state = scene->undo_redo_state.undo.back();

    scene->board.from_serialized(previous_state.board_serialized);
    scene->camera_controller = previous_state.camera_controller;

    scene->undo_redo_state.undo.pop_back();
    scene->undo_redo_state.redo.push_back(current_state);

    DEB_DEBUG("Undid move; popped from undo stack and pushed onto redo stack");

    scene->game.state = GameState::MaybeNextPlayer;
    scene->made_first_move = scene->board.turn_count != 0;

    if (undo_game_over) {
        scene->timer.start();
    }

    update_cursor(app, scene);
    update_turn_indicator(app, scene);
}

template<typename S, typename B>
void redo_generic(Application* app, S* scene) {
    ASSERT(!scene->undo_redo_state.redo.empty(), "Redo history must not be empty");

    if (!scene->board.next_move) {
        DEB_WARNING("Cannot redo when pieces are in air");
        return;
    }

    using State = typename UndoRedoState<B>::State;

    B board_serialized;
    scene->board.to_serialized(board_serialized);

    State current_state = { board_serialized, scene->camera_controller };
    const State& previous_state = scene->undo_redo_state.redo.back();

    scene->board.from_serialized(previous_state.board_serialized);
    scene->camera_controller = previous_state.camera_controller;

    scene->undo_redo_state.redo.pop_back();
    scene->undo_redo_state.undo.push_back(current_state);

    DEB_DEBUG("Redid move; popped from redo stack and pushed onto undo stack");

    scene->game.state = GameState::MaybeNextPlayer;
    scene->made_first_move = scene->board.turn_count != 0;

    const bool redo_game_over = scene->board.phase == BoardPhase::None;

    if (redo_game_over) {
        scene->timer.stop();
        scene->board.phase = BoardPhase::GameOver;  // Make the game over screen show up again
    }

    update_cursor(app, scene);
    update_turn_indicator(app, scene);
}

inline void initialize_keyboard_controls(Application* app) {
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

#ifdef NM3D_PLATFORM_DEBUG
template<typename S>
static void initialize_light_bulb(Application* app, S* scene) {
    auto light_bulb = scene->scene.quad.load("light_bulb"_H);

    gl::TextureSpecification specification;

    auto light_bulb_texture = app->res.texture.load(
        "light_bulb"_H,
        "data/textures/light_bulb/light_bulb.png",
        specification
    );

    light_bulb->texture = light_bulb_texture;
}
#endif

template<typename S>
void initialize_light(Application* app, S* scene) {
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

#ifdef NM3D_PLATFORM_DEBUG
    if (data.options.skybox == game_options::FIELD) {
        scene->scene.quad["light_bulb"_H]->position = LIGHT_FIELD.position;
    } else if (data.options.skybox == game_options::AUTUMN) {
        scene->scene.quad["light_bulb"_H]->position = LIGHT_AUTUMN.position;
    } else if (data.options.skybox == game_options::NONE) {
        scene->scene.quad["light_bulb"_H]->position = LIGHT_NONE.position;
    }
#endif

    DEB_DEBUG("Initialized light");
}

template<typename S>
void setup_and_add_turn_indicator(Application* app, S* scene) {
    auto turn_indicator = scene->scene.image.load(
        "turn_indicator"_H, app->res.texture["white_indicator"_H]
    );

    turn_indicator->stick(gui::Sticky::SE);
    turn_indicator->offset(30, gui::Relative::Right);
    turn_indicator->offset(30, gui::Relative::Bottom);
    turn_indicator->scale(0.4f, 1.0f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);

    app->gui_renderer->add_widget(turn_indicator.get());
}

template<typename S>
void setup_and_add_timer_text(Application* app, S* scene) {
    auto& data = app->user_data<Data>();

    auto timer_text = scene->scene.text.load(
        "timer_text"_H,
        app->res.font["open_sans"_H],
        "00:00",
        1.5f,
        glm::vec3(0.9f)
    );

    timer_text->stick(gui::Sticky::N);
    timer_text->offset(60, gui::Relative::Top);
    timer_text->scale(0.6f, 1.4f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);
    timer_text->set_shadows(true);

    if (!data.options.hide_timer) {
        app->gui_renderer->add_widget(timer_text.get());
    }
}

template<typename S>
void setup_wait_indicator(Application* app, S* scene) {
    auto wait_indicator = scene->scene.image.load(
        "wait_indicator"_H, app->res.texture["wait_indicator"_H]
    );

    wait_indicator->stick(gui::Sticky::NE);
    wait_indicator->offset(25, gui::Relative::Right);
    wait_indicator->offset(55, gui::Relative::Top);
    wait_indicator->scale(0.4f, 1.0f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);
}

template<typename S>
void setup_computer_thinking_indicator(Application* app, S* scene) {
    auto computer_thinking_indicator = scene->scene.image.load(
        "computer_thinking_indicator"_H, app->res.texture["computer_thinking_indicator"_H]
    );

    computer_thinking_indicator->stick(gui::Sticky::NE);
    computer_thinking_indicator->offset(25, gui::Relative::Right);
    computer_thinking_indicator->offset(55, gui::Relative::Top);
    computer_thinking_indicator->scale(0.4f, 1.0f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);
}

template<typename S>
void change_skybox(Application* app, S* scene) {
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

    initialize_light(app, scene);

    app->res.texture_data.release("skybox"_H);
}

// Must be called only once
inline void initialize_game(Application* app) {
    auto& data = app->user_data<Data>();

    initialize_ids(app);

    if (data.launcher_options.normal_mapping) {
        initialize_board(app);
        initialize_board_paint(app);
        initialize_pieces(app);
    } else {
        initialize_board_no_normal(app);
        initialize_board_paint_no_normal(app);
        initialize_pieces_no_normal(app);
    }
    initialize_nodes(app);

    initialize_keyboard_controls(app);
    initialize_skybox(app);
    initialize_indicators_textures(app);

    auto track = app->res.music_track.load("music"_H, app->res.sound_data["music"_H]);
    data.current_music_track = track;

    if (data.options.enable_music) {
        music::play_music_track(track);
    }
}
