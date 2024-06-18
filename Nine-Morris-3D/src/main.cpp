#include <iostream>

// Include entry point first as it includes Windows.h
#include <engine/application_base/entry_point.hpp>

#include <engine/nine_morris_3d.hpp>
#include <engine/external/resmanager.h++>

#include "game/scenes/game_scene.hpp"

#if defined(SM_PLATFORM_LINUX)
    static const char* APP_NAME {"ninemorris3d"};
#elif defined(SM_PLATFORM_WINDOWS)
    static const char* APP_NAME {"NineMorris3D"};
#endif

static const char* LOG_FILE {"debug.log"};
static const char* INFO_FILE {"info.txt"};

static constexpr unsigned int MAJOR {0};
static constexpr unsigned int MINOR {4};
static constexpr unsigned int PATCH {0};

int application_main() {
    sm::ApplicationsData data;
    data.app_name = APP_NAME;
    data.log_file = LOG_FILE;
    data.info_file = INFO_FILE;
    data.res_directory = "data";

    if (!sm::Application::initialize_applications(data)) {
        std::cerr << "Fatal: Could not retrieve username\n";
        return 1;
    }

    while (true) {
        int exit_code {};

        {

            sm::ApplicationProperties properties;
            properties.width = 1024;
            properties.height = 576;
            properties.min_width = 512;
            properties.min_height = 288;
            properties.app_name = APP_NAME;
            properties.version_major = MAJOR;
            properties.version_minor = MINOR;
            properties.version_patch = PATCH;

            try {
                auto game {sm::Application(properties)};
                game.add_scene<GameScene>();
                exit_code = game.run("loading"_H);
            } catch (sm::RuntimeError error) {
                LOG_DIST_INFO("Terminated game with error code {}", error);

                return 1;
            }
        }

        if (exit_code == 0) {
            LOG_DIST_INFO("Terminated game successfully");

            return 0;
        }
    }
}
