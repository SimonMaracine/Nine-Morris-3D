#pragma once

#include <string>

namespace sm {
    /*
        Parameters for an instance of an application or a game
    */
    class ApplicationBuilder {
    private:
        static constexpr const char* TITLE {"Nine Morris 3D Engine"};
    public:
        ApplicationBuilder& display(int width, int height, const std::string& title = TITLE);
        ApplicationBuilder& display_flags(bool fullscreen, bool native_resolution, bool resizable);
        ApplicationBuilder& display_min_resolution(int min_width, int min_height);
        ApplicationBuilder& application_name(const std::string& app_name);
        ApplicationBuilder& version(unsigned int major, unsigned int minor, unsigned int patch);
        ApplicationBuilder& encrypt_key(const std::string& encryption_key);
        ApplicationBuilder& with_audio();
        ApplicationBuilder& with_random_generator();
    private:
        int width {800};
        int height {600};
        std::string title {TITLE};
        bool fullscreen {false};
        bool native_resolution {true};
        bool resizable {true};
        int min_width {-1};
        int min_height {-1};
        std::string app_name {"NineMorris3DEngine"};
        unsigned int major {0};
        unsigned int minor {1};
        unsigned int patch {0};
        std::string encryption_key {TITLE};
        bool audio {false};
        bool random_generator {false};

        friend class Application;
        friend class Window;
    };
}
