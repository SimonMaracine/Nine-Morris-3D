#include <engine/engine_application.h>
#include <engine/engine_entry_point.h>
#include <engine/engine_other.h>

#include "game/game.h"
#include "game/scenes/standard_game_scene.h"
#include "game/scenes/loading_scene.h"
#include "launcher/launcher.h"
#include "launcher/scenes/launcher_scene.h"
#include "other/data.h"

#if defined(NM3D_PLATFORM_LINUX)
    #define APP_NAME "ninemorris3d"
#elif defined(NM3D_PLATFORM_WINDOWS)
    #define APP_NAME "NineMorris3D"
#endif

#define LOG_FILE "debug.log"
#define INFO_FILE "info.txt"

void application_main() {
    constexpr unsigned int MAJOR = 0;
    constexpr unsigned int MINOR = 2;
    constexpr unsigned int PATCH = 0;
    const std::vector<std::string> authors = {
        u8"Simon Mărăcine"
    };
    constexpr const char* KEY = "data/models/board/board.obj";

    path::initialize_for_applications(APP_NAME);
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
            .with_renderer(ApplicationBuilder::RendererImGui)
            .with_renderer(ApplicationBuilder::Renderer2D);

        auto data = std::make_any<Data>();

        auto launcher = std::make_unique<Application>(launcher_builder, data, launcher::start);
        launcher->add_scene(std::make_unique<LauncherScene>(), true);
        exit_code = launcher->run();
        launcher.reset();

        if (exit_code == 1) {
            break;
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
            .with_renderer(ApplicationBuilder::Renderer3D)
            .with_renderer(ApplicationBuilder::Renderer2D)
            .with_renderer(ApplicationBuilder::RendererImGui)
            .with_audio();

        auto game = std::make_unique<Application>(game_builder, data, game::start, game::stop);
        game->add_scene(std::make_unique<LoadingScene>(), true);
        game->add_scene(std::make_unique<StandardGameScene>(), false);
        exit_code = game->run();

        if (exit_code == 0) {
            break;
        }
    }

    REL_INFO("Terminated game successfully");
}
