#include <iostream>
#include <filesystem>
#include <cstring>

// Include entry point first as it includes Windows.h
#include <nine_morris_3d_engine/entry_point.hpp>
#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <nine_morris_3d_engine/external/resmanager.h++>

#include "scenes/loading_scene.hpp"
#include "scenes/nine_mens_morris_scene.hpp"
#include "scenes/twelve_mens_morris_scene.hpp"
#include "game.hpp"
#include "global.hpp"
#include "window_size.hpp"
#include "ver.hpp"

struct Paths {
    std::filesystem::path logs;
    std::filesystem::path saved_data;
    std::filesystem::path assets;
};

static Paths get_paths() {
#ifndef SM_BUILD_DISTRIBUTION
    return {};
#else

#if defined(SM_PLATFORM_LINUX)
    const std::filesystem::path home {sm::utils::get_environment_variable("HOME")};

    return {
        home / ".ninemorris3d",
        home / ".ninemorris3d",
        "/usr/local/share/ninemorris3d"
    };
#elif defined(SM_PLATFORM_WINDOWS)
    const std::filesystem::path username {sm::utils::get_environment_variable("USERNAME")};

    return {
        "C:\\Users" / username / "Documents\\Nine Morris 3D",
        "C:\\Users" / username / "Documents\\Nine Morris 3D",
        ""
    };
#endif

#endif  // SM_BUILD_DISTRIBUTION
}

static int game() {
    Paths paths;

    try {
        paths = get_paths();
    } catch (const sm::RuntimeError& e) {
        std::cerr << "Error initializing paths: " << e.what() << '\n';
        return 1;
    }

    sm::ApplicationProperties properties;
    properties.width = DEFAULT_WIDTH;
    properties.height = DEFAULT_HEIGHT;
    properties.min_width = MIN_WIDTH;
    properties.min_height = MIN_HEIGHT;
    properties.title = "Nine Morris 3D";
    properties.log_file = "nine_morris_3d.log";
    properties.version_major = VERSION_MAJOR;
    properties.version_minor = VERSION_MINOR;
    properties.version_patch = VERSION_PATCH;
    properties.path_logs = paths.logs;
    properties.path_saved_data = paths.saved_data;
    properties.path_assets = paths.assets;
    properties.build_date = __DATE__;
    properties.build_time = __TIME__;
    properties.default_renderer_parameters = false;

    sm::UserFunctions functions;
    functions.start = game_start;
    functions.stop = game_stop;

    try {
        sm::Application game {properties};
        game.add_scene<LoadingScene>();
        game.add_scene<NineMensMorrisScene>();
        game.add_scene<TwelveMensMorrisScene>();
        game.set_global_data<Global>();
        game.run("loading"_H, functions);
    } catch (const sm::RuntimeError& e) {
        std::cerr << "Terminated game with error: " << e.type() << ": " << e.what() << '\n';
        return 1;
    }

    return 0;
}

static void crash_handler(int) {
    const char* title {"Nine Morris 3D Fatal Error"};
    const char* message {"Sorry! Nine Morris 3D encountered a fatal error!\nPlease take a look at the logs and consider reporting the event to the developer."};

    try {
        sm::crash::show_error_window(title, message);
    } catch (const sm::RuntimeError&) {
        std::cerr << title << ": " << message << '\n';
    }
}

int sm_application_main(int argc, char** argv) {
    // argv can be empty; avoid an infinite loop
    if (argc > 1 && std::strcmp(argv[1], "--game") == 0 || argc == 0) {
        return game();
    }

#ifdef SM_BUILD_DISTRIBUTION

#ifdef SM_PLATFORM_WINDOWS
    const char* executable {"nine_morris_3d.exe"};
#else
    const char* executable {"nine_morris_3d"};
#endif

#else

#ifdef SM_PLATFORM_WINDOWS
    const char* executable {"build/nine_morris_3d/Debug/nine_morris_3d.exe"};
#else
    const char* executable {"build/nine_morris_3d/nine_morris_3d"};
#endif

#endif  // SM_BUILD_DISTRIBUTION

#if defined(SM_BUILD_DISTRIBUTION) && defined(SM_PLATFORM_LINUX)
    const bool find_executable {true};
#else
    const bool find_executable {false};
#endif

    try {
        sm::crash::launch_process_with_crash_handler(executable, {"--game"}, crash_handler, find_executable);
    } catch (const sm::RuntimeError& e) {
        std::cerr << "A fatal error occurred in the crash handler: " << e.what() << '\n';

        try {
            sm::crash::show_error_window(
                "Nine Morris 3D Fatal Error",
                "Sorry! A fatal error occurred.\nPlease consider reporting the event to the developer.\n\n" + std::string(e.what())
            );
        } catch (...) {}

        return 1;
    }

    return 0;
}
