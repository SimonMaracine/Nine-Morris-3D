#include "scenes/game_scene.hpp"

#include <nine_morris_3d_engine/external/resmanager.h++>
#include <nine_morris_3d_engine/external/imgui.h++>
#include <nine_morris_3d_engine/external/glm.h++>

#include "global.hpp"
#include "game.hpp"
#include "game/ray.hpp"

void GameScene::on_start() {
    // ctx.add_task([this](const sm::Task& task, void*) {
    //     if (task.get_total_time() > 3.0) {
    //         LOG_INFO("Three second task done");

    //         return sm::Task::Result::Done;
    //     }

    //     return sm::Task::Result::Repeat;
    // });

    // ctx.add_task_async([this](sm::AsyncTask& async_task, void*) {
    //     using namespace std::chrono_literals;

    //     for (unsigned int i {0}; i < 6; i++) {
    //         std::this_thread::sleep_for(3s);
    //         LOG_INFO("Doing stuff...");

    //         if (async_task.stop_requested()) {
    //             async_task.set_done();
    //             return;
    //         }
    //     }

    //     ctx.add_task([this](const sm::Task&, void*) {
    //         LOG_INFO("Long running task done");

    //         return sm::Task::Result::Done;
    //     });

    //     async_task.set_done();
    // });

    // ctx.add_task_async([this](sm::AsyncTask& async_task, void*) {
    //     try {
    //         load_heavy_resources();
    //     } catch (const sm::RuntimeError&) {
    //         async_task.set_done(std::current_exception());
    //         return;
    //     }

    //     async_task.set_done();
    // });

    ctx.connect_event<sm::WindowResizedEvent, &GameScene::on_window_resized>(this);
    ctx.connect_event<sm::KeyReleasedEvent, &GameScene::on_key_released>(this);
    ctx.connect_event<sm::MouseButtonReleasedEvent, &GameScene::on_mouse_button_released>(this);

    ctx.set_clear_color(glm::vec3(0.1f, 0.1f, 0.1f));

    // setup_texts();
    // setup_quads();

    cam_controller = PointCameraController(
        cam,
        ctx.get_window_width(),
        ctx.get_window_height(),
        glm::vec3(0.0f),
        20.0f,
        47.0f,
        0.5f
    );

    cam_controller.connect_events(ctx);

    cam_2d.set_projection(0, ctx.get_window_width(), 0, ctx.get_window_height());

    setup_skybox();
    setup_lights();
    setup_renderables();

    // ctx.set_color_correction(color_correction);
}

void GameScene::on_stop() {
    cam_controller.disconnect_events(ctx);
    ctx.disconnect_events(this);
}

void GameScene::on_update() {
    cam_controller.update_controls(ctx.get_delta(), ctx);
    cam_controller.update_camera(ctx.get_delta());

    // sm::listener::set_position(cam_controller.get_position());

    ctx.capture(cam, cam_controller.get_position());
    ctx.capture(cam_2d);

    ctx.add_light(directional_light);

    const auto ray {cast_mouse_ray(ctx, cam)};

    board.update(ctx, ray, cam_controller.get_position());

    // ctx.add_light(point_light);

    // if (sky && field) {
    //     ctx.skybox(field);
    // }

    ctx.skybox(field);

    // if (blur) {
    //     ctx.add_post_processing(ctx.global<Global>().blur_step);
    // }

    // if (ground) ctx.add_renderable(ground);
    // if (dragon1) ctx.add_renderable(dragon1);
    // if (dragon2) ctx.add_renderable(dragon2);
    // if (teapot) ctx.add_renderable(teapot);
    // if (cube) ctx.add_renderable(cube);
    // if (brick) ctx.add_renderable(brick);
    // if (lamp_stand) ctx.add_renderable(lamp_stand);
    // if (lamp_bulb) ctx.add_renderable(lamp_bulb);
    // if (barrel) ctx.add_renderable(barrel);

    // for (auto& brick : textured_bricks) {
    //     if (brick) ctx.add_renderable(brick);
    // }

    if (ui.get_show_information()) {
        ctx.show_info_text();
    }

    // {
    //     ctx.add_text(text1);
    //     ctx.add_text(text2);
    //     ctx.add_text(text3);

    //     {
    //         const auto [w, h] {sans->get_string_size("Some Text. Three spaces   !!?@#&^`~*&\"", text4.scale)};

    //         text4.text = "Some Text. Three spaces   !!?@#&^`~*&\"";
    //         text4.position = glm::vec2(static_cast<float>(ctx.get_window_width() - w), static_cast<float>(ctx.get_window_height() - h));
    //         text4.color = glm::vec3(1.0f, 0.1f, 0.1f);

    //         ctx.add_text(text4);
    //     }

    //     {
    //         const auto [w, h] {sans->get_string_size("Simon Mărăcine ăîâșț ĂÎÂȘȚ", text4.scale)};

    //         text4.text = "Simon Mărăcine ăîâșț ĂÎÂȘȚ";
    //         text4.position = glm::vec2(static_cast<float>(ctx.get_window_width() / 2 - w / 2), static_cast<float>(ctx.get_window_height() / 2 - h / 2));
    //         text4.color = glm::vec3(0.8f, 0.1f, 0.9f);

    //         ctx.add_text(text4);
    //     }

    //     {
    //         const auto [_, h] {sans->get_string_size("Text that spans\nmultiple lines\nlike that.", text4.scale)};

    //         text4.text = "Text that spans\nmultiple lines\nlike that.";
    //         text4.position = glm::vec2(0.0f, static_cast<float>(ctx.get_window_height() - h));
    //         text4.color = glm::vec3(0.0f, 1.0f, 0.0f);

    //         ctx.add_text(text4);
    //     }

    //     {
    //         const auto [w, _] {sans->get_string_size("Another\ntext\nwith multiple\nlines.", text4.scale)};

    //         text4.text = "Another\ntext\nwith multiple\nlines.";
    //         text4.position = glm::vec2(static_cast<float>(ctx.get_window_width() - w), 0.0f);
    //         text4.color = glm::vec3(0.0f, 0.0f, 1.0f);

    //         ctx.add_text(text4);
    //     }
    // }

    // ctx.add_quad(wait);
    // ctx.add_quad(white);

    // Origin
    ctx.debug_add_line(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ctx.debug_add_line(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ctx.debug_add_line(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Whatever
    // ctx.debug_add_lines(
    //     {
    //         glm::vec3(0.0f, 6.0f, 0.0f),
    //         glm::vec3(2.0f, 7.0f, 0.0f),
    //         glm::vec3(4.0f, 6.0f, 2.0f),
    //         glm::vec3(3.0f, 8.0f, 0.0f),
    //         glm::vec3(0.0f, 11.0f, -4.0f)
    //     },
    //     glm::vec3(0.0f, 0.0f, 0.0f)
    // );

    // Point light
    // ctx.debug_add_lamp(point_light.position, point_light.diffuse_color);

    // Whatever part two
    // ctx.debug_add_point(glm::vec3(0.0f, -3.0f, 4.0f), glm::vec3(0.0f, 1.0f, 1.0f));

    ctx.shadow(shadow_box);
}

void GameScene::on_fixed_update() {
    cam_controller.update_friction();
    board.update_movement();
}

void GameScene::on_imgui_update() {
    ui.update(ctx);
    board.debug();

    // ImGui::Begin("Features");
    // ImGui::Checkbox("Skybox", &sky);
    // if (ImGui::Checkbox("Blur", &blur)) {
    //     if (blur) {
    //         create_post_processing(ctx);
    //     } else {
    //         ctx.global<Global>().blur_step = nullptr;
    //     }
    // }
    // if (ImGui::Checkbox("Outline", &outline)) {
    //     if (outline) {
    //         dragon1.get_material()->flags ^= sm::Material::Outline;
    //     } else {
    //         dragon1.get_material()->flags ^= sm::Material::Outline;
    //     }
    // }
    // if (ImGui::Checkbox("Color Correction", &color_correction)) {
    //     ctx.set_color_correction(color_correction);
    //     reload_textures(color_correction);
    // }
    // ImGui::End();

    // ctx.add_task()
}

void GameScene::on_window_resized(const sm::WindowResizedEvent& event) {
    cam.set_projection(event.width, event.height, LENS_FOV, LENS_NEAR, LENS_FAR);
    cam_2d.set_projection(0, event.width, 0, event.height);
}

void GameScene::on_key_released(const sm::KeyReleasedEvent& event) {
    // switch (event.key) {
    //     case sm::Key::Escape:
    //         ctx.change_scene("game"_H);
    //         break;
    //     case sm::Key::M:
    //         if (emitter && sound_move) emitter->play(sound_move);
    //         break;
    //     case sm::Key::P:
    //         if (emitter && sound_place) emitter->play(sound_place);
    //         break;
    //     case sm::Key::Space:
    //         if (music_playing) {
    //             if (relaxing) ctx.stop_music_track();
    //         } else {
    //             if (relaxing) ctx.play_music_track(relaxing);
    //         }

    //         if (relaxing) music_playing = !music_playing;

    //         break;
    //     default:
    //         break;
    // }
}

void GameScene::on_mouse_button_released(const sm::MouseButtonReleasedEvent& event) {
    if (event.button == sm::MouseButton::Left) {
        board.user_click();
    }
}

void GameScene::setup_skybox() {
    field = ctx.load_texture_cubemap(
        "field"_H,
        {
            ctx.get_texture_data("px.png"_H),
            ctx.get_texture_data("nx.png"_H),
            ctx.get_texture_data("py.png"_H),
            ctx.get_texture_data("ny.png"_H),
            ctx.get_texture_data("pz.png"_H),
            ctx.get_texture_data("nz.png"_H)
        },
        sm::TextureFormat::Srgba8Alpha
    );
}

void GameScene::setup_lights() {
    directional_light.direction = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
    directional_light.ambient_color = glm::vec3(0.1f);
    directional_light.diffuse_color = glm::vec3(0.9f);
    directional_light.specular_color = glm::vec3(1.0f);
}

void GameScene::setup_renderables() {
    const auto renderable_board {setup_board()};
    const auto renderable_board_paint {setup_board_paint()};
    const auto renderable_nodes {setup_nodes()};
    const auto renderable_white_pieces {setup_white_pieces()};
    const auto renderable_black_pieces {setup_black_pieces()};

    board = StandardBoard(renderable_board, renderable_board_paint, renderable_nodes, renderable_white_pieces, renderable_black_pieces);
}

sm::Renderable GameScene::setup_board() {
    const auto mesh {ctx.get_mesh("board.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("board"_H, mesh)};

    sm::TextureSpecification specification;
    specification.format = sm::TextureFormat::Srgba8Alpha;

    const auto diffuse {ctx.load_texture(
        "board_diffuse"_H,
        ctx.get_texture_data("board_diffuse.png"_H),
        specification
    )};

    specification.format = sm::TextureFormat::Rgba8;

    const auto normal {ctx.load_texture(
        "board_normal"_H,
        ctx.get_texture_data("board_normal.png"_H),
        specification
    )};

    const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseNormalShadow, sm::Material::CastShadow)};

    const auto material_instance {ctx.load_material_instance("board"_H, material)};
    material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.9f));
    material_instance->set_float("u_material.shininess"_H, 32.0f);
    material_instance->set_texture("u_material.normal"_H, normal, 1);

    return sm::Renderable(mesh, vertex_array, material_instance);
}

sm::Renderable GameScene::setup_board_paint() {
    const auto mesh {ctx.get_mesh("board_paint.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("board_paint"_H, mesh)};

    sm::TextureSpecification specification;
    specification.format = sm::TextureFormat::Srgba8Alpha;

    const auto diffuse {ctx.load_texture(
        "board_paint_labeled_diffuse"_H,
        ctx.get_texture_data("board_paint_labeled_diffuse.png"_H),
        specification
    )};

    specification.format = sm::TextureFormat::Rgba8;

    const auto normal {ctx.load_texture(
        "board_normal"_H,
        ctx.get_texture_data("board_normal.png"_H),
        specification
    )};

    const auto material {ctx.load_material(
        "board_paint"_H,
        ctx.path_engine_assets("shaders/phong_diffuse_normal_shadow.vert"),
        ctx.path_assets("shaders/board/phong_diffuse_normal_shadow.frag"),
        sm::MaterialType::PhongDiffuseNormalShadow
    )};

    const auto material_instance {ctx.load_material_instance("board_paint"_H, material)};
    material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.9f));
    material_instance->set_float("u_material.shininess"_H, 32.0f);
    material_instance->set_texture("u_material.normal"_H, normal, 1);

    return sm::Renderable(mesh, vertex_array, material_instance);
}

std::vector<sm::Renderable> GameScene::setup_nodes() {
    const auto mesh {ctx.get_mesh("node.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("node"_H, mesh)};

    const auto material {ctx.load_material(sm::MaterialType::Phong)};

    std::vector<sm::Renderable> renderables;

    for (unsigned int i {0}; i < 24; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("node" + std::to_string(i)), material)};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(0.075f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.9f));
        material_instance->set_float("u_material.shininess"_H, 32.0f);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}

std::vector<sm::Renderable> GameScene::setup_white_pieces() {
    const auto mesh {ctx.get_mesh("piece_white.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("piece_white"_H, mesh)};

    sm::TextureSpecification specification;
    specification.format = sm::TextureFormat::Srgba8Alpha;

    const auto diffuse {ctx.load_texture(
        "piece_white_diffuse.png"_H,
        ctx.get_texture_data("piece_white_diffuse.png"_H),
        specification
    )};

    specification.format = sm::TextureFormat::Rgba8;

    const auto normal {ctx.load_texture(
        "piece_normal"_H,
        ctx.get_texture_data("piece_normal.png"_H),
        specification
    )};

    const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseNormalShadow, sm::Material::CastShadow)};

    std::vector<sm::Renderable> renderables;

    for (unsigned int i {0}; i < 9; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("piece_white" + std::to_string(i)), material)};
        material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.9f));
        material_instance->set_float("u_material.shininess"_H, 32.0f);
        material_instance->set_texture("u_material.normal"_H, normal, 1);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}

std::vector<sm::Renderable> GameScene::setup_black_pieces() {
    const auto mesh {ctx.get_mesh("piece_black.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("piece_black"_H, mesh)};

    sm::TextureSpecification specification;
    specification.format = sm::TextureFormat::Srgba8Alpha;

    const auto diffuse {ctx.load_texture(
        "piece_black_diffuse.png"_H,
        ctx.get_texture_data("piece_black_diffuse.png"_H),
        specification
    )};

    specification.format = sm::TextureFormat::Rgba8;

    const auto normal {ctx.load_texture(
        "piece_normal"_H,
        ctx.get_texture_data("piece_normal.png"_H),
        specification
    )};

    const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseNormalShadow, sm::Material::CastShadow)};

    std::vector<sm::Renderable> renderables;

    for (unsigned int i {0}; i < 9; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("piece_black" + std::to_string(i)), material)};
        material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.9f));
        material_instance->set_float("u_material.shininess"_H, 32.0f);
        material_instance->set_texture("u_material.normal"_H, normal, 1);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}

// void GameScene::setup_skybox(
//     std::shared_ptr<sm::TextureData> px,
//     std::shared_ptr<sm::TextureData> nx,
//     std::shared_ptr<sm::TextureData> py,
//     std::shared_ptr<sm::TextureData> ny,
//     std::shared_ptr<sm::TextureData> pz,
//     std::shared_ptr<sm::TextureData> nz
// ) {
//     field = ctx.load_texture_cubemap(
//         "field"_H,
//         {px, nx, py, ny, pz, nz},
//         sm::TextureFormat::Srgba8Alpha
//     );
// }

// void GameScene::setup_ground(std::shared_ptr<sm::Mesh> mesh) {
//     const auto vertex_array {ctx.load_vertex_array("ground"_H, mesh)};

//     const auto material {ctx.load_material(sm::MaterialType::PhongShadow, sm::Material::CastShadow)};

//     const auto material_instance {ctx.load_material_instance("ground"_H, material)};
//     material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(0.7f));
//     material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.4f));
//     material_instance->set_float("u_material.shininess"_H, 16.0f);

//     ground = sm::Renderable(mesh, vertex_array, material_instance);
//     ground.transform.position = glm::vec3(0.0f, -1.0f, 0.0f);
//     ground.transform.scale = 2.0f;
// }

// void GameScene::setup_dragon(std::shared_ptr<sm::Mesh> mesh) {
//     const auto vertex_array {ctx.load_vertex_array("dragon"_H, mesh)};

//     const auto material {ctx.load_material(sm::MaterialType::PhongShadow, sm::Material::CastShadow)};

//     {
//         const auto material_instance {ctx.load_material_instance("dragon1"_H, material)};
//         material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(1.0f, 1.0f, 0.0f));
//         material_instance->set_vec3("u_material.specular"_H, glm::vec3(1.0f, 1.0f, 0.0f));
//         material_instance->set_float("u_material.shininess"_H, 32.0f);
//         material_instance->flags |= sm::Material::Outline;

//         dragon1 = sm::Renderable(mesh, vertex_array, material_instance);
//         dragon1.transform.scale = 0.7f;
//         dragon1.outline.color = glm::vec3(0.2f, 0.1f, 1.0f);
//         dragon1.outline.thickness = 0.2f;
//     }

//     {
//         const auto material_instance {ctx.load_material_instance("dragon2"_H, material)};
//         material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(1.0f, 0.2f, 0.1f));
//         material_instance->set_vec3("u_material.specular"_H, glm::vec3(1.0f, 1.0f, 0.0f));
//         material_instance->set_float("u_material.shininess"_H, 32.0f);

//         dragon2 = sm::Renderable(mesh, vertex_array, material_instance);
//         dragon2.transform.position = glm::vec3(4.0f, 0.0, 0.0f);
//         dragon2.transform.scale = 0.2f;
//     }
// }

// void GameScene::setup_teapot(std::shared_ptr<sm::Mesh> mesh) {
//     const auto vertex_array {ctx.load_vertex_array("teapot"_H, mesh)};

//     const auto material {ctx.load_material(sm::MaterialType::PhongShadow, sm::Material::CastShadow)};

//     const auto material_instance {ctx.load_material_instance("teapot"_H, material)};
//     material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(0.7f));
//     material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.7f));
//     material_instance->set_float("u_material.shininess"_H, 64.0f);
//     material_instance->flags |= sm::Material::DisableBackFaceCulling;

//     teapot = sm::Renderable(mesh, vertex_array, material_instance);
//     teapot.transform.position = glm::vec3(2.6f, 0.0, -7.0f);
//     teapot.transform.rotation = glm::vec3(0.0f, 5.3f, 0.0f);
// }

// void GameScene::setup_cube(std::shared_ptr<sm::Mesh> mesh) {
//     const auto vertex_array {ctx.load_vertex_array("cube"_H, mesh)};

//     const auto material {ctx.load_material(sm::MaterialType::PhongShadow, sm::Material::CastShadow)};

//     const auto material_instance {ctx.load_material_instance("cube"_H, material)};
//     material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(1.0f, 0.0f, 0.0f));
//     material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.8f));
//     material_instance->set_float("u_material.shininess"_H, 128.0f);

//     cube = sm::Renderable(mesh, vertex_array, material_instance);
//     cube.transform.position = glm::vec3(5.0f, 2.0f, -2.0f);
//     cube.transform.scale = 0.8f;
// }

// void GameScene::setup_brick(std::shared_ptr<sm::Mesh> mesh, std::shared_ptr<sm::TextureData> texture_data) {
//     const auto vertex_array {ctx.load_vertex_array("brick"_H, mesh)};

//     const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseShadow, sm::Material::CastShadow)};

//     sm::TextureSpecification specification;
//     specification.format = sm::TextureFormat::Srgba8Alpha;

//     const auto diffuse {ctx.load_texture(
//         "brick_diffuse"_H,
//         texture_data,
//         specification
//     )};

//     const auto material_instance {ctx.load_material_instance("brick"_H, material)};
//     material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
//     material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.5f));
//     material_instance->set_float("u_material.shininess"_H, 64.0f);

//     brick = sm::Renderable(mesh, vertex_array, material_instance);
//     brick.transform.position = glm::vec3(6.0f);
//     brick.transform.rotation = glm::vec3(10.0f);
// }

// void GameScene::setup_lamp(std::shared_ptr<sm::Mesh> mesh_stand, std::shared_ptr<sm::TextureData> texture_data_stand, std::shared_ptr<sm::Mesh> mesh_bulb) {
//     {
//         const auto vertex_array {ctx.load_vertex_array("lamp_stand"_H, mesh_stand)};

//         const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseShadow, sm::Material::CastShadow)};

//         sm::TextureSpecification specification;
//         specification.format = sm::TextureFormat::Srgba8Alpha;

//         const auto diffuse {ctx.load_texture(
//             "lamp_stand_diffuse"_H,
//             texture_data_stand,
//             specification
//         )};

//         const auto material_instance {ctx.load_material_instance("lamp_stand"_H, material)};
//         material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
//         material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.5f));
//         material_instance->set_float("u_material.shininess"_H, 64.0f);

//         lamp_stand = sm::Renderable(mesh_stand, vertex_array, material_instance);
//         lamp_stand.transform.position = glm::vec3(-6.0f, 0.0f, -6.0f);
//     }

//     {
//         const auto vertex_array {ctx.load_vertex_array("lamp_bulb"_H, mesh_bulb)};

//         const auto material {ctx.load_material(sm::MaterialType::Flat, sm::Material::CastShadow)};

//         const auto material_instance {ctx.load_material_instance("lamp_bulb"_H, material)};
//         material_instance->set_vec3("u_material.color"_H, glm::vec3(1.0f));

//         lamp_bulb = sm::Renderable(mesh_bulb, vertex_array, material_instance);
//         lamp_bulb.transform.position = glm::vec3(-6.0f, 0.0f, -6.0f);
//     }
// }

// void GameScene::setup_barrel(std::shared_ptr<sm::Mesh> mesh, std::shared_ptr<sm::TextureData> texture_data_diffuse, std::shared_ptr<sm::TextureData> texture_data_normal) {
//     const auto vertex_array {ctx.load_vertex_array("barrel"_H, mesh)};

//     const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseNormalShadow, sm::Material::CastShadow)};

//     sm::TextureSpecification specification;
//     specification.format = sm::TextureFormat::Srgba8Alpha;

//     const auto diffuse {ctx.load_texture(
//         "barrel_diffuse"_H,
//         texture_data_diffuse,
//         specification
//     )};

//     const auto normal {ctx.load_texture(
//         "barrel_normal"_H,
//         texture_data_normal,
//         {}
//     )};

//     const auto material_instance {ctx.load_material_instance("barrel"_H, material)};
//     material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
//     material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.9f));
//     material_instance->set_float("u_material.shininess"_H, 32.0f);
//     material_instance->set_texture("u_material.normal"_H, normal, 1);

//     barrel = sm::Renderable(mesh, vertex_array, material_instance);
//     barrel.transform.position = glm::vec3(-7.0f, 4.5f, 6.0f);
//     barrel.transform.scale = 0.5f;
// }

// void GameScene::setup_textured_bricks(std::shared_ptr<sm::Mesh> mesh, std::shared_ptr<sm::TextureData> texture_data) {
//     const auto vertex_array {ctx.load_vertex_array("brick"_H, mesh)};

//     const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseShadow, sm::Material::CastShadow)};

//     sm::TextureSpecification specification;
//     specification.format = sm::TextureFormat::Srgba8Alpha;

//     const auto diffuse {ctx.load_texture(
//         "brick_diffuse"_H,
//         texture_data,
//         specification
//     )};

//     const auto material_instance {ctx.load_material_instance("brick"_H, material)};
//     material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
//     material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.5f));
//     material_instance->set_float("u_material.shininess"_H, 64.0f);

//     for (std::size_t i {0}; i < 100; i++) {
//         auto& brick {textured_bricks.emplace_back(mesh, vertex_array, material_instance)};
//         brick.transform.position = glm::linearRand(-glm::vec3(150.0f), glm::vec3(150.0f));
//         brick.transform.rotation = glm::linearRand(glm::vec3(0.0f), glm::vec3(360.0f));
//     }
// }

// void GameScene::setup_texts() {
//     sm::FontSpecification specification;
//     specification.bitmap_size = 512;
//     specification.size_height = 40.0f;

//     sans = ctx.load_font(
//         "sans"_H,
//         ctx.path_assets("fonts/OpenSans/OpenSans-Regular.ttf"),
//         specification,
//         [](sm::Font* font) {
//             font->begin_baking();
//             font->bake_ascii();
//             font->bake_characters(256, 127);
//             font->bake_characters(192, 22);
//             font->bake_characters(223, 23);
//             font->bake_characters(536, 4);
//             font->end_baking("sans");
//         }
//     );

//     text1.font = sans;
//     text1.text = "The quick brown fox jumps over the lazy dog.";
//     text1.color = glm::vec3(0.7f);

//     text2 = text1;
//     text2.position = glm::vec2(200.0f);
//     text2.color = glm::vec3(0.8f, 0.7f, 0.1f);

//     text3 = text1;
//     text3.position = glm::vec2(200.0f, 100.0f);
//     text3.color = glm::vec3(0.0f, 1.0f, 1.0f);

//     text4.font = sans;
// }

// void GameScene::setup_quads() {
//     {
//         sm::TextureSpecification specification;
//         specification.format = sm::TextureFormat::Rgba8;

//         const auto texture {ctx.load_texture(
//             "wait"_H,
//             ctx.load_texture_data(ctx.path_assets("textures/indicator/wait_indicator.png"), {}),
//             specification
//         )};

//         wait.texture = texture;
//         wait.position = glm::vec2(70.0f);
//     }

//     {
//         sm::TexturePostProcessing post_processing;
//         post_processing.size = sm::Size::Half;

//         sm::TextureSpecification specification;
//         specification.format = sm::TextureFormat::Rgba8;

//         const auto texture {ctx.load_texture(
//             "white"_H,
//             ctx.load_texture_data(ctx.path_assets("textures/indicator/white_indicator.png"), post_processing),
//             specification
//         )};

//         white.texture = texture;
//         white.position = glm::vec2(210.0f, 210.0f);
//     }
// }

// void GameScene::setup_sounds() {
//     sound_move = ctx.load_buffer("piece_move"_H, ctx.load_sound_data(ctx.path_assets("sounds/piece_move-01.ogg")));
//     sound_place = ctx.load_buffer("piece_place"_H, ctx.load_sound_data(ctx.path_assets("sounds/piece_place-01.ogg")));
//     relaxing = ctx.load_music_track("relaxing"_H, ctx.load_sound_data(ctx.path_assets("sounds/music/relaxing.ogg")));

//     emitter = ctx.load_source("source"_H);
//     emitter->set_position(glm::vec3(0.0f));
// }

// void GameScene::reload_textures(bool srgb) {
//     const sm::TextureFormat format {srgb ? sm::TextureFormat::Srgba8Alpha : sm::TextureFormat::Rgba8};

//     sm::TextureSpecification specification;
//     specification.format = format;

//     {
//         const auto diffuse {ctx.reload_texture(
//             "brick_diffuse"_H,
//             ctx.load_texture_data(ctx.path_assets("textures/brick-texture3.png"), {}),
//             specification
//         )};

//         brick.get_material()->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
//     }

//     {
//         const auto diffuse {ctx.reload_texture(
//             "lamp_stand_diffuse"_H,
//             ctx.load_texture_data(ctx.path_assets("textures/lamp-texture.png"), {}),
//             specification
//         )};

//         lamp_stand.get_material()->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
//     }

//     {
//         const auto diffuse {ctx.reload_texture(
//             "barrel_diffuse"_H,
//             ctx.load_texture_data(ctx.path_assets("textures/barrel.png"), {}),
//             specification
//         )};

//         barrel.get_material()->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
//     }

//     {
//         const auto diffuse {ctx.reload_texture(
//             "brick_diffuse"_H,
//             ctx.load_texture_data(ctx.path_assets("textures/brick-texture3.png"), {}),
//             specification
//         )};

//         for (const auto& brick : textured_bricks) {
//             brick.get_material()->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
//         }
//     }

//     {
//         sm::TexturePostProcessing post_processing;
//         post_processing.size = sm::Size::Half;  // Half looks better
//         post_processing.flip = false;

//         field = ctx.reload_texture_cubemap(
//             "field"_H,
//             {
//                 ctx.load_texture_data(ctx.path_assets("textures/skybox/field/px.png"), post_processing),
//                 ctx.load_texture_data(ctx.path_assets("textures/skybox/field/nx.png"), post_processing),
//                 ctx.load_texture_data(ctx.path_assets("textures/skybox/field/py.png"), post_processing),
//                 ctx.load_texture_data(ctx.path_assets("textures/skybox/field/ny.png"), post_processing),
//                 ctx.load_texture_data(ctx.path_assets("textures/skybox/field/pz.png"), post_processing),
//                 ctx.load_texture_data(ctx.path_assets("textures/skybox/field/nz.png"), post_processing)
//             },
//             format
//         );
//     }
// }

// void GameScene::load_heavy_resources() {
//     {
//         sm::TexturePostProcessing post_processing;
//         post_processing.size = sm::Size::Half;  // Half looks better
//         post_processing.flip = false;

//         const auto px {ctx.load_texture_data(ctx.path_assets("textures/skybox/field/px.png"), post_processing)};
//         const auto nx {ctx.load_texture_data(ctx.path_assets("textures/skybox/field/nx.png"), post_processing)};
//         const auto py {ctx.load_texture_data(ctx.path_assets("textures/skybox/field/py.png"), post_processing)};
//         const auto ny {ctx.load_texture_data(ctx.path_assets("textures/skybox/field/ny.png"), post_processing)};
//         const auto pz {ctx.load_texture_data(ctx.path_assets("textures/skybox/field/pz.png"), post_processing)};
//         const auto nz {ctx.load_texture_data(ctx.path_assets("textures/skybox/field/nz.png"), post_processing)};

//         ctx.add_task([=](const sm::Task&, void*) {
//             setup_skybox(px, nx, py, ny, pz, nz);
//             return sm::Task::Result::Done;
//         });
//     }

//     {
//         const auto mesh {ctx.load_mesh(ctx.path_assets("models/ground.obj"), "Cube", sm::Mesh::Type::PN)};

//         ctx.add_task([=](const sm::Task&, void*) {
//             setup_ground(mesh);
//             return sm::Task::Result::Done;
//         });
//     }

//     {
//         const auto mesh {ctx.load_mesh(ctx.path_assets("models/dragon.obj"), "default", sm::Mesh::Type::PN)};

//         ctx.add_task([=](const sm::Task&, void*) {
//             setup_dragon(mesh);
//             return sm::Task::Result::Done;
//         });
//     }

//     {
//         const auto mesh {ctx.load_mesh(ctx.path_assets("models/teapot.obj"), sm::Mesh::DEFAULT_OBJECT, sm::Mesh::Type::PN)};

//         ctx.add_task([=](const sm::Task&, void*) {
//             setup_teapot(mesh);
//             return sm::Task::Result::Done;
//         });
//     }

//     {
//         const auto mesh {ctx.load_mesh(ctx.path_assets("models/cube.obj"), "Cube", sm::Mesh::Type::PN)};

//         ctx.add_task([=](const sm::Task&, void*) {
//             setup_cube(mesh);
//             return sm::Task::Result::Done;
//         });
//     }

//     {
//         const auto mesh {ctx.load_mesh(ctx.path_assets("models/brick.obj"), "Brick", sm::Mesh::Type::PNT)};

//         const auto texture_data {ctx.load_texture_data(ctx.path_assets("textures/brick-texture3.png"), sm::TexturePostProcessing())};

//         ctx.add_task([=](const sm::Task&, void*) {
//             setup_brick(mesh, texture_data);
//             return sm::Task::Result::Done;
//         });
//     }

//     {
//         const auto mesh_stand {ctx.load_mesh("lamp_stand"_H, ctx.path_assets("models/lamp.obj"), "Stand", sm::Mesh::Type::PNT)};

//         const auto texture_data_stand {ctx.load_texture_data(ctx.path_assets("textures/lamp-texture.png"), sm::TexturePostProcessing())};

//         const auto mesh_bulb {ctx.load_mesh("lamp_bulb"_H, ctx.path_assets("models/lamp.obj"), "Bulb", sm::Mesh::Type::P)};

//         ctx.add_task([=](const sm::Task&, void*) {
//             setup_lamp(mesh_stand, texture_data_stand, mesh_bulb);
//             return sm::Task::Result::Done;
//         });
//     }

//     {
//         const auto mesh {ctx.load_mesh(ctx.path_assets("models/barrel.obj"), "Mesh_Mesh_Cylinder.001", sm::Mesh::Type::PNTT)};

//         const auto texture_data_diffuse {ctx.load_texture_data(ctx.path_assets("textures/barrel.png"), sm::TexturePostProcessing())};

//         const auto texture_data_normal {ctx.load_texture_data(ctx.path_assets("textures/barrelNormal.png"), sm::TexturePostProcessing())};

//         ctx.add_task([=](const sm::Task&, void*) {
//             setup_barrel(mesh, texture_data_diffuse, texture_data_normal);
//             return sm::Task::Result::Done;
//         });
//     }

//     {
//         const auto mesh {ctx.load_mesh("brick"_H, ctx.path_assets("models/brick.obj"), "Brick", sm::Mesh::Type::PNT)};

//         const auto load_texture {ctx.load_texture_data(ctx.path_assets("textures/brick-texture3.png"), sm::TexturePostProcessing())};

//         ctx.add_task([=](const sm::Task&, void*) {
//             setup_textured_bricks(mesh, load_texture);
//             return sm::Task::Result::Done;
//         });
//     }

//     {
//         ctx.load_sound_data(ctx.path_assets("sounds/piece_move-01.ogg"));
//         ctx.load_sound_data(ctx.path_assets("sounds/piece_place-01.ogg"));
//         ctx.load_sound_data(ctx.path_assets("sounds/music/relaxing.ogg"));

//         ctx.add_task([=](const sm::Task&, void*) {
//             setup_sounds();
//             return sm::Task::Result::Done;
//         });
//     }
// }
