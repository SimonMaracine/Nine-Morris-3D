#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/game.h"
#include "game/scenes/standard_game_scene.h"
#include "game/scenes/loading_scene.h"
#include "launcher/launcher.h"
#include "launcher/launcher_scene.h"
#include "other/data.h"

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
    constexpr unsigned int MAJOR = 0;
    constexpr unsigned int MINOR = 2;
    constexpr unsigned int PATCH = 0;
    const std::vector<std::string> authors = {
        u8"Simon Mărăcine"
    };
    constexpr const char* KEY = "data/models/board/board.obj";

    logging::initialize(LOG_FILE);

    while (true) {
        int exit_code {};

        auto launcher_builder = ApplicationBuilder {}
            .display_config(640, 480, "Nine Morris 3D Launcher", false, false, false)
            .file_names_config(APP_NAME, INFO_FILE)
            .version_config(MAJOR, MINOR, PATCH)
            .authors_config(authors)
            .encrypt_key_config(KEY)
            .with(ApplicationBuilder::Renderer::RImGui)
            .with(ApplicationBuilder::Renderer::R2D);

        auto data = std::make_any<Data>();

        auto launcher = std::make_unique<Application>(launcher_builder, data, launcher::start);
        launcher->add_scene(new LauncherScene, true);
        exit_code = launcher->run();
        launcher.reset();

        if (exit_code == 1) {
            return 0;
        }

        const auto& options = std::any_cast<Data>(data).launcher_options;

        auto game_builder = ApplicationBuilder {}
            .display_config(
                options.resolution.first, options.resolution.second, "Nine Morris 3D",
                options.fullscreen, options.native_resolution, true, 512, 288
            )
            .file_names_config(APP_NAME, INFO_FILE)
            .version_config(MAJOR, MINOR, PATCH)
            .authors_config(authors)
            .encrypt_key_config(KEY)
            .with(ApplicationBuilder::Renderer::R3D)
            .with(ApplicationBuilder::Renderer::R2D)
            .with(ApplicationBuilder::Renderer::RImGui);

        auto game = std::make_unique<Application>(game_builder, data, game::start, game::stop);
        game->add_scene(new LoadingScene, true);
        game->add_scene(new StandardGameScene, false);
        exit_code = game->run();

        if (exit_code == 0) {
            return 0;
        }
    }
}
