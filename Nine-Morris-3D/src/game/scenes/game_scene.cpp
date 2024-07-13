#include "game/scenes/game_scene.hpp"

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

    setup_renderables();

    cam_controller = PointCameraController(
        cam,
        ctx.win.get_width(),
        ctx.win.get_height(),
        glm::vec3(0.0f),
        8.0f,
        47.0f,
        0.5f
    );

    cam_controller.connect_events(ctx.evt);

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

void GameScene::on_stop() {
    cam_controller.disconnect_events(ctx.evt);
}

void GameScene::on_update() {
    cam_controller.update_controls(ctx.delta, ctx.inp);
    cam_controller.update_camera(ctx.delta);

    ctx.scn.capture(cam, cam_controller.get_position());
    ctx.scn.capture(cam_2d);

    ctx.scn.add_light(directional_light);
    ctx.scn.add_light(point_light);

    if (sky) {
        ctx.scn.skybox(ctx.res.texture_cubemap["field"_H]);
    }

    if (blur) {
        ctx.scn.add_post_processing(ctx.global<Global>().blur_step);
    }

    ctx.scn.add_renderable(ground);
    ctx.scn.add_renderable(dragon1);
    ctx.scn.add_renderable(dragon2);
    ctx.scn.add_renderable(teapot);
    ctx.scn.add_renderable(cube);
    ctx.scn.add_renderable(brick);
    ctx.scn.add_renderable(lamp_stand);
    ctx.scn.add_renderable(lamp_bulb);
    ctx.scn.add_renderable(barrel);

    ctx.show_info_text();

    {
        ctx.scn.add_text(text1);
        ctx.scn.add_text(text2);
        ctx.scn.add_text(text3);

        {
            const auto [w, h] {ctx.res.font["sans"_H]->get_string_size("Some Text. Three spaces   !!?@#&^`~*&\"", text4.scale)};

            text4.text = "Some Text. Three spaces   !!?@#&^`~*&\"";
            text4.position = glm::vec2(static_cast<float>(ctx.win.get_width() - w), static_cast<float>(ctx.win.get_height() - h));
            text4.color = glm::vec3(1.0f, 0.1f, 0.1f);

            ctx.scn.add_text(text4);
        }

        {
            const auto [w, h] {ctx.res.font["sans"_H]->get_string_size("Simon Mărăcine ăîâșț ĂÎÂȘȚ", text4.scale)};

            text4.text = "Simon Mărăcine ăîâșț ĂÎÂȘȚ";
            text4.position = glm::vec2(static_cast<float>(ctx.win.get_width() / 2 - w / 2), static_cast<float>(ctx.win.get_height() / 2 - h / 2));
            text4.color = glm::vec3(0.8f, 0.1f, 0.9f);

            ctx.scn.add_text(text4);
        }

        {
            const auto [_, h] {ctx.res.font["sans"_H]->get_string_size("Text that spans\nmultiple lines\nlike that.", text4.scale)};

            text4.text = "Text that spans\nmultiple lines\nlike that.";
            text4.position = glm::vec2(0.0f, static_cast<float>(ctx.win.get_height() - h));
            text4.color = glm::vec3(0.0f, 1.0f, 0.0f);

            ctx.scn.add_text(text4);
        }

        {
            const auto [w, _] {ctx.res.font["sans"_H]->get_string_size("Another\ntext\nwith multiple\nlines.", text4.scale)};

            text4.text = "Another\ntext\nwith multiple\nlines.";
            text4.position = glm::vec2(static_cast<float>(ctx.win.get_width() - w), 0.0f);
            text4.color = glm::vec3(0.0f, 0.0f, 1.0f);

            ctx.scn.add_text(text4);
        }
    }

    ctx.scn.add_quad(wait);
    ctx.scn.add_quad(white);

    // Origin
    ctx.scn.debug_add_line(glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ctx.scn.debug_add_line(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ctx.scn.debug_add_line(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Whatever
    ctx.scn.debug_add_lines(
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
    ctx.scn.debug_add_lamp(point_light.position, point_light.diffuse_color);

    // Whatever part two
    ctx.scn.debug_add_point(glm::vec3(0.0f, -3.0f, 4.0f), glm::vec3(0.0f, 1.0f, 1.0f));

    ctx.scn.shadow(shadow_box);
}

void GameScene::on_fixed_update() {
    cam_controller.update_friction();
}

void GameScene::on_imgui_update() {
    ImGui::Begin("Features");
    if (ImGui::Checkbox("Sync", &sync)) {
        ctx.win.set_vsync(1 ? sync : 0);
    }
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
    ctx.load_model(ctx.fs.path_assets("models/dragon.obj"), "default", sm::Mesh::Type::PN);
    ctx.load_model(ctx.fs.path_assets("models/teapot.obj"), sm::Mesh::DEFAULT_OBJECT, sm::Mesh::Type::PN);
    ctx.load_model(ctx.fs.path_assets("models/cube.obj"), "Cube", sm::Mesh::Type::PN);
    ctx.load_model(ctx.fs.path_assets("models/brick.obj"), "Brick", sm::Mesh::Type::PNT);
    ctx.load_model("lamp_stand", ctx.fs.path_assets("models/lamp.obj"), "Stand", sm::Mesh::Type::PNT);
    ctx.load_model("lamp_bulb", ctx.fs.path_assets("models/lamp.obj"), "Bulb", sm::Mesh::Type::P);
    ctx.load_model(ctx.fs.path_assets("models/ground.obj"), "Cube", sm::Mesh::Type::PN);
    ctx.load_model(ctx.fs.path_assets("models/barrel.obj"), "Mesh_Mesh_Cylinder.001", sm::Mesh::Type::PNTT);
}

void GameScene::load_textures() {
    ctx.load_texture(ctx.fs.path_assets("textures/brick-texture3.png"), {}, {});
    ctx.load_texture(ctx.fs.path_assets("textures/indicator/wait_indicator.png"), {}, {});

    {
        sm::TexturePostProcessing post_processing;
        post_processing.size = sm::Size::Half;

        ctx.load_texture(ctx.fs.path_assets("textures/indicator/white_indicator.png"), post_processing, {});
    }

    ctx.load_texture(ctx.fs.path_assets("textures/lamp-texture.png"), {}, {});
    ctx.load_texture(ctx.fs.path_assets("textures/barrel.png"), {}, {});
    ctx.load_texture(ctx.fs.path_assets("textures/barrelNormal.png"), {}, {});

    {
        sm::TexturePostProcessing processing;
        processing.size = sm::Size::Half;  // Half looks better

        ctx.load_texture_cubemap(
            "field",
            {
                ctx.fs.path_assets("textures/skybox/field/px.png"),
                ctx.fs.path_assets("textures/skybox/field/nx.png"),
                ctx.fs.path_assets("textures/skybox/field/py.png"),
                ctx.fs.path_assets("textures/skybox/field/ny.png"),
                ctx.fs.path_assets("textures/skybox/field/pz.png"),
                ctx.fs.path_assets("textures/skybox/field/nz.png")
            },
            processing
        );
    }
}

void GameScene::load_materials() {
    ctx.load_material(sm::MaterialType::PhongShadow, sm::Material::CastShadow);
    ctx.load_material(sm::MaterialType::PhongDiffuseShadow, sm::Material::CastShadow);
    ctx.load_material(sm::MaterialType::PhongDiffuseNormalShadow, sm::Material::CastShadow);
    ctx.load_material(sm::MaterialType::Flat, sm::Material::CastShadow);
}

void GameScene::load_material_instances() {
    {
        auto material_instance {ctx.res.material_instance.load("dragon1"_H, ctx.res.material["phong_shadow"_H])};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(1.0f, 1.0f, 0.0f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(1.0f, 1.0f, 0.0f));
        material_instance->set_float("u_material.shininess"_H, 32.0f);
        material_instance->flags |= sm::Material::Outline;
    }

    {
        auto material_instance {ctx.res.material_instance.load("dragon2"_H, ctx.res.material["phong_shadow"_H])};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(1.0f, 0.2f, 0.1f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(1.0f, 1.0f, 0.0f));
        material_instance->set_float("u_material.shininess"_H, 32.0f);
    }

    {
        auto material_instance {ctx.res.material_instance.load("teapot"_H, ctx.res.material["phong_shadow"_H])};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(0.7f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.7f));
        material_instance->set_float("u_material.shininess"_H, 64.0f);
        material_instance->flags |= sm::Material::DisableBackFaceCulling;
    }

    {
        auto material_instance {ctx.res.material_instance.load("cube"_H, ctx.res.material["phong_shadow"_H])};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(1.0f, 0.0f, 0.0f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.8f));
        material_instance->set_float("u_material.shininess"_H, 128.0f);
    }

    {
        auto material_instance {ctx.res.material_instance.load("brick"_H, ctx.res.material["phong_diffuse_shadow"_H])};
        material_instance->set_texture("u_material.ambient_diffuse"_H, ctx.res.texture["brick-texture3.png"_H], 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.5f));
        material_instance->set_float("u_material.shininess"_H, 64.0f);
    }

    {
        auto material_instance {ctx.res.material_instance.load("lamp_stand"_H, ctx.res.material["phong_diffuse_shadow"_H])};
        material_instance->set_texture("u_material.ambient_diffuse"_H, ctx.res.texture["lamp-texture.png"_H], 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.5f));
        material_instance->set_float("u_material.shininess"_H, 64.0f);
    }

    {
        auto material_instance {ctx.res.material_instance.load("lamp_bulb"_H, ctx.res.material["flat"_H])};
        material_instance->set_vec3("u_material.color"_H, glm::vec3(1.0f));
    }

    {
        auto material_instance {ctx.res.material_instance.load("ground"_H, ctx.res.material["phong_shadow"_H])};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(0.7f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.4f));
        material_instance->set_float("u_material.shininess"_H, 16.0f);
    }

    {
        auto material_instance {ctx.res.material_instance.load("barrel"_H, ctx.res.material["phong_diffuse_normal_shadow"_H])};
        material_instance->set_texture("u_material.ambient_diffuse"_H, ctx.res.texture["barrel.png"_H], 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.9f));
        material_instance->set_float("u_material.shininess"_H, 32.0f);
        material_instance->set_texture("u_material.normal"_H, ctx.res.texture["barrelNormal.png"_H], 1);
    }
}

void GameScene::setup_renderables() {
    ground = sm::Renderable(ctx.res.mesh["ground.obj"_H], ctx.res.vertex_array["ground.obj"_H], ctx.res.material_instance["ground"_H]);
    ground.transform.position = glm::vec3(0.0f, -1.0f, 0.0f);
    ground.transform.scale = 2.0f;

    dragon1 = sm::Renderable(ctx.res.mesh["dragon.obj"_H], ctx.res.vertex_array["dragon.obj"_H], ctx.res.material_instance["dragon1"_H]);
    dragon1.transform.scale = 0.7f;
    dragon1.outline.color = glm::vec3(0.2f, 0.1f, 1.0f);
    dragon1.outline.thickness = 0.2f;

    dragon2 = sm::Renderable(ctx.res.mesh["dragon.obj"_H], ctx.res.vertex_array["dragon.obj"_H], ctx.res.material_instance["dragon2"_H]);
    dragon2.transform.position = glm::vec3(4.0f, 0.0, 0.0f);
    dragon2.transform.scale = 0.2f;

    teapot = sm::Renderable(ctx.res.mesh["teapot.obj"_H], ctx.res.vertex_array["teapot.obj"_H], ctx.res.material_instance["teapot"_H]);
    teapot.transform.position = glm::vec3(2.6f, 0.0, -7.0f);
    teapot.transform.rotation = glm::vec3(0.0f, 5.3f, 0.0f);

    cube = sm::Renderable(ctx.res.mesh["cube.obj"_H], ctx.res.vertex_array["cube.obj"_H], ctx.res.material_instance["cube"_H]);
    cube.transform.position = glm::vec3(5.0f, 2.0f, -2.0f);
    cube.transform.scale = 0.8f;

    brick = sm::Renderable(ctx.res.mesh["brick.obj"_H], ctx.res.vertex_array["brick.obj"_H], ctx.res.material_instance["brick"_H]);
    brick.transform.position = glm::vec3(6.0f);
    brick.transform.rotation = glm::vec3(10.0f);

    lamp_stand = sm::Renderable(ctx.res.mesh["lamp_stand"_H], ctx.res.vertex_array["lamp_stand"_H], ctx.res.material_instance["lamp_stand"_H]);
    lamp_stand.transform.position = glm::vec3(-6.0f, 0.0f, -6.0f);

    lamp_bulb = sm::Renderable(ctx.res.mesh["lamp_bulb"_H], ctx.res.vertex_array["lamp_bulb"_H], ctx.res.material_instance["lamp_bulb"_H]);
    lamp_bulb.transform.position = glm::vec3(-6.0f, 0.0f, -6.0f);

    barrel = sm::Renderable(ctx.res.mesh["barrel.obj"_H], ctx.res.vertex_array["barrel.obj"_H], ctx.res.material_instance["barrel"_H]);
    barrel.transform.position = glm::vec3(-7.0f, 4.5f, 6.0f);
    barrel.transform.scale = 0.5f;

    text1.font = ctx.res.font["sans"_H];
    text1.text = "The quick brown fox jumps over the lazy dog.";
    text1.color = glm::vec3(0.7f);

    text2 = text1;
    text2.position = glm::vec2(200.0f);
    text2.color = glm::vec3(0.8f, 0.7f, 0.1f);

    text3 = text1;
    text3.position = glm::vec2(200.0f, 100.0f);
    text3.color = glm::vec3(0.0f, 1.0f, 1.0f);

    text4.font = ctx.res.font["sans"_H];

    wait.texture = ctx.res.texture["wait_indicator.png"_H];
    wait.position = glm::vec2(70.0f);

    white.texture = ctx.res.texture["white_indicator.png"_H];
    white.position = glm::vec2(210.0f, 210.0f);
}
