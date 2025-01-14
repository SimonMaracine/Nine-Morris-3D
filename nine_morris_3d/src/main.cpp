#include <iostream>
#include <string>
#include <cstdlib>
#include <stdexcept>
#include <filesystem>

// Include entry point first as it includes Windows.h
#include <nine_morris_3d_engine/entry_point.hpp>
#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <nine_morris_3d_engine/external/resmanager.h++>

#include "scenes/loading_scene.hpp"
#include "scenes/nine_mens_morris_scene.hpp"
#include "scenes/jump_variant_scene.hpp"
#include "scenes/jump_plus_variant_scene.hpp"
#include "game.hpp"
#include "global.hpp"
#include "constants.hpp"
#include "ver.hpp"

struct Paths {
    std::string logs;
    std::string saved_data;
    std::string assets;
};

[[maybe_unused]] static const char* get_environment_variable(const char* variable) {
    const char* value {std::getenv(variable)};

    if (value == nullptr) {
        throw std::runtime_error("Could not get `" + std::string(variable) + "` environment variable");
    }

    return value;
}

static Paths get_paths() {
#ifndef SM_BUILD_DISTRIBUTION
    return {"", "", ""};
#else

#if defined(SM_PLATFORM_LINUX)
    const std::filesystem::path home_directory {get_environment_variable("HOME")};

    return {
        (home_directory / ".ninemorris3d").string(),
        (home_directory / ".ninemorris3d").string(),
        "/usr/local/share/ninemorris3d"
    };
#elif defined(SM_PLATFORM_WINDOWS)
    const std::string username {get_environment_variable("USERNAME")};

    return {
        "C:\\Users\\" + username + "\\Documents\\NineMorris3D",
        "C:\\Users\\" + username + "\\Documents\\NineMorris3D",
        ""
    };
#endif

#endif  // SM_BUILD_DISTRIBUTION
}

int application_main() {
    Paths paths;

    try {
        paths = get_paths();
    } catch (const std::runtime_error& e) {
        std::cerr << "Error initializing paths: " << e.what() << '\n';
        return 1;
    }

    while (true) {
        int exit_code {};

        sm::ApplicationProperties properties;
        properties.width = DEFAULT_WIDTH;
        properties.height = DEFAULT_HEIGHT;
        properties.min_width = MIN_WIDTH;
        properties.min_height = MIN_HEIGHT;
        properties.title = "Nine Morris 3D";
        properties.log_file = "log.txt";
        properties.version_major = VERSION_MAJOR;
        properties.version_minor = VERSION_MINOR;
        properties.version_patch = VERSION_PATCH;
        properties.path_logs = paths.logs;
        properties.path_saved_data = paths.saved_data;
        properties.path_assets = paths.assets;
        properties.default_renderer_parameters = false;
        // properties.audio = true;  // TODO

        sm::UserFunctions functions;
        functions.start = game_start;
        functions.stop = game_stop;

        try {
            sm::Application game {properties};
            game.add_scene<LoadingScene>();
            game.add_scene<StandardGameScene>();
            // game.add_scene<JumpVariantScene>();
            // game.add_scene<JumpPlusVariantScene>();
            game.set_global_data<Global>();
            exit_code = game.run("loading"_H, functions);
        } catch (const sm::RuntimeError& e) {  // FIXME once an exception from a thread managed to not be caught; don't know how; seems to be working now
            std::cerr << "Terminated game with error: " << e.what() << '\n';
            return 1;
        }

        if (exit_code == 0) {
            return 0;
        }
    }
}
