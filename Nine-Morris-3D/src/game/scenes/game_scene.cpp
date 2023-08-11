#include <engine/nine_morris_3d.hpp>
#include <engine/external/resmanager.h++>
#include <engine/external/glm.h++>

#include <game/scenes/game_scene.hpp>
#include <game/point_camera_controller.hpp>

void GameScene::on_start() {
    ctx->tsk.add("test"_H, [this](const sm::Task& task) {
        if (task.get_total_time() > 3.0) {
            LOG_DEBUG("Done");

            return sm::Task::Result::Done;
        }

        return sm::Task::Result::Continue;
    });

    // auto shader = ctx->res.shader.load(
    //     "board_wood"_H,
    //     sm::Encrypt::encr(sm::FileSystem::path_assets("shaders/board/board.vert")),
    //     sm::Encrypt::encr(sm::FileSystem::path_assets("shaders/board/board.frag")),
    //     std::initializer_list<std::string_view> {
    //         "u_model_matrix",
    //         "u_shadow_map",
    //         "u_material.diffuse",
    //         "u_material.specular",
    //         "u_material.shininess",
    //         "u_material.normal"
    //     },
    //     std::initializer_list {
    //         ctx->r3d->pstorage.projection_view_uniform_block
    //         // ctx->r3d->get_storage().light_view_uniform_block,
    //         // ctx->r3d->get_storage().light_space_uniform_block,
    //         // ctx->r3d->get_storage().light_uniform_block
    //     }
    // );

    // ctx->res.mesh.load(
    //     "board_wood"_H,
    //     sm::MeshLoader::PTNT(),
    //     sm::Encrypt::encr(sm::FileSystem::path_assets("models/board/board.obj"))
    // );

    // ctx->res.texture_data.load(
    //     "board_wood_diffuse"_H,
    //     sm::Encrypt::encr(sm::FileSystem::path_assets("textures/board/wood/board_wood.png")),
    //     true
    // );

    // ctx->res.texture_data.load(
    //     "board_normal"_H,
    //     sm::Encrypt::encr(sm::FileSystem::path_assets("textures/board/board_normal.png")),
    //     true
    // );

    // auto vertex_buffer = ctx->res.vertex_buffer.load(
    //     "board_wood"_H,
    //     ctx->res.mesh["board_wood"_H]->get_vertices(),
    //     ctx->res.mesh["board_wood"_H]->get_vertices_size()
    // );

    // auto index_buffer = ctx->res.index_buffer.load(
    //     "board_wood"_H,
    //     ctx->res.mesh["board_wood"_H]->get_indices(),
    //     ctx->res.mesh["board_wood"_H]->get_indices_size()
    // );

    // sm::VertexBufferLayout layout;
    // layout.add(0, sm::VertexBufferLayout::Float, 3);
    // layout.add(1, sm::VertexBufferLayout::Float, 2);
    // layout.add(2, sm::VertexBufferLayout::Float, 3);
    // layout.add(3, sm::VertexBufferLayout::Float, 3);

    // auto vertex_array = ctx->res.vertex_array.load("board_wood"_H);
    // vertex_array->begin_definition()
    //     .add_buffer(vertex_buffer, layout)
    //     .add_index_buffer(index_buffer)
    //     .end_definition();

    // sm::TextureSpecification specification;
    // specification.mag_filter = sm::Filter::Linear;
    // specification.mipmap_levels = 4;
    // specification.bias = -2.0f;

    // auto diffuse_texture = ctx->res.texture.load(
    //     "board_wood_diffuse"_H,
    //     ctx->res.texture_data["board_wood_diffuse"_H],
    //     specification
    // );

    // auto normal_texture = ctx->res.texture.load(
    //     "board_normal"_H,
    //     ctx->res.texture_data["board_normal"_H],
    //     specification
    // );

    // auto material = ctx->res.material.load("wood"_H, shader);
    // material->add_texture("u_material.diffuse"_H);
    // material->add_uniform(sm::Material::Uniform::Vec3, "u_material.specular"_H);
    // material->add_uniform(sm::Material::Uniform::Float, "u_material.shininess"_H);
    // material->add_texture("u_material.normal"_H);

    // auto material_instance = ctx->res.material_instance.load("board_wood"_H, material);
    // material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    // material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    // material_instance->set_float("u_material.shininess"_H, 4.0f);
    // material_instance->set_texture("u_material.normal"_H, normal_texture, 1);

    auto shader = ctx->res.shader.load(
        "dragon"_H,
        sm::Encrypt::encr(sm::FileSystem::path_assets("shaders/dragon.vert")),
        sm::Encrypt::encr(sm::FileSystem::path_assets("shaders/dragon.frag")),
        std::initializer_list<std::string> {
            "u_model_matrix",
            "u_color"
        },
        std::initializer_list {
            ctx->r3d->pstorage.projection_view_uniform_block
        }
    );

    auto mesh = ctx->res.mesh.load(
        "dragon"_H,
        sm::MeshLoader::P(),
        sm::Encrypt::encr(sm::FileSystem::path_assets("models/dragon.obj")),
        true
    );

    auto vertex_buffer = ctx->res.vertex_buffer.load(
        "dragon"_H,
        mesh->get_vertices(),
        mesh->get_vertices_size()
    );

    auto index_buffer = ctx->res.index_buffer.load(
        "dragon"_H,
        mesh->get_indices(),
        mesh->get_indices_size()
    );

    sm::VertexBufferLayout layout;
    layout.add(0, sm::VertexBufferLayout::Float, 3);

    auto vertex_array = ctx->res.vertex_array.load("dragon"_H);
    vertex_array->begin_definition()
        .add_buffer(vertex_buffer, layout)
        .add_index_buffer(index_buffer)
        .end_definition();

    auto material = ctx->res.material.load("simple"_H, shader);
    material->add_uniform(sm::Material::Uniform::Vec3, "u_color"_H);

    auto material_instance = ctx->res.material_instance.load("dragon"_H, material);
    material_instance->set_vec3("u_color"_H, glm::vec3(1.0f, 0.0f, 0.0f));

    cam_controller = PointCameraController(
        &cam,
        ctx,
        ctx->properties->width,
        ctx->properties->height,
        LENS_FOV,
        LENS_NEAR,
        LENS_FAR,
        glm::vec3(0.0f),
        8.0f,
        47.0f,
        0.2f
    );

    cam_controller.connect_events(ctx);

    // cam.set_projection_matrix(ctx->properties->width, ctx->properties->height, 45.0f, 0.1f, 400.0f);
    // cam.set_position_orientation(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void GameScene::on_update() {
    cam_controller.update_controls(ctx->delta);
    cam_controller.update_camera(ctx->delta);
    cam_controller.update_friction();

    ctx->r3d->capture(cam, cam_controller.get_position());
    // ctx->r3d->capture(cam, glm::vec3(10.0f, 0.0f, 0.0f));

    sm::Renderable board;
    board.vertex_array = ctx->res.vertex_array["dragon"_H];
    board.index_buffer = ctx->res.index_buffer["dragon"_H];
    board.material = ctx->res.material_instance["dragon"_H];
    board.scale = 0.7f;

    ctx->r3d->add_renderable(board);
}
