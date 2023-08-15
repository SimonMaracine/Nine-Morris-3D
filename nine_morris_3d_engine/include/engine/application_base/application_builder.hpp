#pragma once

#include <string>
#include <string_view>

namespace sm {
    /*
        Parameters for an instance of an application or a game
    */
    class ApplicationBuilder {
    public:
        ApplicationBuilder& display(int width, int height, std::string_view title = "Nine Morris 3D Engine");
        ApplicationBuilder& display_flags(bool fullscreen, bool native_resolution, bool resizable);
        ApplicationBuilder& display_min_resolution(int min_width, int min_height);
        ApplicationBuilder& application_name(std::string_view app_name);
        ApplicationBuilder& version(unsigned int major, unsigned int minor, unsigned int patch);
        ApplicationBuilder& encrypt_key(std::string_view encryption_key);
        ApplicationBuilder& with_audio();
        ApplicationBuilder& with_random_generator();
    private:
        int width = 800;
        int height = 600;
        std::string title = "Nine Morris 3D Engine";
        bool fullscreen = false;
        bool native_resolution = true;
        bool resizable = true;
        int min_width = -1;
        int min_height = -1;
        std::string app_name = "NineMorris3DEngine";
        unsigned int major = 0;
        unsigned int minor = 1;
        unsigned int patch = 0;
        std::string encryption_key = "Nine Morris 3D Engine";
        bool audio = false;
        bool random_generator = false;

        friend class Application;
        friend class Window;
    };
}
