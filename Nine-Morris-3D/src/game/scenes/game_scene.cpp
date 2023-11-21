#include <vector>

#include <engine/nine_morris_3d.hpp>
#include <engine/external/resmanager.h++>
#include <engine/external/glm.h++>
#include <engine/external/imgui.h++>

#include "game/scenes/game_scene.hpp"
#include "game/point_camera_controller.hpp"

void GameScene::on_start() {
    ctx->tsk.add("test"_H, [this](const sm::Task& task) {
        if (task.get_total_time() > 3.0) {
            LOG_DEBUG("Done");

            return sm::Task::Result::Done;
        }

        return sm::Task::Result::Continue;
    });

    ctx->evt.connect<sm::WindowResizedEvent, &GameScene::on_window_resized>(this);

    sm::RenderGl::clear_color(0.3f, 0.1f, 0.3f);

    {
        auto mesh {std::make_shared<sm::Mesh>(
            sm::Encrypt::encr(sm::FileSystem::path_assets("models/dragon.obj")),
            "default",
            sm::Mesh::Type::PN
        )};

        auto vertex_buffer {std::make_shared<sm::GlVertexBuffer>(
            mesh->get_vertices(),
            mesh->get_vertices_size()
        )};

        auto index_buffer {std::make_shared<sm::GlIndexBuffer>(
            mesh->get_indices(),
            mesh->get_indices_size()
        )};

        sm::VertexBufferLayout layout;
        layout.add(0, sm::VertexBufferLayout::Float, 3);
        layout.add(1, sm::VertexBufferLayout::Float, 3);

        auto vertex_array {ctx->res.vertex_array.load("dragon"_H)};
        vertex_array->configure([&](sm::GlVertexArray* va) {
            va->add_vertex_buffer(vertex_buffer, layout);
            va->add_index_buffer(index_buffer);
        });
    }

    {
        auto mesh {std::make_shared<sm::Mesh>(
            sm::Encrypt::encr(sm::FileSystem::path_assets("models/teapot.obj")),
            sm::Mesh::DEFAULT_OBJECT,
            sm::Mesh::Type::PN
        )};

        auto vertex_buffer {std::make_shared<sm::GlVertexBuffer>(
            mesh->get_vertices(),
            mesh->get_vertices_size()
        )};

        auto index_buffer {std::make_shared<sm::GlIndexBuffer>(
            mesh->get_indices(),
            mesh->get_indices_size()
        )};

        sm::VertexBufferLayout layout;
        layout.add(0, sm::VertexBufferLayout::Float, 3);
        layout.add(1, sm::VertexBufferLayout::Float, 3);

        auto vertex_array {ctx->res.vertex_array.load("teapot"_H)};
        vertex_array->configure([&](sm::GlVertexArray* va) {
            va->add_vertex_buffer(vertex_buffer, layout);
            va->add_index_buffer(index_buffer);
        });
    }

    {
        auto mesh {std::make_shared<sm::Mesh>(
            sm::Encrypt::encr(sm::FileSystem::path_assets("scene/scene.obj")),
            "Cube_Cube.001",
            sm::Mesh::Type::PN
        )};

        auto vertex_buffer {std::make_shared<sm::GlVertexBuffer>(
            mesh->get_vertices(),
            mesh->get_vertices_size()
        )};

        auto index_buffer {std::make_shared<sm::GlIndexBuffer>(
            mesh->get_indices(),
            mesh->get_indices_size()
        )};

        sm::VertexBufferLayout layout;
        layout.add(0, sm::VertexBufferLayout::Float, 3);
        layout.add(1, sm::VertexBufferLayout::Float, 3);

        auto vertex_array {ctx->res.vertex_array.load("cube"_H)};
        vertex_array->configure([&](sm::GlVertexArray* va) {
            va->add_vertex_buffer(vertex_buffer, layout);
            va->add_index_buffer(index_buffer);
        });
    }

    auto shader {
        std::make_shared<sm::GlShader>(
            sm::Encrypt::encr(sm::FileSystem::path_assets("shaders/simple.vert")),
            sm::Encrypt::encr(sm::FileSystem::path_assets("shaders/simple.frag"))
        )
    };

    ctx->rnd->add_shader(shader);

    auto material {ctx->res.material.load("simple"_H, shader)};
    material->add_uniform(sm::Material::Uniform::Vec3, "u_material.ambient_diffuse"_H);
    material->add_uniform(sm::Material::Uniform::Vec3, "u_material.specular"_H);
    material->add_uniform(sm::Material::Uniform::Float, "u_material.shininess"_H);

    {
        auto material_instance {ctx->res.material_instance.load("dragon1"_H, material)};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(1.0f, 1.0f, 0.0f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(1.0f, 1.0f, 0.0f));
        material_instance->set_float("u_material.shininess"_H, 32.0f);
    }

    {
        auto material_instance {ctx->res.material_instance.load("dragon2"_H, material)};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(1.0f, 0.2f, 0.1f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(1.0f, 1.0f, 0.0f));
        material_instance->set_float("u_material.shininess"_H, 32.0f);
    }

    {
        auto material_instance {ctx->res.material_instance.load("teapot"_H, material)};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(0.7f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.7f));
        material_instance->set_float("u_material.shininess"_H, 64.0f);
        material_instance->flags |= sm::Material::DisableBackFaceCulling;
    }

    {
        auto material_instance {ctx->res.material_instance.load("cube"_H, material)};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(1.0f, 0.0f, 0.0f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.8f));
        material_instance->set_float("u_material.shininess"_H, 128.0f);
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

    cam_controller.connect_events();

    directional_light.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    directional_light.ambient_color = glm::vec3(0.1f);
    directional_light.diffuse_color = glm::vec3(1.0f);
    directional_light.specular_color = glm::vec3(1.0f);

    point_light.position = glm::vec3(3.0f, 3.0f, 1.0f);
    point_light.ambient_color = glm::vec3(0.1f, 0.08f, 0.02f);
    point_light.diffuse_color = glm::vec3(1.0f, 0.8f, 0.2f);
    point_light.specular_color = glm::vec3(1.0f);
    point_light.falloff_linear = 0.09f;
    point_light.falloff_quadratic = 0.032f;

    teapot.position = glm::vec3(2.6f, 0.0, -7.0f);
    teapot.rotation = glm::vec3(0.0f, 5.3f, 0.0f);
}

void GameScene::on_update() {
    cam_controller.update_controls(ctx->delta);
    cam_controller.update_camera(ctx->delta);
    cam_controller.update_friction();

    ctx->rnd->capture(cam, cam_controller.get_position());

    ctx->rnd->add_light(directional_light);
    ctx->rnd->add_light(point_light);

    sm::Renderable dragon;
    dragon.vertex_array = ctx->res.vertex_array["dragon"_H];
    dragon.material = ctx->res.material_instance["dragon1"_H];
    dragon.scale = 0.7f;

    ctx->rnd->add_renderable(dragon);

    dragon.material = ctx->res.material_instance["dragon2"_H];
    dragon.position = glm::vec3(4.0f, 0.0, 0.0f);
    dragon.scale = 0.2f;

    ctx->rnd->add_renderable(dragon);

    teapot.vertex_array = ctx->res.vertex_array["teapot"_H];
    teapot.material = ctx->res.material_instance["teapot"_H];

    ctx->rnd->add_renderable(teapot);

    sm::Renderable cube;
    cube.vertex_array = ctx->res.vertex_array["cube"_H];
    cube.material = ctx->res.material_instance["cube"_H];
    cube.position = glm::vec3(5.0f, 2.0f, -2.0f);

    ctx->rnd->add_renderable(cube);

    // Origin
    ctx->rnd->debug_add_line(glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ctx->rnd->debug_add_line(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ctx->rnd->debug_add_line(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Whatever
    ctx->rnd->debug_add_lines(
        std::vector({
            glm::vec3(0.0f, 6.0f, 0.0f),
            glm::vec3(2.0f, 7.0f, 0.0f),
            glm::vec3(4.0f, 6.0f, 2.0f),
            glm::vec3(3.0f, 8.0f, 0.0f),
            glm::vec3(0.0f, 11.0f, -4.0f)
        }),
        glm::vec3(0.0f, 0.0f, 0.0f)
    );

    // Point light
    ctx->rnd->debug_add_lamp(point_light.position, point_light.diffuse_color);

    // Whatever part two
    ctx->rnd->debug_add_point(glm::vec3(0.0f, -3.0f, 4.0f), glm::vec3(0.0f, 1.0f, 1.0f));
}

void GameScene::on_imgui_update() {
    ImGui::Begin("Directional Light");
    ImGui::SliderFloat3("Direction", glm::value_ptr(directional_light.direction), -1.0f, 1.0f);
    ImGui::SliderFloat3("Ambient", glm::value_ptr(directional_light.ambient_color), 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse", glm::value_ptr(directional_light.diffuse_color), 0.0f, 1.0f);
    ImGui::SliderFloat3("Specular", glm::value_ptr(directional_light.specular_color), 0.0f, 1.0f);
    ImGui::End();

    ImGui::Begin("Point Light");
    ImGui::SliderFloat3("Position", glm::value_ptr(point_light.position), -30.0f, 30.0f);
    ImGui::SliderFloat3("Ambient", glm::value_ptr(point_light.ambient_color), 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse", glm::value_ptr(point_light.diffuse_color), 0.0f, 1.0f);
    ImGui::SliderFloat3("Specular", glm::value_ptr(point_light.specular_color), 0.0f, 1.0f);
    ImGui::SliderFloat("Falloff L.", &point_light.falloff_linear, 0.0001f, 1.0f);
    ImGui::SliderFloat("Falloff Q.", &point_light.falloff_quadratic, 0.00001f, 1.0f);
    ImGui::End();

    // ImGui::Begin("Transform");
    // ImGui::SliderFloat3("Position", glm::value_ptr(teapot.position), -30.0f, 30.0f);
    // ImGui::SliderFloat3("Rotation", glm::value_ptr(teapot.rotation), 0.0f, glm::two_pi<float>());
    // ImGui::End();
}

void GameScene::on_window_resized(const sm::WindowResizedEvent& event) {
    LOG_DEBUG("{}", event);
}
