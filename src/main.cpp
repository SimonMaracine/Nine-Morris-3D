#include "application/application.h"
#include "nine_morris_3d/scenes/game/game_scene.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/scenes/loading/loading_scene.h"
#include "nine_morris_3d/layers/loading/loading_layer.h"

int main() {
    constexpr int width = 1024;
    constexpr int height = 576;

    Application* application = new Application(width, height, "Nine Morris 3D");
    Application::set_pointer(application);

    GameScene* game_scene = new GameScene(0);
    LoadingScene* loading_scene = new LoadingScene(1);

    application->add_scene(game_scene);
    application->push_layer(new GameLayer(0, game_scene), game_scene);
    application->push_layer(new GuiLayer(1, game_scene), game_scene);
    application->push_layer(new ImGuiLayer(2, game_scene), game_scene);

    application->add_scene(loading_scene);
    application->push_layer(new LoadingLayer(0, loading_scene), loading_scene);

    application->set_starting_scene(loading_scene);
    application->run();

    delete application;
}
