#pragma once

#include <string>

namespace sm {
    class Ctx;
    class Application;

    /*
        Struct for mostly constant application properties
    */
    class ApplicationProperties {
    public:
        // These two change when the window resizes
        int width;
        int height;

        std::string title;  // TODO reorder members
        bool fullscreen;
        bool native_resolution;
        bool resizable;
        int min_width;
        int min_height;
        std::string app_name;
        unsigned int version_major;
        unsigned int version_minor;
        unsigned int version_patch;
    private:
        Application* application {nullptr};
        Ctx* ctx {nullptr};

        friend class Application;
        friend class Window;
    };
}
