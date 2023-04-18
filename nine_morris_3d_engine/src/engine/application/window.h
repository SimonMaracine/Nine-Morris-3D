#pragma once

#include <resmanager/resmanager.h>

#include "engine/application/platform.h"
#include "engine/graphics/texture_data.h"

struct GLFWwindow;
struct GLFWcursor;
struct GLFWmonitor;
class Application;
class Monitor;

class Window {
private:
    using CursorId = resmanager::HashedStr64;
    using CursorHashFunction = resmanager::Hash<CursorId>;
public:
    Window(Application* application);
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

    // VSync, cursor and icon API
    void show();
    void set_vsync(int interval);
    void add_cursor(CursorId id, std::unique_ptr<TextureData>&& cursor, int x_hotspot, int y_hotspot);
    void set_cursor(CursorId id);
    void set_icons(std::initializer_list<std::unique_ptr<TextureData>> icons);

    static void destroy_glfw_context();
private:
    GLFWwindow* create_window(Application* application);
    void install_callbacks();

    GLFWwindow* window = nullptr;
    std::unordered_map<CursorId, GLFWcursor*, CursorHashFunction> cursors;
};

class Monitor {
public:
    Monitor() = default;
    ~Monitor() = default;

    Monitor(const Monitor&) = default;
    Monitor& operator=(const Monitor&) = default;
    Monitor(Monitor&&) noexcept = default;
    Monitor& operator=(Monitor&&) noexcept = default;

    std::pair<int, int> get_resolution();
    std::pair<float, float> get_content_scale();
    const char* get_name();
private:
    GLFWmonitor* monitor = nullptr;

    friend class Window;
};
