#include <engine/engine_application.h>
#include <engine/engine_audio.h>
#include <engine/engine_graphics.h>
#include <engine/engine_other.h>

#include "game/scenes/common.h"
#include "game/assets.h"
#include "other/data.h"
#include "other/constants.h"

using namespace encrypt;

static void initialize_board(Application* app) {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_wood"_h,
        encr(file_system::path_for_assets(assets::BOARD_VERTEX_SHADER)),
        encr(file_system::path_for_assets(assets::BOARD_FRAGMENT_SHADER)),
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

static void initialize_board_paint(Application* app) {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_paint"_h,
        encr(file_system::path_for_assets(assets::BOARD_PAINT_VERTEX_SHADER)),
        encr(file_system::path_for_assets(assets::BOARD_PAINT_FRAGMENT_SHADER)),
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

static void initialize_pieces(Application* app) {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "piece"_h,
        encr(file_system::path_for_assets(assets::PIECE_VERTEX_SHADER)),
        encr(file_system::path_for_assets(assets::PIECE_FRAGMENT_SHADER)),
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
            app->renderer->get_storage().light_uniform_block,
            app->renderer->get_storage().light_view_position_uniform_block,
            app->renderer->get_storage().light_space_uniform_block
        }
    );
    app->renderer->setup_shader(shader);

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

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);
    layout.add(3, BufferLayout::Float, 3);

    auto white_piece_vertex_array = app->res.vertex_array.load("white_piece"_h);
    white_piece_vertex_array->add_buffer(white_piece_vertex_buffer, layout);
    white_piece_vertex_array->add_index_buffer(white_piece_index_buffer);
    gl::VertexArray::unbind();

    auto black_piece_vertex_array = app->res.vertex_array.load("black_piece"_h);
    black_piece_vertex_array->add_buffer(black_piece_vertex_buffer, layout);
    black_piece_vertex_array->add_index_buffer(black_piece_index_buffer);
    gl::VertexArray::unbind();

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
}

void initialize_piece(Application* app, Index index, std::shared_ptr<gl::Texture> diffuse_texture) {
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

static void initialize_node(Application* app, Index index) {
    auto material_instance = app->res.material_instance.load(
        hs {"node" + std::to_string(index)},
        app->res.material["basic"_h]
    );
    material_instance->set_vec4("u_color", glm::vec4(0.0f));
}

static void initialize_nodes(Application* app) {
    auto shader = app->res.shader.load(
        "node"_h,
        encr(file_system::path_for_assets(assets::NODE_VERTEX_SHADER)),
        encr(file_system::path_for_assets(assets::NODE_FRAGMENT_SHADER)),
        std::vector<std::string> { "u_model_matrix", "u_color" },
        std::initializer_list {
            app->renderer->get_storage().projection_view_uniform_block
        }
    );
    app->renderer->setup_shader(shader);

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

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);

    auto vertex_array = app->res.vertex_array.load("node"_h);
    vertex_array->add_buffer(vertex_buffer, layout);
    vertex_array->add_index_buffer(index_buffer);
    gl::VertexArray::unbind();

    auto material = app->res.material.load("basic"_h, shader);
    material->add_uniform(Material::Uniform::Vec4, "u_color");

    for (size_t i = 0; i < 24; i++) {
        initialize_node(app, i);
    }
}

static void initialize_board_no_normal(Application* app) {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_wood"_h,
        encr(file_system::path_for_assets(assets::BOARD_VERTEX_SHADER_NO_NORMAL)),
        encr(file_system::path_for_assets(assets::BOARD_FRAGMENT_SHADER_NO_NORMAL)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess"
        },
        std::initializer_list {
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

static void initialize_board_paint_no_normal(Application* app) {
    auto& data = app->user_data<Data>();

    auto shader = app->res.shader.load(
        "board_paint"_h,
        encr(file_system::path_for_assets(assets::BOARD_PAINT_VERTEX_SHADER_NO_NORMAL)),
        encr(file_system::path_for_assets(assets::BOARD_PAINT_FRAGMENT_SHADER_NO_NORMAL)),
        std::vector<std::string> {
            "u_model_matrix",
            "u_shadow_map",
            "u_material.diffuse",
            "u_material.specular",
            "u_material.shininess"
        },
        std::initializer_list {
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

static void initialize_pieces_no_normal(Application* app) {
    auto& data = app->user_data<Data>();

    // FIXME maybe should give another name
    auto shader = app->res.shader.load(
        "piece"_h,
        encr(file_system::path_for_assets(assets::PIECE_VERTEX_SHADER_NO_NORMAL)),
        encr(file_system::path_for_assets(assets::PIECE_FRAGMENT_SHADER_NO_NORMAL)),
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
            app->renderer->get_storage().light_uniform_block,
            app->renderer->get_storage().light_view_position_uniform_block,
            app->renderer->get_storage().light_space_uniform_block
        }
    );
    app->renderer->setup_shader(shader);

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

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 3);
    layout.add(1, BufferLayout::Float, 2);
    layout.add(2, BufferLayout::Float, 3);

    auto white_piece_vertex_array = app->res.vertex_array.load("white_piece"_h);
    white_piece_vertex_array->add_buffer(white_piece_vertex_buffer, layout);
    white_piece_vertex_array->add_index_buffer(white_piece_index_buffer);
    gl::VertexArray::unbind();

    auto black_piece_vertex_array = app->res.vertex_array.load("black_piece"_h);
    black_piece_vertex_array->add_buffer(black_piece_vertex_buffer, layout);
    black_piece_vertex_array->add_index_buffer(black_piece_index_buffer);
    gl::VertexArray::unbind();

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
}

void initialize_piece_no_normal(Application* app, Index index, std::shared_ptr<gl::Texture> diffuse_texture) {
    auto material_instance = app->res.material_instance.load(
        hs {"piece" + std::to_string(index)},
        app->res.material["tinted_wood"_h]
    );
    material_instance->set_texture("u_material.diffuse", diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular", glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess", 4.0f);
    material_instance->set_vec3("u_material.tint", DEFAULT_TINT);
}

static void initialize_keyboard_controls(Application* app) {
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

#ifdef NM3D_PLATFORM_DEBUG
static void initialize_light_bulb(Application* app) {
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
#endif

static void initialize_skybox(Application* app) {
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

static void initialize_light(Application* app) {
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
        app->res.quad["light_bulb"_h]->position = LIGHT_FIELD.position;
    } else if (data.options.skybox == game_options::AUTUMN) {
        app->res.quad["light_bulb"_h]->position = LIGHT_AUTUMN.position;
    } else if (data.options.skybox == game_options::NONE) {
        app->res.quad["light_bulb"_h]->position = LIGHT_NONE.position;
    }
#endif

    DEB_DEBUG("Initialized light");
}

static void initialize_indicators_textures(Application* app) {
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

void change_skybox(Application* app) {
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

    initialize_light(app);

    app->res.texture_data.release("skybox"_h);
}

void change_board_paint_texture(Application* app) {
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

void setup_and_add_turn_indicator(Application* app) {
    auto turn_indicator = app->res.image.load(
        "turn_indicator"_h, app->res.texture["white_indicator"_h]
    );

    turn_indicator->stick(gui::Sticky::SE);
    turn_indicator->offset(30, gui::Relative::Right);
    turn_indicator->offset(30, gui::Relative::Bottom);
    turn_indicator->scale(0.4f, 1.0f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);

    app->gui_renderer->add_widget(turn_indicator);
}

void setup_and_add_timer_text(Application* app) {
    auto& data = app->user_data<Data>();

    auto timer_text = app->res.text.load(
        "timer_text"_h,
        app->res.font["open_sans"_h],
        "00:00",
        1.5f,
        glm::vec3(0.9f)
    );

    timer_text->stick(gui::Sticky::N);
    timer_text->offset(60, gui::Relative::Top);
    timer_text->scale(0.6f, 1.4f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);
    timer_text->set_shadows(true);

    if (!data.options.hide_timer) {
        app->gui_renderer->add_widget(timer_text);
    }
}

void setup_wait_indicator(Application* app) {
    auto wait_indicator = app->res.image.load(
        "wait_indicator"_h, app->res.texture["wait_indicator"_h]
    );

    wait_indicator->stick(gui::Sticky::NE);
    wait_indicator->offset(25, gui::Relative::Right);
    wait_indicator->offset(55, gui::Relative::Top);
    wait_indicator->scale(0.4f, 1.0f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);
}

void setup_computer_thinking_indicator(Application* app) {
    auto computer_thinking_indicator = app->res.image.load(
        "computer_thinking_indicator"_h, app->res.texture["computer_thinking_indicator"_h]
    );

    computer_thinking_indicator->stick(gui::Sticky::NE);
    computer_thinking_indicator->offset(25, gui::Relative::Right);
    computer_thinking_indicator->offset(55, gui::Relative::Top);
    computer_thinking_indicator->scale(0.4f, 1.0f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);
}

static void initialize_ids(Application* app) {
    auto& data = app->user_data<Data>();

    for (size_t i = 0; i < MAX_NODES; i++) {
        data.node_ids[i] = identifier::generate_id();
    }

    for (size_t i = 0; i < MAX_PIECES; i++) {
        data.piece_ids[i] = identifier::generate_id();
    }
}

void release_piece_material_instances(Application* app) {
    for (size_t i = 0; i < MAX_PIECES; i++) {
        app->res.material_instance.release(hs {"piece" + std::to_string(i)});
    }
}

void initialize_game(Application* app) {
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
#ifdef NM3D_PLATFORM_DEBUG
    initialize_light_bulb(app);
#endif

    initialize_skybox(app);
    initialize_light(app);
    initialize_indicators_textures(app);

    auto track = app->res.music_track.load("music"_h, app->res.sound_data["music"_h]);
    data.current_music_track = track;

    if (data.options.enable_music) {
        music::play_music_track(track);
    }
}
