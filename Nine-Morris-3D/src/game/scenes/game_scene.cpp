#include <engine/nine_morris_3d.hpp>
#include <engine/external/resmanager.h++>
#include <engine/external/glm.h++>
#include <engine/external/imgui.h++>

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

    {
        auto mesh = std::make_shared<sm::Mesh>(
            sm::Encrypt::encr(sm::FileSystem::path_assets("models/dragon.obj")),
            sm::Mesh::Type::PN
        );

        auto vertex_buffer = std::make_shared<sm::GlVertexBuffer>(
            mesh->get_vertices(),
            mesh->get_vertices_size()
        );

        auto index_buffer = std::make_shared<sm::GlIndexBuffer>(
            mesh->get_indices(),
            mesh->get_indices_size()
        );

        sm::VertexBufferLayout layout;
        layout.add(0, sm::VertexBufferLayout::Float, 3);
        layout.add(1, sm::VertexBufferLayout::Float, 3);

        auto vertex_array = ctx->res.vertex_array.load("dragon"_H);
        vertex_array->bind();
        vertex_array->add_vertex_buffer(vertex_buffer, layout);
        vertex_array->add_index_buffer(index_buffer);
        sm::GlVertexArray::unbind();
    }

    {
        auto mesh = std::make_shared<sm::Mesh>(
            sm::Encrypt::encr(sm::FileSystem::path_assets("models/teapot.obj")),
            sm::Mesh::Type::PN
        );

        auto vertex_buffer = std::make_shared<sm::GlVertexBuffer>(
            mesh->get_vertices(),
            mesh->get_vertices_size()
        );

        auto index_buffer = std::make_shared<sm::GlIndexBuffer>(
            mesh->get_indices(),
            mesh->get_indices_size()
        );

        sm::VertexBufferLayout layout;
        layout.add(0, sm::VertexBufferLayout::Float, 3);
        layout.add(1, sm::VertexBufferLayout::Float, 3);

        auto vertex_array = ctx->res.vertex_array.load("teapot"_H);
        vertex_array->bind();
        vertex_array->add_vertex_buffer(vertex_buffer, layout);
        vertex_array->add_index_buffer(index_buffer);
        sm::GlVertexArray::unbind();
    }

    auto shader = std::make_shared<sm::GlShader>(
        sm::Encrypt::encr(sm::FileSystem::path_assets("shaders/dragon.vert")),
        sm::Encrypt::encr(sm::FileSystem::path_assets("shaders/dragon.frag"))
    );

    ctx->r3d->add_shader(shader);

    auto material = ctx->res.material.load("simple"_H, shader);
    material->add_uniform(sm::Material::Uniform::Vec3, "u_material.ambient_diffuse"_H);
    material->add_uniform(sm::Material::Uniform::Vec3, "u_material.specular"_H);
    material->add_uniform(sm::Material::Uniform::Float, "u_material.shininess"_H);

    {
        auto material_instance = ctx->res.material_instance.load("dragon"_H, material);
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(1.0f, 1.0f, 0.0f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(1.0f, 1.0f, 0.0f));
        material_instance->set_float("u_material.shininess"_H, 32.0f);
    }

    {
        auto material_instance = ctx->res.material_instance.load("teapot"_H, material);
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(0.7f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.7f));
        material_instance->set_float("u_material.shininess"_H, 64.0f);
        material_instance->flags |= sm::Material::DisableBackFaceCulling;
    }

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
        0.5f
    );

    cam_controller.connect_events(ctx);

    directional_light.position = glm::vec3(2.5f, 15.0f, -18.0f);
    directional_light.ambient_color = glm::vec3(0.1f);
    directional_light.diffuse_color = glm::vec3(1.0f);
    directional_light.specular_color = glm::vec3(1.0f);

    teapot.position = glm::vec3(2.6f, 0.0, -7.0f);
    teapot.rotation = glm::vec3(0.0f, 5.3f, 0.0f);
}

void GameScene::on_update() {
    cam_controller.update_controls(ctx->delta);
    cam_controller.update_camera(ctx->delta);
    cam_controller.update_friction();

    ctx->r3d->capture(cam, cam_controller.get_position());

    ctx->r3d->add_light(directional_light);

    sm::Renderable dragon;
    dragon.vertex_array = ctx->res.vertex_array["dragon"_H];
    dragon.material = ctx->res.material_instance["dragon"_H];
    dragon.scale = 0.7f;

    ctx->r3d->add_renderable(dragon);

    dragon.position = glm::vec3(4.0f, 0.0, 0.0f);
    dragon.scale = 0.2f;

    ctx->r3d->add_renderable(dragon);

    teapot.vertex_array = ctx->res.vertex_array["teapot"_H];
    teapot.material = ctx->res.material_instance["teapot"_H];

    ctx->r3d->add_renderable(teapot);
}

void GameScene::on_imgui_update() {
    ImGui::Begin("Light Settings");
    ImGui::SliderFloat3("Position", glm::value_ptr(directional_light.position), -30.0f, 30.0f);
    ImGui::SliderFloat3("Ambient color", glm::value_ptr(directional_light.ambient_color), 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse color", glm::value_ptr(directional_light.diffuse_color), 0.0f, 1.0f);
    ImGui::SliderFloat3("Specular color", glm::value_ptr(directional_light.specular_color), 0.0f, 1.0f);
    ImGui::End();

    // ImGui::Begin("Transform");
    // ImGui::SliderFloat3("Position", glm::value_ptr(teapot.position), -30.0f, 30.0f);
    // ImGui::SliderFloat3("Rotation", glm::value_ptr(teapot.rotation), 0.0f, glm::two_pi<float>());
    // ImGui::End();
}
