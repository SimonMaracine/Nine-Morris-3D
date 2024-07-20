#pragma once

#include <memory>
#include <unordered_map>
#include <initializer_list>
#include <utility>
#include <cstddef>

#include <resmanager/resmanager.hpp>

#include "nine_morris_3d_engine/application/properties.hpp"
#include "nine_morris_3d_engine/application/events.hpp"
#include "nine_morris_3d_engine/application/id.hpp"
#include "nine_morris_3d_engine/graphics/texture_data.hpp"

struct GLFWwindow;
struct GLFWcursor;
struct GLFWmonitor;

namespace sm {
    namespace internal {
        class Window;
    }

    class Monitors {
    public:
        std::pair<int, int> get_resolution(std::size_t index) const;
        std::pair<float, float> get_content_scale(std::size_t index) const;
        const char* get_name(std::size_t index) const;
    private:
        GLFWmonitor** monitors {nullptr};
        std::size_t count {};

        friend class internal::Window;
    };

    namespace internal {
        class Window {
        public:
            Window(const ApplicationProperties& properties, EventDispatcher* evt);
            ~Window();

            Window(const Window&) = delete;
            Window& operator=(const Window&) = delete;
            Window(Window&&) = delete;
            Window& operator=(Window&&) = delete;

            int get_width() const;
            int get_height() const;

            void show() const;
            void set_vsync(int interval) const;
            void add_cursor(Id id, std::unique_ptr<TextureData>&& cursor, int x_hotspot, int y_hotspot);
            void set_cursor(Id id) const;
            void set_icons(std::initializer_list<std::unique_ptr<TextureData>> icons) const;

            const Monitors& get_monitors();
            static double get_time();

            GLFWwindow* get_handle() const;

            // Swap buffers and update events
            void update() const;
        private:
            GLFWwindow* create_window(const ApplicationProperties& properties);
            void install_callbacks() const;

            int width {};
            int height {};

            GLFWwindow* window {nullptr};
            Monitors monitors;
            std::unordered_map<Id, GLFWcursor*, Hash> cursors;
            EventDispatcher* evt {nullptr};
        };
    }
}
