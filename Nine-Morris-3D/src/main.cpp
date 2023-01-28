#include <engine/engine_application.h>
#include <engine/engine_entry_point.h>
#include <engine/engine_other.h>

#include "game/game.h"
#include "game/scenes/standard_game_scene.h"
#include "game/scenes/jump_variant_scene.h"
#include "game/scenes/jump_plus_variant_scene.h"
#include "game/scenes/loading_scene.h"
#include "launcher/launcher.h"
#include "launcher/scenes/launcher_scene.h"
#include "other/data.h"

#if defined(NM3D_PLATFORM_LINUX)
    static const char* APP_NAME = "ninemorris3d";
#elif defined(NM3D_PLATFORM_WINDOWS)
    static const char* APP_NAME = "NineMorris3D";
#endif

static const char* LOG_FILE = "debug.log";
static const char* INFO_FILE = "info.txt";

static constexpr unsigned int MAJOR = 0;
static constexpr unsigned int MINOR = 3;
static constexpr unsigned int PATCH = 0;

static const char* KEY = "data/models/board/board.obj";

static const std::vector<std::string> authors = {
    u8"Simon Mărăcine"
};

void application_main() {
    Application::preinitialize(APP_NAME, LOG_FILE, INFO_FILE);

    while (true) {
        int exit_code {};

        auto launcher_builder = ApplicationBuilder {}
            .display(640, 480, "Nine Morris 3D Launcher")
            .display_flags(false, false, false)
            .application_name(APP_NAME)
            .version(MAJOR, MINOR, PATCH)
            .authors(authors)
            .encrypt_key(KEY)
            .with_renderer(ApplicationBuilder::RendererImGui)
            .with_renderer(ApplicationBuilder::Renderer2D);

        auto data = std::make_any<Data>();

        auto launcher = std::make_unique<Application>(launcher_builder, data, launcher::start);
        launcher->add_scene(std::make_unique<LauncherScene>());
        exit_code = launcher->run("launcher"_H);
        launcher.reset();

        if (exit_code == 1) {
            break;
        }

        const auto& options = std::any_cast<Data>(data).launcher_options;

        auto game_builder = ApplicationBuilder {}
            .display(options.resolution.first, options.resolution.second, "Nine Morris 3D")
            .display_flags(options.fullscreen, options.native_resolution, true)
            .display_min_resolution(512, 288)
            .application_name(APP_NAME)
            .version(MAJOR, MINOR, PATCH)
            .authors(authors)
            .encrypt_key(KEY)
            .with_renderer(ApplicationBuilder::Renderer3D)
            .with_renderer(ApplicationBuilder::Renderer2D)
            .with_renderer(ApplicationBuilder::RendererImGui)
            .with_audio();

        auto game = std::make_unique<Application>(game_builder, data, game::start, game::stop);
        game->add_scene(std::make_unique<LoadingScene>());
        game->add_scene(std::make_unique<StandardGameScene>());
        game->add_scene(std::make_unique<JumpVariantScene>());
        game->add_scene(std::make_unique<JumpPlusVariantScene>());
        exit_code = game->run("loading"_H);

        if (exit_code == 0) {
            break;
        }
    }

    REL_INFO("Terminated game successfully");
}
