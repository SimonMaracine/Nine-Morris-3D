#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "nine_morris_3d.h"
#include "scenes/game_scene.h"

#if defined(PLATFORM_GAME_LINUX)
    #define APP_NAME "ninemorris3d"
#elif defined(PLATFORM_GAME_WINDOWS)
    #define APP_NAME "NineMorris3D"
#endif

#if defined(PLATFORM_GAME_LINUX)
    #define LOG_FILE "log.txt"
    #define INFO_FILE "info.txt"
#elif defined(PLATFORM_GAME_WINDOWS)
    #define LOG_FILE "ninemorris3d_log.txt"
    #define INFO_FILE "ninemorris3d_info.txt"
#endif

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

    logging::initialize(LOG_FILE);

    auto builder = ApplicationBuilder {}
        .display_config(1024, 576, "Nine Morris 3D", 512, 288)
        .file_names_config(APP_NAME, INFO_FILE)
        .version_config(0, 2, 0)
        .authors_config({ u8"Simon Mărăcine" })
        .with(ApplicationBuilder::Renderer::R3D)
        .with(ApplicationBuilder::Renderer::R2D)
        .with(ApplicationBuilder::Renderer::RImGui);

    auto application = std::make_unique<NineMorris3D>(builder);
    application->add_scene(new GameScene, true);
    application->run();
}
