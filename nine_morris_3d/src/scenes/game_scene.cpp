#include "scenes/game_scene.hpp"

#include <nine_morris_3d_engine/external/resmanager.h++>

#include "global.hpp"

void GameScene::on_start() {
    ctx.connect_event<sm::WindowResizedEvent, &GameScene::on_window_resized>(this);

    ctx.set_renderer_clear_color(glm::vec3(0.1f, 0.1f, 0.1f));

    setup_camera();
    setup_skybox();
    setup_lights();

    m_ui.initialize(ctx);
}

void GameScene::on_stop() {
    m_cam_controller.disconnect_events(ctx);
    ctx.disconnect_events(this);
}

void GameScene::on_update() {
    m_cam_controller.update_controls(ctx.get_delta(), ctx);
    m_cam_controller.update_camera(ctx.get_delta());

    ctx.capture(m_cam, m_cam_controller.get_position());
    ctx.capture(m_cam_2d);

    ctx.add_light(m_directional_light);

    if (m_skybox) {
        ctx.skybox(m_skybox);
    }

    if (m_ui.get_show_information()) {
        ctx.show_information_text();
    }

    // Origin
    ctx.debug_add_line(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ctx.debug_add_line(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ctx.debug_add_line(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Light
    ctx.debug_add_lamp(glm::normalize(-m_directional_light.direction) * 15.0f, glm::vec3(0.6f));
}

void GameScene::on_fixed_update() {
    m_cam_controller.update_friction();
}

void GameScene::on_imgui_update() {
    m_ui.update(ctx, *this);
}

void GameScene::load_and_set_skybox() {
    ctx.add_task_async([this](sm::AsyncTask& task, void*) {
        load_skybox();

        ctx.add_task([this](const sm::Task&, void*) {
            setup_skybox();
            setup_lights();

            return sm::Task::Result::Done;
        });

        task.set_done();
    });
}

void GameScene::load_and_set_board_paint_texture() {
    ctx.add_task_async([this](sm::AsyncTask& task, void*) {
        load_board_paint_texture();

        ctx.add_task([this](const sm::Task&, void*) {
            get_board().set_board_paint_renderable(setup_board_paint());

            return sm::Task::Result::Done;
        });

        task.set_done();
    });
}

void GameScene::on_window_resized(const sm::WindowResizedEvent& event) {
    m_cam.set_projection(event.width, event.height, LENS_FOV, LENS_NEAR, LENS_FAR);
    m_cam_2d.set_projection(0, event.width, 0, event.height);
}

void GameScene::setup_camera() {
    const auto& g {ctx.global<Global>()};

    m_cam_controller = PointCameraController(
        m_cam,
        ctx.get_window_width(),
        ctx.get_window_height(),
        glm::vec3(0.0f),
        8.0f,
        47.0f,
        g.options.camera_sensitivity
    );

    m_cam_controller.connect_events(ctx);

    m_default_camera_position = m_cam_controller.get_position();
    m_cam_controller.set_distance_to_point(m_cam_controller.get_distance_to_point() + 1.0f);
    m_cam_controller.go_towards_position(m_default_camera_position);

    m_cam_2d.set_projection(0, ctx.get_window_width(), 0, ctx.get_window_height());
}

void GameScene::setup_skybox() {
    m_skybox = get_skybox_texture_cubemap();
}

void GameScene::setup_lights() {
    const auto& g {ctx.global<Global>()};

    switch (g.options.skybox) {
        case static_cast<int>(Skybox::None):
            m_directional_light.direction = glm::normalize(glm::vec3(0.1f, -0.8f, 0.1f));
            m_directional_light.ambient_color = glm::vec3(0.07f);
            m_directional_light.diffuse_color = glm::vec3(0.6f);
            m_directional_light.specular_color = glm::vec3(0.75f);
            break;
        case static_cast<int>(Skybox::Field):
            m_directional_light.direction = glm::normalize(glm::vec3(-0.198f, -0.192f, -0.282f));
            m_directional_light.ambient_color = glm::vec3(0.08f);
            m_directional_light.diffuse_color = glm::vec3(0.95f);
            m_directional_light.specular_color = glm::vec3(1.0f);
            break;
        case static_cast<int>(Skybox::Autumn):
            m_directional_light.direction = glm::normalize(glm::vec3(0.4f, -1.0f, -0.1f));
            m_directional_light.ambient_color = glm::vec3(0.15f);
            m_directional_light.diffuse_color = glm::vec3(0.75f);
            m_directional_light.specular_color = glm::vec3(0.65f);
            break;
    }
}

sm::Renderable GameScene::setup_board() const {
    const auto mesh {ctx.get_mesh("board.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("board"_H, mesh)};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;

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
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 8.0f);
    material_instance->set_texture("u_material.normal"_H, normal, 1);

    return sm::Renderable(mesh, vertex_array, material_instance);
}

sm::Renderable GameScene::setup_board_paint() const {
    const auto mesh {ctx.get_mesh("board_paint.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("board_paint"_H, mesh)};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;

    const auto diffuse {get_board_paint_texture(specification)};

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
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 8.0f);
    material_instance->set_texture("u_material.normal"_H, normal, 1);

    return sm::Renderable(mesh, vertex_array, material_instance);
}

std::vector<sm::Renderable> GameScene::setup_nodes(unsigned int count) const {
    const auto mesh {ctx.get_mesh("node.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("node"_H, mesh)};

    const auto material {ctx.load_material(sm::MaterialType::Phong)};

    std::vector<sm::Renderable> renderables;

    for (unsigned int i {0}; i < count; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("node" + std::to_string(i)), material)};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(0.075f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.3f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}

std::vector<sm::Renderable> GameScene::setup_white_pieces(unsigned int count) const {
    const auto mesh {ctx.get_mesh("piece_white.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("piece_white"_H, mesh)};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;

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

    for (unsigned int i {0}; i < count; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("piece_white" + std::to_string(i)), material)};
        material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);
        material_instance->set_texture("u_material.normal"_H, normal, 1);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}

std::vector<sm::Renderable> GameScene::setup_black_pieces(unsigned int count) const {
    const auto mesh {ctx.get_mesh("piece_black.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("piece_black"_H, mesh)};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;

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

    for (unsigned int i {0}; i < count; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("piece_black" + std::to_string(i)), material)};
        material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);
        material_instance->set_texture("u_material.normal"_H, normal, 1);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}

void GameScene::load_skybox() const {
    // Global options must have been set to the desired skybox

    const auto& g {ctx.global<Global>()};

    sm::TexturePostProcessing post_processing;
    post_processing.flip = false;

    switch (g.options.skybox) {
        case static_cast<int>(Skybox::None):
            break;
        case static_cast<int>(Skybox::Field):
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/px.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/nx.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/py.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/ny.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/pz.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/nz.png"), post_processing);
            break;
        case static_cast<int>(Skybox::Autumn):
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/px.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/nx.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/py.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/ny.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/pz.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/nz.png"), post_processing);
            break;
    }
}

void GameScene::load_board_paint_texture() const {
    // Global options must have been set to the desired texture

    const auto& g {ctx.global<Global>()};

    if (g.options.labeled_board) {
        ctx.load_texture_data(ctx.path_assets("textures/board/board_paint_labeled_diffuse.png"), sm::TexturePostProcessing());
    } else {
        ctx.load_texture_data(ctx.path_assets("textures/board/board_paint_diffuse.png"), sm::TexturePostProcessing());
    }
}

std::shared_ptr<sm::GlTextureCubemap> GameScene::get_skybox_texture_cubemap() const {
    const auto& g {ctx.global<Global>()};

    switch (g.options.skybox) {
        case static_cast<int>(Skybox::None):
            return nullptr;
        case static_cast<int>(Skybox::Field):
            return ctx.load_texture_cubemap(
                "field"_H,
                {
                    ctx.get_texture_data("px.png"_H),
                    ctx.get_texture_data("nx.png"_H),
                    ctx.get_texture_data("py.png"_H),
                    ctx.get_texture_data("ny.png"_H),
                    ctx.get_texture_data("pz.png"_H),
                    ctx.get_texture_data("nz.png"_H)
                },
                sm::TextureFormat::Srgb8Alpha8
            );
        case static_cast<int>(Skybox::Autumn):
            return ctx.load_texture_cubemap(
                "autumn"_H,
                {
                    ctx.get_texture_data("px.png"_H),
                    ctx.get_texture_data("nx.png"_H),
                    ctx.get_texture_data("py.png"_H),
                    ctx.get_texture_data("ny.png"_H),
                    ctx.get_texture_data("pz.png"_H),
                    ctx.get_texture_data("nz.png"_H)
                },
                sm::TextureFormat::Srgb8Alpha8
            );
    }

    return {};
}

std::shared_ptr<sm::GlTexture> GameScene::get_board_paint_texture(const sm::TextureSpecification& specification) const {
    const auto& g {ctx.global<Global>()};

    if (g.options.labeled_board) {
        return ctx.load_texture(
            "board_paint_labeled_diffuse"_H,
            ctx.get_texture_data("board_paint_labeled_diffuse.png"_H),
            specification
        );
    } else {
        return ctx.load_texture(
            "board_paint_diffuse"_H,
            ctx.get_texture_data("board_paint_diffuse.png"_H),
            specification
        );
    }
}

