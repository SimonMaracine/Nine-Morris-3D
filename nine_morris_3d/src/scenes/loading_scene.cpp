#include "scenes/loading_scene.hpp"

#include "nine_morris_3d_engine/external/resmanager.h++"

#include "global.hpp"

void LoadingScene::on_start() {
    ctx.connect_event<sm::WindowResizedEvent, &LoadingScene::on_window_resized>(this);

    m_camera_2d.set_projection(0, ctx.get_window_width(), 0, ctx.get_window_height());

    ctx.add_task_async([this](sm::AsyncTask& task) {
        try {
            load_assets(task);
        } catch (const sm::RuntimeError&) {
            task.set_done(std::current_exception());
            return;
        }

        task.set_done();
    });

    load_splash_screen();
}

void LoadingScene::on_stop() {
    ctx.disconnect_events(this);
}

void LoadingScene::on_update() {
    ctx.capture(m_camera_2d);

    update_splash_screen();

    if (m_done) {
        const auto& g {ctx.global<Global>()};

        switch (g.options.game_mode) {
            case GameModeNineMensMorris:
                ctx.change_scene("nine_mens_morris"_H);
                break;
            case GameModeTwelveMensMorris:
                ctx.change_scene("twelve_mens_morris"_H);
                break;
        }
    }
}

void LoadingScene::on_window_resized(const sm::WindowResizedEvent& event) {
    m_camera_2d.set_projection(0, event.width, 0, event.height);
}

void LoadingScene::update_splash_screen() {
    float x, y, width, height;
    sm::utils::center_image(
        static_cast<float>(ctx.get_window_width()),
        static_cast<float>(ctx.get_window_height()),
        static_cast<float>(m_splash_screen->get_width()),
        static_cast<float>(m_splash_screen->get_height()),
        x, y, width, height
    );

    sm::Quad splash_screen;
    splash_screen.texture = m_splash_screen;
    splash_screen.position = glm::vec2(x, y);
    splash_screen.scale = glm::vec2(width / static_cast<float>(m_splash_screen->get_width()), height / static_cast<float>(m_splash_screen->get_height()));

    ctx.add_quad(splash_screen);
}

void LoadingScene::load_splash_screen() {
    sm::TexturePostProcessing post_processing;
    const auto texture_data {ctx.load_texture_data(ctx.path_assets("textures/splash_screen/splash_screen.png"), post_processing)};

    sm::TextureSpecification specification;
    specification.format = sm::TextureFormat::Rgba8;
    m_splash_screen = ctx.load_texture("splash_screen"_H, texture_data, specification);
}

void LoadingScene::load_assets(sm::AsyncTask& task) {
    const auto& g {ctx.global<Global>()};

    {
        sm::TexturePostProcessing post_processing;
        post_processing.flip = false;

        if (g.options.texture_quality == TextureQualityHalf) {
            post_processing.size = sm::TextureSize::Half;
        }

        switch (g.options.skybox) {
            case SkyboxNone:
                break;
            case SkyboxField:
                ctx.load_texture_data(ctx.path_assets("textures/skybox/field/px.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/field/nx.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/field/py.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/field/ny.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/field/pz.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/field/nz.png"), post_processing);
                break;
            case SkyboxAutumn:
                ctx.load_texture_data(ctx.path_assets("textures/skybox/autumn/px.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/autumn/nx.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/autumn/py.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/autumn/ny.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/autumn/pz.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/autumn/nz.png"), post_processing);
                break;
        }
    }

    if (task.stop_requested()) {
        task.set_done();
        return;
    }

    {
        sm::TexturePostProcessing post_processing;

        if (g.options.texture_quality == TextureQualityHalf) {
            post_processing.size = sm::TextureSize::Half;
        }

        ctx.load_mesh(ctx.path_assets("models/board/board.obj"), "Board_Cube", sm::Mesh::Type::PNTT);
        ctx.load_texture_data(ctx.path_assets("textures/board/board_diffuse.png"), post_processing);
        ctx.load_texture_data(ctx.path_assets("textures/board/board_normal.png"), post_processing);
    }

    if (task.stop_requested()) {
        task.set_done();
        return;
    }

    {
        sm::TexturePostProcessing post_processing;

        if (g.options.texture_quality == TextureQualityHalf) {
            post_processing.size = sm::TextureSize::Half;
        }

        ctx.load_mesh(ctx.path_assets("models/board/paint.obj"), "Plane", sm::Mesh::Type::PNTT);

        ctx.load_texture_data(ctx.path_assets("textures/board/paint_diffuse.png"), post_processing);
    }

    {
        ctx.load_mesh(ctx.path_assets("models/node/node.obj"), "Node", sm::Mesh::Type::PN, true);
    }

    if (task.stop_requested()) {
        task.set_done();
        return;
    }

    {
        sm::TexturePostProcessing post_processing;

        if (g.options.texture_quality == TextureQualityHalf) {
            post_processing.size = sm::TextureSize::Half;
        }

        ctx.load_mesh(ctx.path_assets("models/piece/piece_white.obj"), "White_Piece_Cylinder", sm::Mesh::Type::PNTT);
        ctx.load_mesh(ctx.path_assets("models/piece/piece_black.obj"), "Black_Piece_Cylinder", sm::Mesh::Type::PNTT);
        ctx.load_texture_data(ctx.path_assets("textures/piece/piece_white_diffuse.png"), post_processing);
        ctx.load_texture_data(ctx.path_assets("textures/piece/piece_black_diffuse.png"), post_processing);
        ctx.load_texture_data(ctx.path_assets("textures/piece/piece_normal.png"), post_processing);
    }

    ctx.add_task_immediate([this]() {
        m_done = true;
        return sm::Task::Result::Done;
    });
}
