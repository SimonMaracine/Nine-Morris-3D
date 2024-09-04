#include "scenes/loading_scene.hpp"

#include "nine_morris_3d_engine/external/resmanager.h++"

#include "global.hpp"

void LoadingScene::on_start() {
    ctx.add_task_async([this](sm::AsyncTask& task, void*) {
        load_assets();
        task.set_done();
    });
}

void LoadingScene::on_stop() {

}

void LoadingScene::on_update() {
    if (m_done) {
        ctx.change_scene("standard_game"_H);
    }
}

void LoadingScene::load_assets() {
    const auto& g {ctx.global<Global>()};

    {
        sm::TexturePostProcessing post_processing;
        post_processing.flip = false;

        switch (g.options.skybox) {
            case static_cast<int>(Skybox::None):
                break;
            case static_cast<int>(Skybox::Field):
                ctx.load_texture_data(ctx.path_assets("textures/skybox/field/px.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/field/nx.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/field/py.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/field/ny.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/field/pz.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/field/nz.png"), post_processing);
                break;
            case static_cast<int>(Skybox::Autumn):
                ctx.load_texture_data(ctx.path_assets("textures/skybox/autumn/px.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/autumn/nx.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/autumn/py.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/autumn/ny.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/autumn/pz.png"), post_processing);
                ctx.load_texture_data(ctx.path_assets("textures/skybox/autumn/nz.png"), post_processing);
                break;
        }
    }

    {
        ctx.load_mesh(ctx.path_assets("models/board/board.obj"), "Board_Cube", sm::Mesh::Type::PNTT);
        ctx.load_texture_data(ctx.path_assets("textures/board/board_diffuse.png"), sm::TexturePostProcessing());
        ctx.load_texture_data(ctx.path_assets("textures/board/board_normal.png"), sm::TexturePostProcessing());
    }

    {
        ctx.load_mesh(ctx.path_assets("models/board/board_paint.obj"), "Plane", sm::Mesh::Type::PNTT);

        if (g.options.labeled_board) {
            ctx.load_texture_data(ctx.path_assets("textures/board/board_paint_labeled_diffuse.png"), sm::TexturePostProcessing());
        } else {
            ctx.load_texture_data(ctx.path_assets("textures/board/board_paint_diffuse.png"), sm::TexturePostProcessing());
        }
    }

    {
        ctx.load_mesh(ctx.path_assets("models/node/node.obj"), "Node", sm::Mesh::Type::PN, true);
    }

    {
        ctx.load_mesh(ctx.path_assets("models/piece/piece_white.obj"), "White_Piece_Cylinder", sm::Mesh::Type::PNTT);
        ctx.load_mesh(ctx.path_assets("models/piece/piece_black.obj"), "Black_Piece_Cylinder", sm::Mesh::Type::PNTT);
        ctx.load_texture_data(ctx.path_assets("textures/piece/piece_white_diffuse.png"), sm::TexturePostProcessing());
        ctx.load_texture_data(ctx.path_assets("textures/piece/piece_black_diffuse.png"), sm::TexturePostProcessing());
        ctx.load_texture_data(ctx.path_assets("textures/piece/piece_normal.png"), sm::TexturePostProcessing());
    }

    {
        ctx.load_texture_data(ctx.path_assets("textures/indicator/white_indicator.png"), sm::TexturePostProcessing());
        ctx.load_texture_data(ctx.path_assets("textures/indicator/black_indicator.png"), sm::TexturePostProcessing());
    }

    ctx.add_task([this](const sm::Task&, void*) {
        m_done = true;
        return sm::Task::Result::Done;
    });
}
