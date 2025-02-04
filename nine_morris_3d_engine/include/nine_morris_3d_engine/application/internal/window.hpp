#pragma once

#include <memory>
#include <initializer_list>

#include <resmanager/resmanager.hpp>

#include "nine_morris_3d_engine/application/internal/event_dispatcher.hpp"
#include "nine_morris_3d_engine/application/properties.hpp"
#include "nine_morris_3d_engine/application/id.hpp"
#include "nine_morris_3d_engine/graphics/texture_data.hpp"

struct SDL_Window;

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
        int get_width() const;

        // Get current window height
        int get_height() const;

        // Show the window (it is always created hidden)
        void show() const;

        // Set VSync
        void set_vsync(bool enable) const;

        // Set application icons
        void set_icons(std::initializer_list<std::unique_ptr<TextureData>> icons) const;

        // Set window size
        void set_size(int width, int height);

        // Get the time in seconds since the window has been initialized
        static double get_time();

        // Swap buffers
        void flip() const;

        // Update events
        void poll_events();

        // Get handle to the window
        SDL_Window* get_window() const { return m_window; }

        // Get handle to the OpenGL context
        void* get_context() const { return m_context; }
    private:
        int m_width {};
        int m_height {};

        SDL_Window* m_window {};
        void* m_context {};

        EventDispatcher& m_evt;
    };
}
