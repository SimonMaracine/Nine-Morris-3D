#pragma once

class Application;

class ApplicationData {
public:
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
    Application* app = nullptr;

    friend class Application;
    friend class Window;
};
