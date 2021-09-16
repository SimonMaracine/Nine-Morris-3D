#include "application/application.h"
#include "nine_morris_3d/scenes/game/game_scene.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/scenes/loading/loading_scene.h"
#include "nine_morris_3d/layers/loading/loading_layer.h"
#include "other/asset_manager.h"

int main() {
    constexpr int width = 1024;
    constexpr int height = 576;

    Application* application = new Application(width, height, "Nine Morris 3D");

    application->add_asset(0, AssetType::Mesh, "data/models/board/board.obj");
    application->add_asset(1, AssetType::Mesh, "data/models/board/board_paint.obj");

    application->add_asset(2, AssetType::MeshP, "data/models/node/node.obj");

    application->add_asset(3, AssetType::Mesh, "data/models/piece/white_piece.obj");
    application->add_asset(4, AssetType::Mesh, "data/models/piece/black_piece.obj");

    application->add_asset(5, AssetType::TextureFlipped, "data/textures/board/board_wood.png");
    application->add_asset(6, AssetType::TextureFlipped, "data/textures/board/board_wood-small.png");
    application->add_asset(7, AssetType::TextureFlipped, "data/textures/board/board_paint.png");
    application->add_asset(8, AssetType::TextureFlipped, "data/textures/board/board_paint-small.png");

    application->add_asset(9, AssetType::TextureFlipped, "data/textures/indicator/white_indicator.png");
    application->add_asset(10, AssetType::TextureFlipped, "data/textures/indicator/black_indicator.png");

    application->add_asset(11, AssetType::TextureFlipped, "data/textures/loading/loading.png");

    application->add_asset(12, AssetType::TextureFlipped, "data/textures/piece/white_piece.png");
    application->add_asset(13, AssetType::TextureFlipped, "data/textures/piece/white_piece-small.png");
    application->add_asset(14, AssetType::TextureFlipped, "data/textures/piece/black_piece.png");
    application->add_asset(15, AssetType::TextureFlipped, "data/textures/piece/black_piece-small.png");

    application->add_asset(16, AssetType::Texture, "data/textures/skybox/right.jpg");
    application->add_asset(17, AssetType::Texture, "data/textures/skybox/left.jpg");
    application->add_asset(18, AssetType::Texture, "data/textures/skybox/top.jpg");
    application->add_asset(19, AssetType::Texture, "data/textures/skybox/bottom.jpg");
    application->add_asset(20, AssetType::Texture, "data/textures/skybox/front.jpg");
    application->add_asset(21, AssetType::Texture, "data/textures/skybox/back.jpg");

    GameScene* game_scene = new GameScene(0);
    LoadingScene* loading_scene = new LoadingScene(1);

    application->add_scene(game_scene);
    application->push_layer(new GameLayer(0, application, game_scene), game_scene);
    application->push_layer(new GuiLayer(1, application, game_scene), game_scene);
    application->push_layer(new ImGuiLayer(2, application, game_scene), game_scene);

    application->add_scene(loading_scene);
    application->push_layer(new LoadingLayer(0, application, loading_scene), loading_scene);

    application->set_starting_scene(loading_scene);
    application->run();

    delete application;
}
