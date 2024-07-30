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
    class Window {
    public:
        Window(const ApplicationProperties& properties, EventDispatcher* evt);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        int get_width() const noexcept;
        int get_height() const noexcept;

        void show() const noexcept;
        void set_vsync(int interval) const noexcept;
        void add_cursor(Id id, std::unique_ptr<TextureData>&& cursor, int x_hotspot, int y_hotspot);
        void set_cursor(Id id) const;
        void set_icons(std::initializer_list<std::unique_ptr<TextureData>> icons) const;

        Monitors get_monitors() const;
        static double get_time() noexcept;

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
        EventDispatcher* m_evt {};
    };
}
