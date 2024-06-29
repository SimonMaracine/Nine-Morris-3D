#include "game/scenes/game_scene.hpp"

#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <nine_morris_3d_engine/external/resmanager.h++>
#include <nine_morris_3d_engine/external/imgui.h++>

#include "game/global.hpp"
#include "game/game.hpp"

void GameScene::on_start() {
    ctx.tsk.add("test"_H, [this](const sm::Task& task) {
        if (task.get_total_time() > 3.0) {
            LOG_DEBUG("Done");

            return sm::Task::Result::Done;
        }

        return sm::Task::Result::Continue;
    });

    ctx.evt.connect<sm::WindowResizedEvent, &GameScene::on_window_resized>(this);

    sm::opengl::clear_color(0.1f, 0.05f, 0.1f);

    load_models();
    load_textures();
    load_materials();
    load_material_instances();

    {
        sm::FontSpecification specification;
        specification.bitmap_size = 512;
        specification.size_height = 40.0f;

        auto font {ctx.res.font.load(
            "sans"_H,
            sm::utils::read_file(ctx.fs.path_assets("fonts/OpenSans/OpenSans-Regular.ttf")),
            specification
        )};

        font->begin_baking();
        font->bake_ascii();
        font->bake_characters(256, 127);
        font->bake_characters(192, 22);
        font->bake_characters(223, 23);
        font->bake_characters(536, 4);
        font->end_baking("sans");
    }

    cam_controller = PointCameraController(
        &cam,
        ctx.win.get_width(),
        ctx.win.get_height(),
        LENS_FOV,
        LENS_NEAR,
        LENS_FAR,
        glm::vec3(0.0f),
        8.0f,
        47.0f,
        0.5f
    );

    cam_controller.connect_events(ctx);

    cam_2d.set_projection(0, ctx.win.get_width(), 0, ctx.win.get_height());

    directional_light.direction = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
    directional_light.ambient_color = glm::vec3(0.1f);
    directional_light.diffuse_color = glm::vec3(0.9f);
    directional_light.specular_color = glm::vec3(1.0f);

    point_light.position = glm::vec3(3.0f, 3.0f, 1.0f);
    point_light.ambient_color = glm::vec3(0.1f, 0.08f, 0.02f);
    point_light.diffuse_color = glm::vec3(0.9f, 0.8f, 0.2f);
    point_light.specular_color = glm::vec3(1.0f);
    point_light.falloff_linear = 0.09f;
    point_light.falloff_quadratic = 0.032f;
}

void GameScene::on_update() {
    cam_controller.update_controls(ctx.delta, ctx.inp);
    cam_controller.update_camera(ctx.delta);
    cam_controller.update_friction();

    ctx.rnd.capture(cam, cam_controller.get_position());
    ctx.rnd.capture(cam_2d);

    ctx.rnd.add_light(directional_light);
    ctx.rnd.add_light(point_light);
    ctx.rnd.shadows(shadow_left, shadow_right, shadow_bottom, shadow_top, 1.0f, shadow_far, directional_light.direction * -30.0f);

    if (sky) {
        ctx.rnd.skybox(ctx.res.texture_cubemap["field"_H]);
    }

    if (blur) {
        ctx.rnd.add_post_processing(ctx.global<Global>().blur_step);
    }

    {
        sm::Renderable ground;
        ground.vertex_array = ctx.res.vertex_array["ground"_H];
        ground.material = ctx.res.material_instance["ground"_H];
        ground.transform.position = glm::vec3(0.0f, -1.0f, 0.0f);
        ground.transform.scale = 2.0f;

        ctx.rnd.add_renderable(ground);
    }

    {
        sm::Renderable dragon;
        dragon.vertex_array = ctx.res.vertex_array["dragon"_H];
        dragon.material = ctx.res.material_instance["dragon1"_H];
        dragon.transform.scale = 0.7f;
        dragon.outline.color = glm::vec3(0.2f, 0.1f, 1.0f);
        dragon.outline.offset = glm::vec3(0.04f, -0.2f, 0.0f);

        ctx.rnd.add_renderable(dragon);

        dragon.material = ctx.res.material_instance["dragon2"_H];
        dragon.transform.position = glm::vec3(4.0f, 0.0, 0.0f);
        dragon.transform.scale = 0.2f;

        ctx.rnd.add_renderable(dragon);
    }

    {
        sm::Renderable teapot;
        teapot.vertex_array = ctx.res.vertex_array["teapot"_H];
        teapot.material = ctx.res.material_instance["teapot"_H];
        teapot.transform.position = glm::vec3(2.6f, 0.0, -7.0f);
        teapot.transform.rotation = glm::vec3(0.0f, 5.3f, 0.0f);

        ctx.rnd.add_renderable(teapot);
    }

    {
        sm::Renderable cube;
        cube.vertex_array = ctx.res.vertex_array["cube"_H];
        cube.material = ctx.res.material_instance["cube"_H];
        cube.transform.position = glm::vec3(5.0f, 2.0f, -2.0f);
        cube.transform.scale = 0.8f;

        ctx.rnd.add_renderable(cube);
    }

    {
        sm::Renderable brick;
        brick.vertex_array = ctx.res.vertex_array["brick"_H];
        brick.material = ctx.res.material_instance["brick"_H];
        brick.transform.position = glm::vec3(6.0f);
        brick.transform.rotation = glm::vec3(10.0f);

        ctx.rnd.add_renderable(brick);
    }

    {
        sm::Renderable lamp;
        lamp.vertex_array = ctx.res.vertex_array["lamp_stand"_H];
        lamp.material = ctx.res.material_instance["lamp_stand"_H];
        lamp.transform.position = glm::vec3(-6.0f, 0.0f, -6.0f);

        ctx.rnd.add_renderable(lamp);

        lamp.vertex_array = ctx.res.vertex_array["lamp_bulb"_H];
        lamp.material = ctx.res.material_instance["lamp_bulb"_H];

        ctx.rnd.add_renderable(lamp);
    }

    ctx.add_info_text();

    {
        sm::Text test;
        test.font = ctx.res.font["sans"_H];
        test.text = "The quick brown fox jumps over the lazy dog.";
        test.color = glm::vec3(0.7f);

        ctx.rnd.add_text(test);

        test.position = glm::vec2(200.0f) + pos;
        test.color = glm::vec3(0.8f, 0.7f, 0.1f);

        ctx.rnd.add_text(test);

        test.position = glm::vec2(200.0f, 100.0f) + pos;
        test.scale = scl;
        test.color = glm::vec3(0.0f, 1.0f, 1.0f);

        ctx.rnd.add_text(test);

        {
            const auto [w, h] {ctx.res.font["sans"_H]->get_string_size("Some Text. Three spaces   !!?@#&^`~*&\"", scl)};

            test.text = "Some Text. Three spaces   !!?@#&^`~*&\"";
            test.position = glm::vec2(static_cast<float>(ctx.win.get_width() - w), static_cast<float>(ctx.win.get_height() - h));
            test.color = glm::vec3(1.0f, 0.1f, 0.1f);

            ctx.rnd.add_text(test);
        }

        {
            const auto [w, h] {ctx.res.font["sans"_H]->get_string_size("Simon Mărăcine ăîâșț ĂÎÂȘȚ", scl)};

            test.text = "Simon Mărăcine ăîâșț ĂÎÂȘȚ";
            test.position = glm::vec2(static_cast<float>(ctx.win.get_width() / 2 - w / 2), static_cast<float>(ctx.win.get_height() / 2 - h / 2));
            test.color = glm::vec3(0.8f, 0.1f, 0.9f);

            ctx.rnd.add_text(test);
        }

        {
            const auto [_, h] {ctx.res.font["sans"_H]->get_string_size("Text that spans\nmultiple lines\nlike that.", scl)};

            test.text = "Text that spans\nmultiple lines\nlike that.";
            test.position = glm::vec2(0.0f, static_cast<float>(ctx.win.get_height() - h));
            test.color = glm::vec3(0.0f, 1.0f, 0.0f);

            ctx.rnd.add_text(test);
        }

        {
            const auto [w, _] {ctx.res.font["sans"_H]->get_string_size("Another\ntext\nwith multiple\nlines.", scl)};

            test.text = "Another\ntext\nwith multiple\nlines.";
            test.position = glm::vec2(static_cast<float>(ctx.win.get_width() - w), 0.0f);
            test.color = glm::vec3(0.0f, 0.0f, 1.0f);

            ctx.rnd.add_text(test);
        }
    }

    {
        sm::Quad quad;
        quad.texture = ctx.res.texture["wait_indicator"_H];
        quad.position = glm::vec2(70.0f) + pos;
        quad.scale = glm::vec2(scl);

        ctx.rnd.add_quad(quad);
    }

    {
        sm::Quad quad;
        quad.texture = ctx.res.texture["white_indicator"_H];
        quad.scale = glm::vec2(scl);
        quad.position = glm::vec2(210.0f, 210.0f) + pos;

        ctx.rnd.add_quad(quad);
    }

    // Origin
    ctx.rnd.debug_add_line(glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ctx.rnd.debug_add_line(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ctx.rnd.debug_add_line(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Whatever
    ctx.rnd.debug_add_lines(
        {
            glm::vec3(0.0f, 6.0f, 0.0f),
            glm::vec3(2.0f, 7.0f, 0.0f),
            glm::vec3(4.0f, 6.0f, 2.0f),
            glm::vec3(3.0f, 8.0f, 0.0f),
            glm::vec3(0.0f, 11.0f, -4.0f)
        },
        glm::vec3(0.0f, 0.0f, 0.0f)
    );

    // Point light
    ctx.rnd.debug_add_lamp(point_light.position, point_light.diffuse_color);

    // Whatever part two
    ctx.rnd.debug_add_point(glm::vec3(0.0f, -3.0f, 4.0f), glm::vec3(0.0f, 1.0f, 1.0f));

    draw_bounding_box(shadow_left, shadow_right, shadow_bottom, shadow_top, 1.0f, shadow_far, directional_light.direction * -30.0f, directional_light.direction);
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
    ImGui::SliderFloat("Falloff L", &point_light.falloff_linear, 0.0001f, 1.0f);
    ImGui::SliderFloat("Falloff Q", &point_light.falloff_quadratic, 0.00001f, 1.0f);
    ImGui::End();

    ImGui::Begin("Shadow");
    ImGui::SliderFloat("Left", &shadow_left, -100.0f, 100.0f);
    ImGui::SliderFloat("Right", &shadow_right, -100.0f, 100.0f);
    ImGui::SliderFloat("Bottom", &shadow_bottom, -100.0f, 100.0f);
    ImGui::SliderFloat("Top", &shadow_top, -100.0f, 100.0f);
    ImGui::SliderFloat("Far", &shadow_far, 2.0f, 150.0f);
    ImGui::End();

    ImGui::Begin("Quads");
    ImGui::SliderFloat2("Position", glm::value_ptr(pos), -500.0f, 500.0f);
    ImGui::SliderFloat("Scale", &scl, 0.0f, 3.0f);
    ImGui::End();

    ImGui::Begin("Features");
    ImGui::Checkbox("Skybox", &sky);
    if (ImGui::Checkbox("Blur", &blur)) {
        if (blur) {
            create_post_processing(ctx);
        } else {
            ctx.global<Global>().blur_step = nullptr;
        }
    }
    if (ImGui::Checkbox("Outline", &outline)) {
        if (outline) {
            ctx.res.material_instance["dragon1"_H]->flags ^= sm::Material::Outline;
        } else {
            ctx.res.material_instance["dragon1"_H]->flags ^= sm::Material::Outline;
        }
    }
    ImGui::End();
}

void GameScene::on_window_resized(const sm::WindowResizedEvent& event) {
    cam.set_projection(event.width, event.height, LENS_FOV, LENS_NEAR, LENS_FAR);
    cam_2d.set_projection(0, event.width, 0, event.height);
}

void GameScene::load_models() {
    {
        const sm::Mesh mesh {
            sm::utils::read_file(ctx.fs.path_assets("models/dragon.obj")),
            "default",
            sm::Mesh::Type::PN
        };

        auto vertex_buffer {std::make_shared<sm::GlVertexBuffer>(
            mesh.get_vertices(),
            mesh.get_vertices_size()
        )};

        auto index_buffer {std::make_shared<sm::GlIndexBuffer>(
            mesh.get_indices(),
            mesh.get_indices_size()
        )};

        auto vertex_array {ctx.res.vertex_array.load("dragon"_H)};
        vertex_array->configure([&](sm::GlVertexArray* va) {
            sm::VertexBufferLayout layout;
            layout.add(0, sm::VertexBufferLayout::Float, 3);
            layout.add(1, sm::VertexBufferLayout::Float, 3);

            va->add_vertex_buffer(vertex_buffer, layout);
            va->add_index_buffer(index_buffer);
        });
    }

    {
        const sm::Mesh mesh {
            sm::utils::read_file(ctx.fs.path_assets("models/teapot.obj")),
            sm::Mesh::DEFAULT_OBJECT,
            sm::Mesh::Type::PN
        };

        auto vertex_buffer {std::make_shared<sm::GlVertexBuffer>(
            mesh.get_vertices(),
            mesh.get_vertices_size()
        )};

        auto index_buffer {std::make_shared<sm::GlIndexBuffer>(
            mesh.get_indices(),
            mesh.get_indices_size()
        )};

        auto vertex_array {ctx.res.vertex_array.load("teapot"_H)};
        vertex_array->configure([&](sm::GlVertexArray* va) {
            sm::VertexBufferLayout layout;
            layout.add(0, sm::VertexBufferLayout::Float, 3);
            layout.add(1, sm::VertexBufferLayout::Float, 3);

            va->add_vertex_buffer(vertex_buffer, layout);
            va->add_index_buffer(index_buffer);
        });
    }

    {
        const sm::Mesh mesh {
            sm::utils::read_file(ctx.fs.path_assets("models/cube.obj")),
            "Cube",
            sm::Mesh::Type::PN
        };

        auto vertex_buffer {std::make_shared<sm::GlVertexBuffer>(
            mesh.get_vertices(),
            mesh.get_vertices_size()
        )};

        auto index_buffer {std::make_shared<sm::GlIndexBuffer>(
            mesh.get_indices(),
            mesh.get_indices_size()
        )};

        auto vertex_array {ctx.res.vertex_array.load("cube"_H)};
        vertex_array->configure([&](sm::GlVertexArray* va) {
            sm::VertexBufferLayout layout;
            layout.add(0, sm::VertexBufferLayout::Float, 3);
            layout.add(1, sm::VertexBufferLayout::Float, 3);

            va->add_vertex_buffer(vertex_buffer, layout);
            va->add_index_buffer(index_buffer);
        });
    }

    {
        const sm::Mesh mesh {
            sm::utils::read_file(ctx.fs.path_assets("models/brick.obj")),
            "Brick",
            sm::Mesh::Type::PNT
        };

        auto vertex_buffer {std::make_shared<sm::GlVertexBuffer>(
            mesh.get_vertices(),
            mesh.get_vertices_size()
        )};

        auto index_buffer {std::make_shared<sm::GlIndexBuffer>(
            mesh.get_indices(),
            mesh.get_indices_size()
        )};

        auto vertex_array {ctx.res.vertex_array.load("brick"_H)};
        vertex_array->configure([&](sm::GlVertexArray* va) {
            sm::VertexBufferLayout layout;
            layout.add(0, sm::VertexBufferLayout::Float, 3);
            layout.add(1, sm::VertexBufferLayout::Float, 3);
            layout.add(2, sm::VertexBufferLayout::Float, 2);

            va->add_vertex_buffer(vertex_buffer, layout);
            va->add_index_buffer(index_buffer);
        });
    }

    {
        const sm::Mesh mesh {
            sm::utils::read_file(ctx.fs.path_assets("models/lamp.obj")),
            "Stand",
            sm::Mesh::Type::PNT
        };

        auto vertex_buffer {std::make_shared<sm::GlVertexBuffer>(
            mesh.get_vertices(),
            mesh.get_vertices_size()
        )};

        auto index_buffer {std::make_shared<sm::GlIndexBuffer>(
            mesh.get_indices(),
            mesh.get_indices_size()
        )};

        auto vertex_array {ctx.res.vertex_array.load("lamp_stand"_H)};
        vertex_array->configure([&](sm::GlVertexArray* va) {
            sm::VertexBufferLayout layout;
            layout.add(0, sm::VertexBufferLayout::Float, 3);
            layout.add(1, sm::VertexBufferLayout::Float, 3);
            layout.add(2, sm::VertexBufferLayout::Float, 2);

            va->add_vertex_buffer(vertex_buffer, layout);
            va->add_index_buffer(index_buffer);
        });
    }

    {
        const sm::Mesh mesh {
            sm::utils::read_file(ctx.fs.path_assets("models/lamp.obj")),
            "Bulb",
            sm::Mesh::Type::P
        };

        auto vertex_buffer {std::make_shared<sm::GlVertexBuffer>(
            mesh.get_vertices(),
            mesh.get_vertices_size()
        )};

        auto index_buffer {std::make_shared<sm::GlIndexBuffer>(
            mesh.get_indices(),
            mesh.get_indices_size()
        )};

        auto vertex_array {ctx.res.vertex_array.load("lamp_bulb"_H)};
        vertex_array->configure([&](sm::GlVertexArray* va) {
            sm::VertexBufferLayout layout;
            layout.add(0, sm::VertexBufferLayout::Float, 3);

            va->add_vertex_buffer(vertex_buffer, layout);
            va->add_index_buffer(index_buffer);
        });
    }

    {
        const sm::Mesh mesh {
            sm::utils::read_file(ctx.fs.path_assets("models/ground.obj")),
            "Cube",
            sm::Mesh::Type::PN
        };

        auto vertex_buffer {std::make_shared<sm::GlVertexBuffer>(
            mesh.get_vertices(),
            mesh.get_vertices_size()
        )};

        auto index_buffer {std::make_shared<sm::GlIndexBuffer>(
            mesh.get_indices(),
            mesh.get_indices_size()
        )};

        auto vertex_array {ctx.res.vertex_array.load("ground"_H)};
        vertex_array->configure([&](sm::GlVertexArray* va) {
            sm::VertexBufferLayout layout;
            layout.add(0, sm::VertexBufferLayout::Float, 3);
            layout.add(1, sm::VertexBufferLayout::Float, 3);

            va->add_vertex_buffer(vertex_buffer, layout);
            va->add_index_buffer(index_buffer);
        });
    }
}

void GameScene::load_textures() {
    {
        sm::TexturePostProcessing post_processing;
        post_processing.flip = true;

        auto data {ctx.res.texture_data.load(
            "brick-texture3"_H,
            sm::utils::read_file(ctx.fs.path_assets("textures/brick-texture3.png")),
            post_processing
        )};

        ctx.res.texture.load("brick"_H, data);
    }

    {
        sm::TexturePostProcessing post_processing;
        post_processing.flip = true;

        auto data {ctx.res.texture_data.load(
            "wait_indicator"_H,
            sm::utils::read_file(ctx.fs.path_assets("textures/indicator/wait_indicator.png")),
            post_processing
        )};

        ctx.res.texture.load("wait_indicator"_H, data);
    }

    {
        sm::TexturePostProcessing post_processing;
        post_processing.flip = true;
        post_processing.size = sm::Size::Half;

        auto data {ctx.res.texture_data.load(
            "white_indicator"_H,
            sm::utils::read_file(ctx.fs.path_assets("textures/indicator/white_indicator.png")),
            post_processing
        )};

        ctx.res.texture.load("white_indicator"_H, data);
    }

    {
        sm::TexturePostProcessing processing;
        processing.size = sm::Size::Half;  // Half looks better

        auto px {std::make_shared<sm::TextureData>(
            sm::utils::read_file(ctx.fs.path_assets("textures/skybox/field/px.png")),
            processing
        )};

        auto nx {std::make_shared<sm::TextureData>(
            sm::utils::read_file(ctx.fs.path_assets("textures/skybox/field/nx.png")),
            processing
        )};

        auto py {std::make_shared<sm::TextureData>(
            sm::utils::read_file(ctx.fs.path_assets("textures/skybox/field/py.png")),
            processing
        )};

        auto ny {std::make_shared<sm::TextureData>(
            sm::utils::read_file(ctx.fs.path_assets("textures/skybox/field/ny.png")),
            processing
        )};

        auto pz {std::make_shared<sm::TextureData>(
            sm::utils::read_file(ctx.fs.path_assets("textures/skybox/field/pz.png")),
            processing
        )};

        auto nz {std::make_shared<sm::TextureData>(
            sm::utils::read_file(ctx.fs.path_assets("textures/skybox/field/nz.png")),
            processing
        )};

        const std::initializer_list list {px, nx, py, ny, pz, nz};

        ctx.res.texture_cubemap.load("field"_H, list);
    }

    {
        sm::TexturePostProcessing post_processing;
        post_processing.flip = true;

        auto data {ctx.res.texture_data.load(
            "lamp"_H,
            sm::utils::read_file(ctx.fs.path_assets("textures/lamp-texture.png")),
            post_processing
        )};

        ctx.res.texture.load("lamp"_H, data);
    }
}

void GameScene::load_materials() {
    {
        auto shader {std::make_shared<sm::GlShader>(
            ctx.shd.load_shader(sm::utils::read_file(ctx.fs.path_assets("shaders/phong_shadows.vert"))),
            ctx.shd.load_shader(sm::utils::read_file(ctx.fs.path_assets("shaders/phong_shadows.frag")))
        )};

        ctx.rnd.register_shader(shader);

        auto material {ctx.res.material.load("phong_shadows"_H, shader, sm::Material::CastShadow)};
        material->add_uniform(sm::Material::Uniform::Vec3, "u_material.ambient_diffuse"_H);
        material->add_uniform(sm::Material::Uniform::Vec3, "u_material.specular"_H);
        material->add_uniform(sm::Material::Uniform::Float, "u_material.shininess"_H);
    }

    {
        auto shader {std::make_shared<sm::GlShader>(
            ctx.shd.load_shader(sm::utils::read_file(ctx.fs.path_assets("shaders/phong_textured_shadows.vert"))),
            ctx.shd.load_shader(sm::utils::read_file(ctx.fs.path_assets("shaders/phong_textured_shadows.frag")))
        )};

        ctx.rnd.register_shader(shader);

        auto material {ctx.res.material.load("phong_textured_shadows"_H, shader, sm::Material::CastShadow)};
        material->add_texture("u_material.ambient_diffuse"_H);
        material->add_uniform(sm::Material::Uniform::Vec3, "u_material.specular"_H);
        material->add_uniform(sm::Material::Uniform::Float, "u_material.shininess"_H);
    }

    {
        auto shader {std::make_shared<sm::GlShader>(
            ctx.shd.load_shader(sm::utils::read_file(ctx.fs.path_assets("shaders/flat.vert"))),
            ctx.shd.load_shader(sm::utils::read_file(ctx.fs.path_assets("shaders/flat.frag")))
        )};

        ctx.rnd.register_shader(shader);

        auto material {ctx.res.material.load("flat"_H, shader, sm::Material::CastShadow)};
        material->add_uniform(sm::Material::Uniform::Vec3, "u_material.color"_H);
    }
}

void GameScene::load_material_instances() {
    {
        auto material_instance {ctx.res.material_instance.load("dragon1"_H, ctx.res.material["phong_shadows"_H])};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(1.0f, 1.0f, 0.0f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(1.0f, 1.0f, 0.0f));
        material_instance->set_float("u_material.shininess"_H, 32.0f);
        material_instance->flags |= sm::Material::Outline;
    }

    {
        auto material_instance {ctx.res.material_instance.load("dragon2"_H, ctx.res.material["phong_shadows"_H])};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(1.0f, 0.2f, 0.1f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(1.0f, 1.0f, 0.0f));
        material_instance->set_float("u_material.shininess"_H, 32.0f);
    }

    {
        auto material_instance {ctx.res.material_instance.load("teapot"_H, ctx.res.material["phong_shadows"_H])};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(0.7f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.7f));
        material_instance->set_float("u_material.shininess"_H, 64.0f);
        material_instance->flags |= sm::Material::DisableBackFaceCulling;
    }

    {
        auto material_instance {ctx.res.material_instance.load("cube"_H, ctx.res.material["phong_shadows"_H])};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(1.0f, 0.0f, 0.0f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.8f));
        material_instance->set_float("u_material.shininess"_H, 128.0f);
    }

    {
        auto material_instance {ctx.res.material_instance.load("brick"_H, ctx.res.material["phong_textured_shadows"_H])};
        material_instance->set_texture("u_material.ambient_diffuse"_H, ctx.res.texture["brick"_H], 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.5f));
        material_instance->set_float("u_material.shininess"_H, 64.0f);
    }

    {
        auto material_instance {ctx.res.material_instance.load("lamp_stand"_H, ctx.res.material["phong_textured_shadows"_H])};
        material_instance->set_texture("u_material.ambient_diffuse"_H, ctx.res.texture["lamp"_H], 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.5f));
        material_instance->set_float("u_material.shininess"_H, 64.0f);
    }

    {
        auto material_instance {ctx.res.material_instance.load("lamp_bulb"_H, ctx.res.material["flat"_H])};
        material_instance->set_vec3("u_material.color"_H, glm::vec3(1.0f));
    }

    {
        auto material_instance {ctx.res.material_instance.load("ground"_H, ctx.res.material["phong_shadows"_H])};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(0.7f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.4f));
        material_instance->set_float("u_material.shininess"_H, 16.0f);
    }
}

void GameScene::draw_bounding_box(float left, float right, float bottom, float top, float near, float far, const glm::vec3& position, const glm::vec3& orientation) {
    const glm::vec3 color {1.0f};
    const glm::vec3 third {glm::cross(orientation, glm::vec3(0.0f, 1.0f, 0.0f))};
    const glm::vec3 third2 {-glm::cross(orientation, glm::cross(orientation, glm::vec3(0.0f, 1.0f, 0.0f)))};

    ctx.rnd.debug_add_line(
        position + glm::normalize(orientation) * near + glm::normalize(third) * left + glm::normalize(third2) * bottom,
        position + glm::normalize(orientation) * near + glm::normalize(third) * left + glm::normalize(third2) * top,
        color
    );

    ctx.rnd.debug_add_line(
        position + glm::normalize(orientation) * near + glm::normalize(third) * right + glm::normalize(third2) * bottom,
        position + glm::normalize(orientation) * near + glm::normalize(third) * right + glm::normalize(third2) * top,
        color
    );

    ctx.rnd.debug_add_line(
        position + glm::normalize(orientation) * near + glm::normalize(third2) * bottom + glm::normalize(third) * left,
        position + glm::normalize(orientation) * near + glm::normalize(third2) * bottom + glm::normalize(third) * right,
        color
    );

    ctx.rnd.debug_add_line(
        position + glm::normalize(orientation) * near + glm::normalize(third2) * top + glm::normalize(third) * left,
        position + glm::normalize(orientation) * near + glm::normalize(third2) * top + glm::normalize(third) * right,
        color
    );

    ctx.rnd.debug_add_line(
        position + glm::normalize(orientation) * far + glm::normalize(third) * left + glm::normalize(third2) * bottom,
        position + glm::normalize(orientation) * far + glm::normalize(third) * left + glm::normalize(third2) * top,
        color
    );

    ctx.rnd.debug_add_line(
        position + glm::normalize(orientation) * far + glm::normalize(third) * right + glm::normalize(third2) * bottom,
        position + glm::normalize(orientation) * far + glm::normalize(third) * right + glm::normalize(third2) * top,
        color
    );

    ctx.rnd.debug_add_line(
        position + glm::normalize(orientation) * far + glm::normalize(third2) * bottom + glm::normalize(third) * left,
        position + glm::normalize(orientation) * far + glm::normalize(third2) * bottom + glm::normalize(third) * right,
        color
    );

    ctx.rnd.debug_add_line(
        position + glm::normalize(orientation) * far + glm::normalize(third2) * top + glm::normalize(third) * left,
        position + glm::normalize(orientation) * far + glm::normalize(third2) * top + glm::normalize(third) * right,
        color
    );

    ctx.rnd.debug_add_line(
        position + glm::normalize(orientation) * near + glm::normalize(third) * left + glm::normalize(third2) * bottom,
        position + glm::normalize(orientation) * far + glm::normalize(third) * left + glm::normalize(third2) * bottom,
        color
    );

    ctx.rnd.debug_add_line(
        position + glm::normalize(orientation) * near + glm::normalize(third) * right + glm::normalize(third2) * bottom,
        position + glm::normalize(orientation) * far + glm::normalize(third) * right + glm::normalize(third2) * bottom,
        color
    );

    ctx.rnd.debug_add_line(
        position + glm::normalize(orientation) * near + glm::normalize(third) * left + glm::normalize(third2) * top,
        position + glm::normalize(orientation) * far + glm::normalize(third) * left + glm::normalize(third2) * top,
        color
    );

    ctx.rnd.debug_add_line(
        position + glm::normalize(orientation) * near + glm::normalize(third) * right + glm::normalize(third2) * top,
        position + glm::normalize(orientation) * far + glm::normalize(third) * right + glm::normalize(third2) * top,
        color
    );
}
