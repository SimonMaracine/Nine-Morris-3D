#pragma once

namespace sm {
    class Ctx;
    class Application;

    /**
     * Struct for mostly constant application properties.
     */
    class ApplicationProperties final {
    public:
        // These two change when the window resizes
        int width;
        int height;

        std::string title;
        bool fullscreen;
        bool native_resolution;
        bool resizable;
        int min_width;
        int min_height;
        std::string app_name;
        unsigned int version_major;
        unsigned int version_minor;
        unsigned int version_patch;
        std::vector<std::string> authors;
    private:
        Application* application = nullptr;
        Ctx* ctx = nullptr;

        friend class Application;
        friend class Window;
    };
}
