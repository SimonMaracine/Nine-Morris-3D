#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/game.h"
#include "game/scenes/standard_game_scene.h"
#include "game/scenes/loading_scene.h"
#include "launcher/launcher.h"
#include "launcher/scenes/launcher_scene.h"
#include "other/data.h"

#if defined(PLATFORM_GAME_LINUX)
    #define APP_NAME "ninemorris3d"
    #define LOG_FILE "log.txt"
    #define INFO_FILE "info.txt"
#elif defined(PLATFORM_GAME_WINDOWS)
    #define APP_NAME "NineMorris3D"
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

    logging::initialize_for_applications(LOG_FILE);

    while (true) {
        int exit_code {};

        auto launcher_builder = ApplicationBuilder {}
            .display(640, 480, "Nine Morris 3D Launcher")
            .display_flags(false, false, false)
            .file_names(APP_NAME, INFO_FILE)
            .version(MAJOR, MINOR, PATCH)
            .authors(authors)
            .encrypt_key(KEY)
            .with_renderer(ApplicationBuilder::RImGui)
            .with_renderer(ApplicationBuilder::R2D);

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
            .display(options.resolution.first, options.resolution.second, "Nine Morris 3D")
            .display_flags(options.fullscreen, options.native_resolution, true)
            .display_min_resolution(512, 288)
            .file_names(APP_NAME, INFO_FILE)
            .version(MAJOR, MINOR, PATCH)
            .authors(authors)
            .encrypt_key(KEY)
            .with_renderer(ApplicationBuilder::R3D)
            .with_renderer(ApplicationBuilder::R2D)
            .with_renderer(ApplicationBuilder::RImGui);

        auto game = std::make_unique<Application>(game_builder, data, game::start, game::stop);
        game->add_scene(new LoadingScene, true);
        game->add_scene(new StandardGameScene, false);
        exit_code = game->run();

        if (exit_code == 0) {
            return 0;
        }
    }
}
