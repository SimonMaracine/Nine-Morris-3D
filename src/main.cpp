#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/nine_morris_3d.h"
#include "scenes/standard_game_scene.h"
#include "scenes/loading_scene.h"

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
    logging::initialize(LOG_FILE);

    auto builder = ApplicationBuilder {}
        .display_config(1024, 576, "Nine Morris 3D", 512, 288)
        .file_names_config(APP_NAME, INFO_FILE)
        .version_config(0, 2, 0)
        .authors_config({ u8"Simon Mărăcine" })
        .encrypt_key_config("data/models/board/board.obj")
        .with(ApplicationBuilder::Renderer::R3D)
        .with(ApplicationBuilder::Renderer::R2D)
        .with(ApplicationBuilder::Renderer::RImGui);

    auto application = std::make_unique<NineMorris3D>(builder);
    application->add_scene(new LoadingScene, true);
    application->add_scene(new StandardGameScene, false);
    application->run();
}
