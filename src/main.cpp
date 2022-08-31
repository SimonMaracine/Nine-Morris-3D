#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

// #include "application/scene.h"
#include "nine_morris_3d.h"
#include "scenes/game_scene.h"
// #include "layers/game/game_layer.h"
// #include "layers/game/imgui_layer.h"
// #include "layers/game/gui_layer.h"
// #include "layers/loading/loading_layer.h"

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

int main() {
    // NineMorris3D* application = new NineMorris3D(INFO_FILE, LOG_FILE, APP_NAME);
    // NineMorris3D::set_app_pointer(application);

    // Scene* game_scene = new Scene("game");
    // game_scene->add_layer(new GameLayer("game", application));
    // game_scene->add_overlay(new GuiLayer("gui", application));
    // game_scene->add_overlay(new ImGuiLayer("imgui", application));
    // application->add_scene(game_scene);

    // Scene* loading_scene = new Scene("loading");
    // loading_scene->add_overlay(new LoadingLayer("loading", application));
    // application->add_scene(loading_scene);

    // application->set_starting_scene(loading_scene);
    // application->run();

    // delete application;

    std::unique_ptr<NineMorris3D> application = std::make_unique<NineMorris3D>(INFO_FILE, LOG_FILE, APP_NAME);  // TODO improve initialization
    application->add_scene(new GameScene, true);
    application->run();
}
