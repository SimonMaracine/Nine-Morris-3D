// Include entry point first as it includes Windows.h
#include <engine/application_base/entry_point.hpp>

#include <engine/prelude.hpp>
#include <engine/application_base/application.hpp>
#include <engine/application_base/application_builder.hpp>

// #include <engine/public/application_base.h>
// #include <engine/public/other.h>
// #include <engine/public/external/resmanager.h++>

// #include "game/game.h"
// #include "game/scenes/standard_game_scene.h"
// #include "game/scenes/jump_variant_scene.h"
// #include "game/scenes/jump_plus_variant_scene.h"
// #include "game/scenes/loading_scene.h"
// #include "launcher/launcher.h"
// #include "launcher/scenes/launcher_scene.h"
// #include "other/data.h"

#if defined(SM_PLATFORM_LINUX)
    static const char* APP_NAME = "ninemorris3d";
#elif defined(SM_PLATFORM_WINDOWS)
    static const char* APP_NAME = "NineMorris3D";
#endif

static const char* LOG_FILE = "debug.log";
static const char* INFO_FILE = "info.txt";

static constexpr unsigned int MAJOR = 0;
static constexpr unsigned int MINOR = 4;
static constexpr unsigned int PATCH = 0;

static const char* KEY = "data/models/board/board.obj";

void application_main() {
    sm::Application::preinitialize(APP_NAME, LOG_FILE, INFO_FILE);

    while (true) {
        int exit_code {};

        Data* global_data = new Data;

        // {
        //     auto launcher_builder = sm::ApplicationBuilder {}
        //         .display(640, 480, "Nine Morris 3D Launcher")
        //         .display_flags(false, false, false)
        //         .application_name(APP_NAME)
        //         .version(MAJOR, MINOR, PATCH)
        //         .authors(AUTHORS)
        //         .encrypt_key(KEY)
        //         .with_dear_imgui()
        //         .with_renderer(sm::ApplicationBuilder::Renderer2D);

        //     auto launcher = sm::Application {launcher_builder, global_data};
        //     launcher.set_start_function(launcher::start);
        //     launcher.add_scene<LauncherScene>();
        //     exit_code = launcher.run("launcher"_H);
        // }

        if (exit_code == 1) {
            break;
        }

        {
            const auto& options = global_data->launcher_options;

            auto game_builder = sm::ApplicationBuilder {}
                .display(options.resolution.first, options.resolution.second, "Nine Morris 3D")
                .display_flags(options.fullscreen, options.native_resolution, true)
                .display_min_resolution(512, 288)
                .application_name(APP_NAME)
                .version(MAJOR, MINOR, PATCH)
                .encrypt_key(KEY)
                .with_renderer(sm::ApplicationBuilder::Renderer3D)
                .with_renderer(sm::ApplicationBuilder::Renderer2D)
                .with_dear_imgui()
                .with_audio()
                .with_random_generator();

            auto game = sm::Application {game_builder, global_data};
            game.set_start_function(game::start);
            game.set_stop_function(game::stop);
            game.add_scene<LoadingScene>();
            game.add_scene<StandardGameScene>();
            game.add_scene<JumpVariantScene>();
            game.add_scene<JumpPlusVariantScene>();
            exit_code = game.run("loading"_H);
        }

        if (exit_code == 0) {
            break;
        }
    }

    LOG_DIST_INFO("Terminated game successfully");
}
