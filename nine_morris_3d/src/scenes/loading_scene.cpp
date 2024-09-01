#include "scenes/loading_scene.hpp"

#include "nine_morris_3d_engine/external/resmanager.h++"

void LoadingScene::on_start() {
    ctx.add_task_async([this](sm::AsyncTask& task, void*) {
        load_assets();
        task.set_done();
    });
}

void LoadingScene::on_stop() {

}

void LoadingScene::on_update() {
    if (done) {
        ctx.change_scene("game"_H);
    }
}

void LoadingScene::load_assets() {
    {
        sm::TexturePostProcessing post_processing;
        post_processing.flip = false;

        ctx.load_texture_data(ctx.path_assets("textures/skybox/field/px.png"), post_processing);
        ctx.load_texture_data(ctx.path_assets("textures/skybox/field/nx.png"), post_processing);
        ctx.load_texture_data(ctx.path_assets("textures/skybox/field/py.png"), post_processing);
        ctx.load_texture_data(ctx.path_assets("textures/skybox/field/ny.png"), post_processing);
        ctx.load_texture_data(ctx.path_assets("textures/skybox/field/pz.png"), post_processing);
        ctx.load_texture_data(ctx.path_assets("textures/skybox/field/nz.png"), post_processing);
    }

    {
        ctx.load_mesh(ctx.path_assets("models/board/board.obj"), "Board_Cube", sm::Mesh::Type::PNTT);
        ctx.load_texture_data(ctx.path_assets("textures/board/board_diffuse.png"), sm::TexturePostProcessing());
        ctx.load_texture_data(ctx.path_assets("textures/board/board_normal.png"), sm::TexturePostProcessing());
    }

    {
        ctx.load_mesh(ctx.path_assets("models/board/board_paint.obj"), "Plane", sm::Mesh::Type::PNTT);
        ctx.load_texture_data(ctx.path_assets("textures/board/board_paint_labeled_diffuse.png"), sm::TexturePostProcessing());
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
        done = true;
        return sm::Task::Result::Done;
    });
}
