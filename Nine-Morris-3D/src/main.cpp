#include <iostream>
#include <memory>

// Include entry point first as it includes Windows.h
#include <nine_morris_3d_engine/application/entry_point.hpp>
#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <nine_morris_3d_engine/external/resmanager.h++>

#include "game/scenes/game_scene.hpp"
#include "game/game.hpp"
#include "game/global.hpp"

#if defined(SM_PLATFORM_LINUX)
    static const char* APP_NAME {"ninemorris3d"};
#elif defined(SM_PLATFORM_WINDOWS)
    static const char* APP_NAME {"NineMorris3D"};
#endif

static const char* LOG_FILE {"debug.log"};
static const char* INFO_FILE {"info.txt"};
static const char* ASSETS_DIRECTORY {"assets"};

static constexpr unsigned int MAJOR {0};
static constexpr unsigned int MINOR {4};
static constexpr unsigned int PATCH {0};

int application_main() {
    while (true) {
        int exit_code {};

        {
            sm::ApplicationProperties properties;
            properties.width = 1024;
            properties.height = 576;
            properties.min_width = 512;
            properties.min_height = 288;
            properties.version_major = MAJOR;
            properties.version_minor = MINOR;
            properties.version_patch = PATCH;
            properties.application_name = APP_NAME;
            properties.info_file = INFO_FILE;
            properties.log_file = LOG_FILE;
            properties.assets_directory = ASSETS_DIRECTORY;

            sm::UserFunctions functions;
            functions.start = game_start;
            functions.stop = game_stop;

            try {
                auto game {sm::Application(properties)};
                game.add_scene<GameScene>();
                game.set_global_data<Global>();
                exit_code = game.run("game"_H, functions);
            } catch (sm::RuntimeError error) {
                std::cerr << "Terminated game with error code " << static_cast<int>(error) << '\n';
                return 1;
            }
        }

        if (exit_code == 0) {
            return 0;
        }
    }
}
