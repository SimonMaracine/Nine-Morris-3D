#include "scenes/loading_scene.hpp"

#include "nine_morris_3d_engine/external/resmanager.h++"

#include "global.hpp"

void LoadingScene::on_start() {
    ctx.add_task_async([this](sm::AsyncTask& task) {
        try {
            load_assets(task);
        } catch (const sm::RuntimeError&) {
            task.set_done(std::current_exception());
            return;
        }

        task.set_done();
    });

    m_loading_image = std::make_shared<sm::ImageNode>(load_splash_screen());
}

void LoadingScene::on_stop() {

}

void LoadingScene::on_update() {
    update_loading_image();

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

void LoadingScene::update_loading_image() {
    float x, y, width, height;
    sm::utils::center_image(
        static_cast<float>(ctx.get_window_width()),
        static_cast<float>(ctx.get_window_height()),
        static_cast<float>(m_loading_image->get_texture()->get_width()),
        static_cast<float>(m_loading_image->get_texture()->get_height()),
        x, y, width, height
    );

    m_loading_image->transform.position = glm::vec2(x, y);
    m_loading_image->transform.scale = glm::vec2(
        width / static_cast<float>(m_loading_image->get_texture()->get_width()),
        height / static_cast<float>(m_loading_image->get_texture()->get_height())
    );

    ctx.root_2d()->add_node(m_loading_image);
}

std::shared_ptr<sm::GlTexture> LoadingScene::load_splash_screen() {
    sm::TextureSpecification specification;
    specification.format = sm::TextureFormat::Rgba8;

    return ctx.load_texture(
        "splash_screen"_H,
        ctx.load_texture_data(ctx.path_assets("textures/splash_screen/splash_screen.png"), sm::TexturePostProcessing()),
        specification
    );
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

        ctx.load_mesh(ctx.path_assets("models/board/board.obj"), "Board", sm::Mesh::Type::PNTT);
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

        ctx.load_mesh(ctx.path_assets("models/board/paint.obj"), "Paint", sm::Mesh::Type::PNTT);

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

        ctx.load_mesh(ctx.path_assets("models/piece/piece_white.obj"), "White_Piece", sm::Mesh::Type::PNTT);
        ctx.load_mesh(ctx.path_assets("models/piece/piece_black.obj"), "Black_Piece", sm::Mesh::Type::PNTT);
        ctx.load_texture_data(ctx.path_assets("textures/piece/piece_white_diffuse.png"), post_processing);
        ctx.load_texture_data(ctx.path_assets("textures/piece/piece_black_diffuse.png"), post_processing);
        ctx.load_texture_data(ctx.path_assets("textures/piece/piece_normal.png"), post_processing);
    }

    ctx.add_task_immediate([this]() {
        m_done = true;
        return sm::Task::Result::Done;
    });
}
