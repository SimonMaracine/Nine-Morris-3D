#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <initializer_list>

namespace sm {
    // Start a new child process and wait for it to terminate
    // Call crash_handler, if the process terminates unsuccessfully
    void launch_process_with_crash_handler(
        std::filesystem::path executable,
        std::initializer_list<std::string> arguments,
        std::function<void(int)>&& crash_handler,
        bool find_executable = false
    );

    // Attempt to display a window indicating a fatal error
    void show_error_window(const std::string& title, const std::string& message);
}
