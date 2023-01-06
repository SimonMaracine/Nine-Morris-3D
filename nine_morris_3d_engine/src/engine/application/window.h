#pragma once

#include "engine/application/platform.h"
#include "engine/graphics/texture_data.h"

struct GLFWwindow;
struct GLFWcursor;
struct GLFWmonitor;
class Application;
class Monitor;

class Window {
public:
    Window(Application* app);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    // Call this to swap buffers and update events
    void update();

    GLFWwindow* get_handle();
    double get_time();
    std::vector<Monitor> get_monitors();

    // Vsync, cursor and icon API
    void show();
    void set_vsync(int interval);
    unsigned int add_cursor(std::unique_ptr<TextureData>&& cursor, int x_hotspot, int y_hotspot);
    void set_cursor(unsigned int handle);
    void set_icons(const std::initializer_list<std::unique_ptr<TextureData>>& icons);
private:
    GLFWwindow* create_window(Application* app);

    GLFWwindow* window = nullptr;
    std::unordered_map<unsigned int, GLFWcursor*> cursors;
};

class Monitor {
public:
    Monitor() = default;
    ~Monitor() = default;

    std::pair<int, int> get_resolution();
    const char* get_name();
private:
    GLFWmonitor* monitor = nullptr;

    friend class Window;
};

void destroy_glfw_context();
