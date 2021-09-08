#include "application/application.h"
#include "application/layers/game_layer.h"
#include "application/layers/loading_layer.h"
#include "application/layers/imgui_layer.h"
#include "application/layers/gui_layer.h"

int main() {
    constexpr int width = 1024;
    constexpr int height = 576;

    Application* application = new Application(width, height, "Nine Morris 3D");

    application->add_asset(0, "data/models/board/board.obj");
    application->add_asset(1, "data/models/board/board_paint.obj");

    application->add_asset(2, "data/models/node/node.obj");

    application->add_asset(3, "data/models/piece/white_piece.obj");
    application->add_asset(4, "data/models/piece/black_piece.obj");

    application->add_asset(5, "data/textures/board/board_wood.png");
    application->add_asset(6, "data/textures/board/board_wood-small.png");
    application->add_asset(7, "data/textures/board/board_paint.png");
    application->add_asset(8, "data/textures/board/board_paint-small.png");

    application->add_asset(9, "data/textures/indicator/white_indicator.png");
    application->add_asset(10, "data/textures/indicator/black_indicator.png");

    application->add_asset(11, "data/textures/loading/loading.png");

    application->add_asset(12, "data/textures/piece/white_piece.png");
    application->add_asset(13, "data/textures/piece/white_piece-small.png");
    application->add_asset(14, "data/textures/piece/black_piece.png");
    application->add_asset(15, "data/textures/piece/black_piece-small.png");

    application->add_asset(16, "data/textures/skybox/back.png");
    application->add_asset(17, "data/textures/skybox/bottom.png");
    application->add_asset(18, "data/textures/skybox/front.png");
    application->add_asset(19, "data/textures/skybox/left.png");
    application->add_asset(20, "data/textures/skybox/right.png");
    application->add_asset(21, "data/textures/skybox/top.png");

    application->push_layer(new GameLayer(0, application));
    application->push_layer(new GuiLayer(1, application));
    application->push_layer(new ImGuiLayer(2, application));
    application->push_layer(new LoadingLayer(3, application));

    application->run();

    delete application;
}
