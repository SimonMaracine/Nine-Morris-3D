#pragma once

#include <memory>
#include <unordered_map>
#include <initializer_list>
#include <utility>
#include <vector>

#include <resmanager/resmanager.hpp>

#include "engine/application_base/platform.hpp"
#include "engine/graphics/texture_data.hpp"

struct GLFWwindow;
struct GLFWcursor;
struct GLFWmonitor;

namespace sm {
    class Application;
    class Monitor;

    class Window {
    public:
        using CursorId = resmanager::HashedStr64;
        using CursorHash = resmanager::Hash<CursorId>;

        Window(Application* application);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        // Call this to swap buffers and update events
        void update();

        GLFWwindow* get_handle();
        std::vector<Monitor> get_monitors();

        // VSync, cursor and icon API
        void show();
        void set_vsync(int interval);
        void add_cursor(CursorId id, std::unique_ptr<TextureData>&& cursor, int x_hotspot, int y_hotspot);
        void set_cursor(CursorId id);
        void set_icons(std::initializer_list<std::unique_ptr<TextureData>> icons);

        static double get_time();
    private:
        GLFWwindow* create_window(Application* application);
        void install_callbacks();

        GLFWwindow* window {nullptr};
        std::unordered_map<CursorId, GLFWcursor*, CursorHash> cursors;
    };

    class Monitor {
    public:
        Monitor() = default;
        ~Monitor() = default;

        Monitor(const Monitor&) = default;
        Monitor& operator=(const Monitor&) = default;
        Monitor(Monitor&&) noexcept = default;
        Monitor& operator=(Monitor&&) noexcept = default;

        std::pair<int, int> get_resolution() const;
        std::pair<float, float> get_content_scale() const;
        const char* get_name() const;
    private:
        GLFWmonitor* monitor {nullptr};

        friend class Window;
    };
}
