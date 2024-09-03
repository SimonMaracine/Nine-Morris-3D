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
#include "scenes/standard_game_scene.hpp"
#include "game.hpp"
#include "global.hpp"
#include "constants.hpp"
#include "ver.hpp"

struct Paths {
    std::string logs;
    std::string saved_data;
    std::string assets;
};

static void get_paths_linux(Paths& paths) {
    const char* home {std::getenv("HOME")};

    if (home == nullptr) {
        throw std::runtime_error("Could not get `HOME` environment variable");
    }

    const std::filesystem::path home_directory {home};

    paths.logs = (home_directory / ".ninemorris3d").string();
    paths.saved_data = (home_directory / ".ninemorris3d").string();
    paths.assets = "/usr/local/share/ninemorris3d";
}

static void get_paths_windows(Paths& paths) {
    const char* username {std::getenv("USERNAME")};

    if (username == nullptr) {
        throw std::runtime_error("Could not get `USERNAME` environment variable");
    }

    paths.logs = "C:\\Users\\" + std::string(username) + "\\Documents\\NineMorris3D";
    paths.saved_data = "C:\\Users\\" + std::string(username) + "\\Documents\\NineMorris3D";
    paths.assets = "";
}

static Paths get_paths() {
    Paths paths;

#ifndef SM_BUILD_DISTRIBUTION
    paths.logs = "";
    paths.saved_data = "";
    paths.assets = "";
#else

#if defined(SM_PLATFORM_LINUX)
    get_paths_linux(paths);
#elif defined(SM_PLATFORM_WINDOWS)
    get_paths_windows(paths);
#endif

#endif  // SM_BUILD_DISTRIBUTION

    return paths;
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
