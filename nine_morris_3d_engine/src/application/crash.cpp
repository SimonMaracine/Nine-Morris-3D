#include "nine_morris_3d_engine/application/crash.hpp"

#include <SDL3/SDL.h>

#include "nine_morris_3d_engine/external/process_asio.h++"  // Should have included the headers directly
#include "nine_morris_3d_engine/application/internal/error.hpp"

namespace boost_process = boost::process::v2;

namespace sm {
    void launch_process_with_crash_handler(
        std::filesystem::path executable,
        std::initializer_list<std::string> arguments,
        std::function<void(int)>&& crash_handler,
        bool find_executable
    ) {
        boost::filesystem::path executable_ {executable.string()};

        if (find_executable) {
            executable_ = boost_process::environment::find_executable(executable_);

            if (executable_.empty()) {
                throw internal::OtherError("Could not find executable");
            }
        }

        boost::asio::io_context context;
        int exit_code {0};

        try {
            auto process {boost_process::process(context, executable_, arguments)};
            exit_code = process.wait();
        } catch (const boost_process::system_error& e) {
            throw internal::OtherError("An error occurred with the process: " + std::string(e.what()));
        }

        if (exit_code != 0) {
            crash_handler(exit_code);
        }
    }

    void show_error_window(const std::string& title, const std::string& message) {
        if (!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), message.c_str(), nullptr)) {
            throw internal::VideoError("Could not show error window: " + std::string(SDL_GetError()));
        }
    }
}
