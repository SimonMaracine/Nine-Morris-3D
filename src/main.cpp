#include "application/application.h"
#include "application/layers/game_layer.h"
#include "application/layers/loading_layer.h"
#include "application/layers/imgui_layer.h"
#include "application/layers/gui_layer.h"
#include "other/asset_manager.h"

int main() {
    constexpr int width = 1024;
    constexpr int height = 576;

    Application* application = new Application(width, height, "Nine Morris 3D");

    application->add_asset(0, AssetType::FullModel, "data/models/board/board.obj");
    application->add_asset(1, AssetType::FullModel, "data/models/board/board_paint.obj");

    application->add_asset(2, AssetType::PositionModel, "data/models/node/node.obj");

    application->add_asset(3, AssetType::FullModel, "data/models/piece/white_piece.obj");
    application->add_asset(4, AssetType::FullModel, "data/models/piece/black_piece.obj");

    application->add_asset(5, AssetType::TextureData, "data/textures/board/board_wood.png");
    application->add_asset(6, AssetType::TextureData, "data/textures/board/board_wood-small.png");
    application->add_asset(7, AssetType::TextureData, "data/textures/board/board_paint.png");
    application->add_asset(8, AssetType::TextureData, "data/textures/board/board_paint-small.png");

    application->add_asset(9, AssetType::TextureData, "data/textures/indicator/white_indicator.png");
    application->add_asset(10, AssetType::TextureData, "data/textures/indicator/black_indicator.png");

    application->add_asset(11, AssetType::TextureData, "data/textures/loading/loading.png");

    application->add_asset(12, AssetType::TextureData, "data/textures/piece/white_piece.png");
    application->add_asset(13, AssetType::TextureData, "data/textures/piece/white_piece-small.png");
    application->add_asset(14, AssetType::TextureData, "data/textures/piece/black_piece.png");
    application->add_asset(15, AssetType::TextureData, "data/textures/piece/black_piece-small.png");

    application->add_asset(16, AssetType::TextureData, "data/textures/skybox/back.png");
    application->add_asset(17, AssetType::TextureData, "data/textures/skybox/bottom.png");
    application->add_asset(18, AssetType::TextureData, "data/textures/skybox/front.png");
    application->add_asset(19, AssetType::TextureData, "data/textures/skybox/left.png");
    application->add_asset(20, AssetType::TextureData, "data/textures/skybox/right.png");
    application->add_asset(21, AssetType::TextureData, "data/textures/skybox/top.png");

    application->push_layer(new GameLayer(0, application));
    application->push_layer(new GuiLayer(1, application));
    application->push_layer(new ImGuiLayer(2, application));
    application->push_layer(new LoadingLayer(3, application));

    application->run();

    delete application;
}
