#include "application/application.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/layers/loading/loading_layer.h"

int main() {
    constexpr int width = 1024;
    constexpr int height = 576;

    Application* application = new Application(width, height, "Nine Morris 3D");
    Application::set_pointer(application);

    Scene* game_scene = new Scene("game");
    game_scene->add_layer(new GameLayer("game"));
    game_scene->add_layer(new GuiLayer("gui"));
    game_scene->add_layer(new ImGuiLayer("imgui"));
    application->add_scene(game_scene);

    Scene* loading_scene = new Scene("loading");
    loading_scene->add_layer(new LoadingLayer("loading"));
    application->add_scene(loading_scene);

    application->set_starting_scene(loading_scene);
    application->run();

    delete application;
}
