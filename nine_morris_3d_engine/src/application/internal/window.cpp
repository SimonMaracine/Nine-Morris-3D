#include "nine_morris_3d_engine/application/internal/window.hpp"

#include <vector>
#include <iterator>
#include <cassert>

#include <glad/glad.h>
#include <SDL3/SDL.h>

#include "nine_morris_3d_engine/application/internal/input_codes.hpp"
#include "nine_morris_3d_engine/application/internal/error.hpp"
#include "nine_morris_3d_engine/application/events.hpp"
#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/graphics/internal/imgui_context.hpp"
#include "nine_morris_3d_engine/graphics/opengl/debug.hpp"

namespace sm::internal {
    Window::Window(const ApplicationProperties& properties, EventDispatcher& evt)
        : m_width(properties.width), m_height(properties.height), m_evt(evt) {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SM_THROW_ERROR(VideoError, "Could not initialize SDL: {}", SDL_GetError());
        }

        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4)) {
            SDL_Quit();
            SM_THROW_ERROR(VideoError, "Could not set SDL_GL_CONTEXT_MAJOR_VERSION attribute: {}", SDL_GetError());
        }

        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3)) {
            SDL_Quit();
            SM_THROW_ERROR(VideoError, "Could not set SDL_GL_CONTEXT_MINOR_VERSION attribute: {}", SDL_GetError());
        }

        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)) {
            SDL_Quit();
            SM_THROW_ERROR(VideoError, "Could not set SDL_GL_CONTEXT_PROFILE_MASK attribute: {}", SDL_GetError());
        }

        if (!SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)) {
            SDL_Quit();
            SM_THROW_ERROR(VideoError, "Could not set SDL_GL_DOUBLEBUFFER attribute: {}", SDL_GetError());
        }

        if (!SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24)) {
            SDL_Quit();
            SM_THROW_ERROR(VideoError, "Could not set SDL_GL_DEPTH_SIZE attribute: {}", SDL_GetError());
        }

        if (!SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1)) {
            LOG_DIST_ERROR("Could not set SDL_GL_FRAMEBUFFER_SRGB_CAPABLE attribute: {}", SDL_GetError());
        }

        unsigned int flags {SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN};

        if (properties.resizable) {
            flags |= SDL_WINDOW_RESIZABLE;
        }

        if (properties.fullscreen) {
            flags |= SDL_WINDOW_FULLSCREEN;
        }

        m_window = SDL_CreateWindow(properties.title, properties.width, properties.height, flags);

        if (m_window == nullptr) {
            SDL_Quit();
            SM_THROW_ERROR(VideoError, "Could not create window: {}", SDL_GetError());
        }

        m_context = SDL_GL_CreateContext(m_window);

        if (m_context == nullptr) {
            SDL_Quit();
            SM_THROW_ERROR(VideoError, "Could not create OpenGL context: {}", SDL_GetError());
        }

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
            SDL_Quit();
            SM_THROW_ERROR(VideoError, "Could not initialize GLAD");
        }

        if (!SDL_GL_SetSwapInterval(1)) {
            LOG_DIST_ERROR("Could not set swap interval: {}", SDL_GetError());
        }

        if (!SDL_SetWindowMinimumSize(m_window, properties.min_width, properties.min_height)) {
            LOG_DIST_ERROR("Could not set minimum window size: {}", SDL_GetError());
        }

#ifndef SM_BUILD_DISTRIBUTION
        opengl_debug::initialize();
#endif

        LOG_INFO("Initialized SDL and created window and OpenGL context");
    }

    Window::~Window() {
        SDL_GL_DestroyContext(static_cast<SDL_GLContext>(m_context));
        SDL_DestroyWindow(m_window);
        SDL_Quit();

        LOG_INFO("Destroyed OpenGL context and window and terminated SDL");
    }

    int Window::get_width() const {
        return m_width;
    }

    int Window::get_height() const {
        return m_height;
    }

    void Window::show() const {
        if (!SDL_ShowWindow(m_window)) {
            SM_THROW_ERROR(VideoError, "Could not show window: {}", SDL_GetError());
        }
    }

    void Window::set_vsync(bool enable) const {
        if (!SDL_GL_SetSwapInterval(static_cast<int>(enable))) {
            LOG_DIST_ERROR("Could not set swap interval: {}", SDL_GetError());
        }
    }

    void Window::set_icons(std::initializer_list<std::unique_ptr<TextureData>> icons) {
        assert(icons.size() > 0);

        std::vector<SDL_Surface*> surfaces;

        for (const auto& icon : icons) {
            SDL_Surface* surface {SDL_CreateSurfaceFrom(
                icon->get_width(),
                icon->get_height(),
                SDL_PIXELFORMAT_RGBA32,
                icon->get_data(),
                icon->get_width() * 4
            )};

            if (surface == nullptr) {
                SM_THROW_ERROR(VideoError, "Could not create surface: {}", SDL_GetError());
            }

            surfaces.push_back(surface);
        }

        SDL_Surface* surface {surfaces[0]};

        for (auto iter {std::next(surfaces.begin())}; iter != surfaces.end(); iter++) {
            if (!SDL_AddSurfaceAlternateImage(surface, *iter)) {
                SM_THROW_ERROR(VideoError, "Could not add alternate image to surface: {}", SDL_GetError());
            }
        }

        if (!SDL_SetWindowIcon(m_window, surface)) {
            LOG_DIST_ERROR("Could not set window icon: {}", SDL_GetError());
        }

        for (auto iter {std::next(surfaces.begin())}; iter != surfaces.end(); iter++) {
            SDL_DestroySurface(*iter);
        }

        SDL_DestroySurface(surface);
    }

    void Window::set_size(int width, int height) {
        if (!SDL_SetWindowSize(m_window, width, height)) {
            SM_THROW_ERROR(VideoError, "Could not set window size: {}", SDL_GetError());
        }

        if (!SDL_SyncWindow(m_window)) {
            SM_THROW_ERROR(VideoError, "Could not synchronize window: {}", SDL_GetError());
        }

        m_width = width;
        m_height = height;
    }

    double Window::get_time() {
        const Uint64 milliseconds {SDL_GetTicks()};

        return static_cast<double>(milliseconds) / 1000.0;
    }

    void Window::flip() const {
        if (!SDL_GL_SwapWindow(m_window)) {
            SM_THROW_ERROR(VideoError, "Could not swap window buffers: {}", SDL_GetError());
        }
    }

    void Window::poll_events() {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            imgui_context::process_event(&event);

            switch (event.type) {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    m_evt.enqueue<WindowClosedEvent>();

                    break;
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                    m_evt.enqueue<WindowResizedEvent>(event.window.data1, event.window.data2);

                    m_width = event.window.data1;
                    m_height = event.window.data2;

                    break;
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                    m_evt.enqueue<WindowFocusGainedEvent>();

                    break;
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                    m_evt.enqueue<WindowFocusLostEvent>();

                    break;
                case SDL_EVENT_WINDOW_MOUSE_ENTER:
                    m_evt.enqueue<WindowMouseEnteredEvent>();

                    break;
                case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                    m_evt.enqueue<WindowMouseLeftEvent>();

                    break;
                case SDL_EVENT_WINDOW_RESTORED:
                    m_evt.enqueue<WindowRestoredEvent>();

                    break;
                case SDL_EVENT_WINDOW_MINIMIZED:
                    m_evt.enqueue<WindowMinimizedEvent>();

                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (imgui_context::want_capture_keyboard()) {
                        break;
                    }

                    m_evt.enqueue<KeyPressedEvent>(
                        sdl_keycode_to_key(event.key.key),
                        event.key.repeat,
                        static_cast<bool>(event.key.mod & SDL_KMOD_CTRL),
                        static_cast<bool>(event.key.mod & SDL_KMOD_SHIFT),
                        static_cast<bool>(event.key.mod & SDL_KMOD_ALT)
                    );

                    break;
                case SDL_EVENT_KEY_UP:
                    if (imgui_context::want_capture_keyboard()) {
                        break;
                    }

                    m_evt.enqueue<KeyReleasedEvent>(sdl_keycode_to_key(event.key.key));

                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    if (imgui_context::want_capture_mouse()) {
                        break;
                    }

                    m_evt.enqueue<MouseMovedEvent>(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);

                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (imgui_context::want_capture_mouse()) {
                        break;
                    }

                    m_evt.enqueue<MouseButtonPressedEvent>(sdl_button_to_button(event.button.button), event.button.x, event.button.y);

                    break;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (imgui_context::want_capture_mouse()) {
                        break;
                    }

                    m_evt.enqueue<MouseButtonReleasedEvent>(sdl_button_to_button(event.button.button), event.button.x, event.button.y);

                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    if (imgui_context::want_capture_mouse()) {
                        break;
                    }

                    m_evt.enqueue<MouseWheelScrolledEvent>(event.wheel.y);

                    break;
            }
        }
    }
}
