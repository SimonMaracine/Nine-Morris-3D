#include "nine_morris_3d_engine/application/internal/window.hpp"

#include <vector>
#include <string>
#include <cstddef>
#include <cassert>

#include <glad/glad.h>
#include <SDL3/SDL.h>
// #include <GLFW/glfw3.h>

#include "nine_morris_3d_engine/application/application.hpp"
#include "nine_morris_3d_engine/application/events.hpp"
#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/application/context.hpp"
#include "nine_morris_3d_engine/application/input_codes.hpp"
#include "nine_morris_3d_engine/graphics/internal/imgui_context.hpp"
#include "nine_morris_3d_engine/graphics/opengl/debug.hpp"

using namespace std::string_literals;

namespace sm::internal {
    Window::Window(const ApplicationProperties& properties, EventDispatcher& evt)
        : m_evt(evt) {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SM_THROW_ERROR(WindowError, "Could not initialize SDL: "s + SDL_GetError());
        }

        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4)) {
            SM_THROW_ERROR(WindowError, "Could not set SDL_GL_CONTEXT_MAJOR_VERSION attribute: "s + SDL_GetError());
        }

        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3)) {
            SM_THROW_ERROR(WindowError, "Could not set SDL_GL_CONTEXT_MINOR_VERSION attribute: "s + SDL_GetError());
        }

        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)) {
            SM_THROW_ERROR(WindowError, "Could not set SDL_GL_CONTEXT_PROFILE_MASK attribute: "s + SDL_GetError());
        }

        if (!SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)) {
            SM_THROW_ERROR(WindowError, "Could not set SDL_GL_DOUBLEBUFFER attribute: "s + SDL_GetError());
        }

        if (!SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24)) {
            SM_THROW_ERROR(WindowError, "Could not set SDL_GL_DEPTH_SIZE attribute: "s + SDL_GetError());
        }

        if (!SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1)) {
            SM_THROW_ERROR(WindowError, "Could not set SDL_GL_FRAMEBUFFER_SRGB_CAPABLE attribute: "s + SDL_GetError());
        }

        unsigned int flags {SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN};

        if (properties.fullscreen) {
            flags |= SDL_WINDOW_FULLSCREEN;
        }

        m_window = SDL_CreateWindow(properties.title, properties.width, properties.height, flags);

        if (m_window == nullptr) {
            SDL_Quit();
            SM_THROW_ERROR(WindowError, "Could not create window: "s + SDL_GetError());
        }

        LOG_INFO("Initialized SDL and created window");

        m_context = SDL_GL_CreateContext(m_window);

        if (m_context == nullptr) {
            SDL_Quit();
            SM_THROW_ERROR(WindowError, "Could not create OpenGL context: "s + SDL_GetError());
        }

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
            SDL_Quit();
            SM_THROW_ERROR(WindowError, "Could not initialize GLAD");
        }

        // FIXME don't panic
        if (!SDL_GL_SetSwapInterval(1)) {
            SDL_Quit();
            SM_THROW_ERROR(WindowError, "Could not set swap interval: "s + SDL_GetError());
        }

        if (!SDL_SetWindowMinimumSize(m_window, properties.min_width, properties.min_height)) {
            SDL_Quit();
            SM_THROW_ERROR(WindowError, "Could not set minimum window size: "s + SDL_GetError());
        }

#ifndef SM_BUILD_DISTRIBUTION
        opengl_debug::initialize();
#endif

//         if (!glfwInit()) {
//             SM_THROW_ERROR(InitializationError, "Could not initialize GLFW");
//         }

//         LOG_INFO("Initialized GLFW");

// #ifndef SM_BUILD_DISTRIBUTION
//         glfwSetErrorCallback([](int error, const char* description) {
//             LOG_CRITICAL("({}) GLFW: {}", error, description);
//         });
// #endif

//         glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//         glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//         glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//         glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
//         glfwWindowHint(GLFW_RESIZABLE, properties.resizable ? GLFW_TRUE : GLFW_FALSE);
//         glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
//         glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

// #ifndef SM_BUILD_DISTRIBUTION
//         glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
//         LOG_INFO("Using OpenGL debug context");
// #endif

//         create_window(properties);

//         LOG_INFO("Created window and OpenGL context");

//         glfwMakeContextCurrent(m_window);

//         if (!gladLoadGL()) {
//             SM_THROW_ERROR(InitializationError, "Could not initialize GLAD");
//         }

// #ifndef SM_BUILD_DISTRIBUTION
//         opengl_debug::initialize();
// #endif

//         glfwSwapInterval(1);
//         glfwSetWindowUserPointer(m_window, this);
//         glfwSetWindowSizeLimits(m_window, properties.min_width, properties.min_height, GLFW_DONT_CARE, GLFW_DONT_CARE);

//         install_callbacks();

//         LOG_INFO("Installed window input callbacks");
    }

    Window::~Window() {
        SDL_GL_DestroyContext(static_cast<SDL_GLContext>(m_context));
        SDL_DestroyWindow(m_window);
        SDL_Quit();

        // for (const auto& cursor : m_cursors) {
        //     glfwDestroyCursor(cursor.second);
        // }

        // glfwDestroyWindow(m_window);
        // glfwTerminate();

        LOG_INFO("Destroyed OpenGL context and window and terminated SDL");
    }

    int Window::get_width() const noexcept {
        return m_width;
    }

    int Window::get_height() const noexcept {
        return m_height;
    }

    void Window::show() const {
        // glfwShowWindow(m_window);

        if (!SDL_ShowWindow(m_window)) {
            SM_THROW_ERROR(WindowError, "Could not show window: "s + SDL_GetError());
        }
    }

    void Window::set_vsync(bool enable) const {
        // FIXME don't panic
        if (!SDL_GL_SetSwapInterval(static_cast<int>(enable))) {
            SM_THROW_ERROR(WindowError, "Could not set swap interval: "s + SDL_GetError());
        }

        // glfwSwapInterval(enable ? 1 : 0);
    }

    // void Window::add_cursor(Id id, std::unique_ptr<TextureData>&& data, int x_hotspot, int y_hotspot) {
    //     GLFWimage image;
    //     image.width = data->get_width();
    //     image.height = data->get_height();
    //     image.pixels = const_cast<unsigned char*>(data->get_data());  // :P

    //     GLFWcursor* cursor {glfwCreateCursor(&image, x_hotspot, y_hotspot)};

    //     if (cursor == nullptr) {
    //         LOG_DIST_ERROR("Could not create custom cursor");
    //     }

    //     m_cursors[id] = cursor;
    // }

    // void Window::set_cursor(Id id) const {
    //     static constexpr auto null {Id("null")};

    //     if (id == null) {
    //         glfwSetCursor(m_window, nullptr);
    //         return;
    //     }

    //     GLFWcursor* cursor {m_cursors.at(id)};
    //     glfwSetCursor(m_window, cursor);
    // }

    void Window::set_icons(std::initializer_list<std::unique_ptr<TextureData>> datas) const {
        // std::vector<GLFWimage> icons;
        // icons.reserve(datas.size());

        // for (const std::unique_ptr<TextureData>& data : datas) {
        //     GLFWimage icon;
        //     icon.width = data->get_width();
        //     icon.height = data->get_height();
        //     icon.pixels = const_cast<unsigned char*>(data->get_data());  // :P

        //     icons.push_back(icon);
        // }

        // glfwSetWindowIcon(m_window, static_cast<int>(icons.size()), icons.data());
    }

    void Window::set_size(int width, int height) {
        if (!SDL_SetWindowSize(m_window, width, height)) {
            SM_THROW_ERROR(WindowError, "Could not set window size: "s + SDL_GetError());
        }

        if (!SDL_SyncWindow(m_window)) {
            SM_THROW_ERROR(WindowError, "Could not synchronize window: "s + SDL_GetError());
        }

        m_width = width;
        m_height = height;

        // glfwSetWindowSize(m_window, width, height);

        // m_width = width;
        // m_height = height;
    }

    // Monitors Window::get_monitors() const {
    //     int count {};
    //     GLFWmonitor** connected_monitors {glfwGetMonitors(&count)};

    //     if (connected_monitors == nullptr) {
    //         SM_THROW_ERROR(OtherError, "Could not get monitors");
    //     }

    //     Monitors monitors;
    //     monitors.m_count = static_cast<std::size_t>(count);
    //     monitors.m_monitors = connected_monitors;

    //     return monitors;
    // }

    double Window::get_time() noexcept {
        const Uint64 milliseconds {SDL_GetTicks()};

        const Uint64 seconds {milliseconds / 1000};
        const Uint64 remainder {milliseconds % 1000};

        return static_cast<double>(seconds) + static_cast<double>(remainder) / 1000.0;

//         const double time {glfwGetTime()};

// #ifndef SM_BUILD_DISTRIBUTION
//         if (time == 0.0) {
//             try {
//                 LOG_ERROR("Could not get time");
//             } catch (...) {}
//         }
// #endif

//         return time;
    }

    // GLFWwindow* Window::get_handle() const noexcept {
    //     return m_window;
    // }

    void Window::flip() const {
        if (!SDL_GL_SwapWindow(m_window)) {
            SM_THROW_ERROR(WindowError, "Could not swap window buffers: "s + SDL_GetError());
        }

        // glfwPollEvents();
        // glfwSwapBuffers(m_window);
    }

    // if (imgui_context::on_char_typed(codepoint)) {
    //     return;
    // }

    void Window::poll_events() {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    // application->events.enqueue<WindowClosedEvent>();
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
                case SDL_EVENT_WINDOW_MAXIMIZED:
                    m_evt.enqueue<WindowMaximizedEvent>();

                    break;
                case SDL_EVENT_WINDOW_MINIMIZED:
                    m_evt.enqueue<WindowMinimizedEvent>();

                    break;
                case SDL_EVENT_KEY_DOWN:
                    // application->events.enqueue<KeyPressedEvent>(
                    //     static_cast<KeyCode>(event.key.keysym.sym),
                    //     static_cast<bool>(event.key.repeat)
                    // );

                    if (imgui_context::on_key_pressed(event.key.key, event.key.scancode)) {
                        return;
                    }

                    m_evt.enqueue<KeyPressedEvent>(
                        sdl_keycode_to_key(event.key.key),
                        event.key.repeat,
                        event.key.mod & SDL_KMOD_CTRL,
                        event.key.mod & SDL_KMOD_SHIFT,
                        event.key.mod & SDL_KMOD_ALT
                    );

                    break;
                case SDL_EVENT_KEY_UP:
                    if (imgui_context::on_key_released(event.key.key, event.key.scancode)) {
                        return;
                    }

                    m_evt.enqueue<KeyReleasedEvent>(sdl_keycode_to_key(event.key.key));

                    // application->events.enqueue<KeyReleasedEvent>(static_cast<KeyCode>(event.key.keysym.sym));

                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    // application->events.enqueue<MouseMovedEvent>(
                    //     event.motion.state,
                    //     event.motion.x,
                    //     event.motion.y,
                    //     event.motion.xrel,
                    //     event.motion.yrel
                    // );

                    if (imgui_context::on_mouse_moved(event.motion.x, event.motion.y)) {
                        return;
                    }

                    m_evt.enqueue<MouseMovedEvent>(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);

                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    // application->events.enqueue<MouseButtonPressedEvent>(
                    //     event.button.button,
                    //     event.button.x,
                    //     event.button.y
                    // );

                    if (imgui_context::on_mouse_button_pressed(event.button.button)) {
                        return;
                    }

                    m_evt.enqueue<MouseButtonPressedEvent>(sdl_button_to_button(event.button.button), event.button.x, event.button.y);

                    break;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    // application->events.enqueue<MouseButtonReleasedEvent>(
                    //     event.button.button,
                    //     event.button.x,
                    //     event.button.y
                    // );

                    if (imgui_context::on_mouse_button_released(event.button.button)) {
                        return;
                    }

                    m_evt.enqueue<MouseButtonReleasedEvent>(sdl_button_to_button(event.button.button), event.button.x, event.button.y);

                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    // application->events.enqueue<MouseWheelScrolledEvent>(event.wheel.y);

                    if (imgui_context::on_mouse_wheel_scrolled(event.wheel.y)) {
                        return;
                    }

                    m_evt.enqueue<MouseWheelScrolledEvent>(event.wheel.y);

                    break;
            }
        }
    }

    // void Window::create_window(const ApplicationProperties& properties) {
    //     GLFWmonitor* primary_monitor {};

    //     if (properties.fullscreen) {
    //         primary_monitor = glfwGetPrimaryMonitor();

    //         if (primary_monitor == nullptr) {
    //             SM_THROW_ERROR(InitializationError, "Could not get primary monitor");
    //         }

    //         const GLFWvidmode* video_mode {glfwGetVideoMode(primary_monitor)};

    //         if (video_mode == nullptr) {
    //             SM_THROW_ERROR(InitializationError, "Could not get monitor video mode");
    //         }

    //         if (properties.native_resolution) {
    //             m_width = video_mode->width;
    //             m_height = video_mode->height;
    //         } else {
    //             if (properties.width > video_mode->width || properties.height > video_mode->height) {
    //                 SM_THROW_ERROR(InitializationError, "Invalid window width or height");
    //             }

    //             m_width = properties.width;
    //             m_height = properties.height;
    //         }
    //     } else {
    //         m_width = properties.width;
    //         m_height = properties.height;
    //     }

    //     assert(m_width > 0 && m_height > 0);

    //     m_window = glfwCreateWindow(m_width, m_height, properties.title, primary_monitor, nullptr);

    //     if (m_window == nullptr) {
    //         SM_THROW_ERROR(InitializationError, "Could not create window");
    //     }
    // }

    // void Window::install_callbacks() const noexcept {
    //     glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
    //         auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

    //         win->m_evt.enqueue<WindowClosedEvent>();
    //     });

    //     glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
    //         auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

    //         win->m_evt.enqueue<WindowResizedEvent>(width, height);

    //         // Sem_t these after firing the event
    //         win->m_width = width;
    //         win->m_height = height;
    //     });

    //     glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused) {
    //         auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

    //         win->m_evt.enqueue<WindowFocusedEvent>(static_cast<bool>(focused));
    //     });

    //     glfwSetWindowIconifyCallback(m_window, [](GLFWwindow* window, int iconified) {
    //         auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

    //         win->m_evt.enqueue<WindowIconifiedEvent>(static_cast<bool>(iconified));
    //     });

    //     glfwSetWindowPosCallback(m_window, [](GLFWwindow* window, int xpos, int ypos) {
    //         auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

    //         win->m_evt.enqueue<WindowMovedEvent>(xpos, ypos);
    //     });

    //     glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    //         auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

    //         switch (action) {
    //             case GLFW_PRESS:
    //                 if (imgui_context::on_key_pressed(key, scancode)) {
    //                     return;
    //                 }

    //                 win->m_evt.enqueue<KeyPressedEvent>(
    //                     Input::key_from_code(key),
    //                     false,
    //                     static_cast<bool>(mods & GLFW_MOD_CONTROL)
    //                 );

    //                 break;
    //             case GLFW_RELEASE:
    //                 if (imgui_context::on_key_released(key, scancode)) {
    //                     return;
    //                 }

    //                 win->m_evt.enqueue<KeyReleasedEvent>(Input::key_from_code(key));

    //                 break;
    //             case GLFW_REPEAT:
    //                 if (imgui_context::on_key_pressed(key, scancode)) {
    //                     return;
    //                 }

    //                 win->m_evt.enqueue<KeyPressedEvent>(
    //                     Input::key_from_code(key),
    //                     true,
    //                     static_cast<bool>(mods & GLFW_MOD_CONTROL)
    //                 );

    //                 break;
    //         }
    //     });

    //     glfwSetCharCallback(m_window, [](GLFWwindow*, unsigned int codepoint) {
    //         if (imgui_context::on_char_typed(codepoint)) {
    //             return;
    //         }

    //         // A char typed event is not defined
    //     });

    //     glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int) {
    //         auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

    //         switch (action) {
    //             case GLFW_PRESS:
    //                 if (imgui_context::on_mouse_button_pressed(button)) {
    //                     return;
    //                 }

    //                 win->m_evt.enqueue<MouseButtonPressedEvent>(Input::mouse_button_from_code(button));

    //                 break;
    //             case GLFW_RELEASE:
    //                 if (imgui_context::on_mouse_button_released(button)) {
    //                     return;
    //                 }

    //                 win->m_evt.enqueue<MouseButtonReleasedEvent>(Input::mouse_button_from_code(button));

    //                 break;
    //         }
    //     });

    //     glfwSetScrollCallback(m_window, [](GLFWwindow* window, double, double yoffset) {
    //         auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

    //         if (imgui_context::on_mouse_wheel_scrolled(static_cast<float>(yoffset))) {
    //             return;
    //         }

    //         win->m_evt.enqueue<MouseWheelScrolledEvent>(static_cast<float>(yoffset));
    //     });

    //     glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
    //         auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

    //         if (imgui_context::on_mouse_moved(static_cast<float>(xpos), static_cast<float>(ypos))) {
    //             return;
    //         }

    //         win->m_evt.enqueue<MouseMovedEvent>(static_cast<float>(xpos), static_cast<float>(ypos));
    //     });
    // }
}
