#pragma once

#include <memory>
#include <unordered_map>
#include <initializer_list>

#include <resmanager/resmanager.hpp>

#include "nine_morris_3d_engine/application/internal/event_dispatcher.hpp"
#include "nine_morris_3d_engine/application/monitors.hpp"
#include "nine_morris_3d_engine/application/properties.hpp"
#include "nine_morris_3d_engine/application/id.hpp"
#include "nine_morris_3d_engine/graphics/texture_data.hpp"

struct GLFWwindow;
struct GLFWcursor;

namespace sm::internal {
    // Window management API
    class Window {
    public:
        Window(const ApplicationProperties& properties, EventDispatcher& evt);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        // Get window current width
        int get_width() const noexcept;

        // Get current window height
        int get_height() const noexcept;

        // Show the window (it is always created hidden)
        void show() const noexcept;

        // Set VSync
        void set_vsync(bool enable) const noexcept;

        // Add a cursor for the application to use
        // Refer to the cursor by its ID
        void add_cursor(Id id, std::unique_ptr<TextureData>&& cursor, int x_hotspot, int y_hotspot);

        // Set the current cursor
        // Set to "null" to reset to the default cursor
        void set_cursor(Id id) const;

        // Set application icons
        // You may refer to the documentation https://www.glfw.org/docs/latest/window_guide.html
        void set_icons(std::initializer_list<std::unique_ptr<TextureData>> icons) const;

        // Set window dimensions
        void set_dimensions(int width, int height) noexcept;

        // Get the current display monitors
        Monitors get_monitors() const;

        // Get the time in seconds since the window has been initialized
        static double get_time() noexcept;

        // Get handle to the window
        GLFWwindow* get_handle() const noexcept;

        // Swap buffers and update events
        void update() const noexcept;
    private:
        void create_window(const ApplicationProperties& properties);
        void install_callbacks() const noexcept;

        int m_width {};
        int m_height {};

        GLFWwindow* m_window {};
        std::unordered_map<Id, GLFWcursor*, Hash> m_cursors;
        EventDispatcher& m_evt;
    };
}
