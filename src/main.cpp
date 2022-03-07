#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/layers/loading/loading_layer.h"

int main() {
    NineMorris3D* application = new NineMorris3D();
    NineMorris3D::set_app_pointer(application);

    Scene* game_scene = new Scene("game");
    game_scene->add_layer(new GameLayer("game", application));
    game_scene->add_overlay(new GuiLayer("gui", application));
    game_scene->add_overlay(new ImGuiLayer("imgui", application));
    application->add_scene(game_scene);

    Scene* loading_scene = new Scene("loading");
    loading_scene->add_overlay(new LoadingLayer("loading", application));
    application->add_scene(loading_scene);

    application->set_starting_scene(loading_scene);
    application->run();

    delete application;
}
