#include "nine_morris_3d_engine/application/internal/window.hpp"

#include <vector>
#include <cstddef>
#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "nine_morris_3d_engine/application/internal/input.hpp"
#include "nine_morris_3d_engine/application/application.hpp"
#include "nine_morris_3d_engine/application/events.hpp"
#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/application/context.hpp"
#include "nine_morris_3d_engine/graphics/internal/imgui_context.hpp"
#include "nine_morris_3d_engine/graphics/opengl/debug.hpp"

namespace sm::internal {
    Window::Window(const ApplicationProperties& properties, EventDispatcher* evt)
        : m_evt(evt) {
        if (!glfwInit()) {
            SM_THROW_ERROR(InitializationError, "Could not initialize GLFW");
        }

        LOG_INFO("Initialized GLFW");

#ifndef SM_BUILD_DISTRIBUTION
        glfwSetErrorCallback([](int error, const char* description) {
            LOG_CRITICAL("({}) GLFW: {}", error, description);
        });
#endif

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, properties.resizable ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

#ifndef SM_BUILD_DISTRIBUTION
        glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
        LOG_INFO("Using OpenGL debug context");
#endif

        create_window(properties);

        LOG_INFO("Created window and OpenGL context");

        glfwMakeContextCurrent(m_window);

        if (!gladLoadGL()) {
            SM_THROW_ERROR(InitializationError, "Could not initialize GLAD");
        }

#ifndef SM_BUILD_DISTRIBUTION
        opengl_debug::initialize();
#endif

        glfwSwapInterval(1);
        glfwSetWindowUserPointer(m_window, this);
        glfwSetWindowSizeLimits(m_window, properties.min_width, properties.min_height, GLFW_DONT_CARE, GLFW_DONT_CARE);

        install_callbacks();

        LOG_INFO("Installed window input callbacks");
    }

    Window::~Window() {
        for (const auto& cursor : m_cursors) {
            glfwDestroyCursor(cursor.second);
        }

        glfwDestroyWindow(m_window);
        glfwTerminate();

        LOG_INFO("Destroyed window and OpenGL context and terminated GLFW");
    }

    int Window::get_width() const noexcept {
        return m_width;
    }

    int Window::get_height() const noexcept {
        return m_height;
    }

    void Window::show() const noexcept {
        glfwShowWindow(m_window);
    }

    void Window::set_vsync(int interval) const noexcept {
        assert(interval >= 0);

        glfwSwapInterval(interval);
    }

    void Window::add_cursor(Id id, std::unique_ptr<TextureData>&& data, int x_hotspot, int y_hotspot) {
        GLFWimage image;
        image.width = data->get_width();
        image.height = data->get_height();
        image.pixels = const_cast<unsigned char*>(data->get_data());  // :P

        GLFWcursor* cursor {glfwCreateCursor(&image, x_hotspot, y_hotspot)};

        if (cursor == nullptr) {
            LOG_DIST_ERROR("Could not create custom cursor");
        }

        m_cursors[id] = cursor;
    }

    void Window::set_cursor(Id id) const {
        static constexpr auto null {Id("null")};

        if (id == null) {
            glfwSetCursor(m_window, nullptr);
            return;
        }

        GLFWcursor* cursor {m_cursors.at(id)};
        glfwSetCursor(m_window, cursor);
    }

    void Window::set_icons(std::initializer_list<std::unique_ptr<TextureData>> datas) const {
        std::vector<GLFWimage> icons;
        icons.reserve(datas.size());

        for (const std::unique_ptr<TextureData>& data : datas) {
            GLFWimage icon;
            icon.width = data->get_width();
            icon.height = data->get_height();
            icon.pixels = const_cast<unsigned char*>(data->get_data());  // :P

            icons.push_back(icon);
        }

        glfwSetWindowIcon(m_window, static_cast<int>(icons.size()), icons.data());
    }

    Monitors Window::get_monitors() const {
        int count {};
        GLFWmonitor** connected_monitors {glfwGetMonitors(&count)};

        if (connected_monitors == nullptr) {
            SM_THROW_ERROR(OtherError, "Could not get monitors");
        }

        Monitors monitors;
        monitors.m_count = static_cast<std::size_t>(count);
        monitors.m_monitors = connected_monitors;

        return monitors;
    }

    double Window::get_time() noexcept {
        const double time {glfwGetTime()};

#ifndef SM_BUILD_DISTRIBUTION
        if (time == 0.0) {
            try {
                LOG_ERROR("Could not get time");
            } catch (...) {}
        }
#endif

        return time;
    }

    GLFWwindow* Window::get_handle() const noexcept {
        return m_window;
    }

    void Window::update() const noexcept {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

    void Window::create_window(const ApplicationProperties& properties) {
        GLFWmonitor* primary_monitor {};

        if (properties.fullscreen) {
            primary_monitor = glfwGetPrimaryMonitor();

            if (primary_monitor == nullptr) {
                SM_THROW_ERROR(InitializationError, "Could not get primary monitor");
            }

            const GLFWvidmode* video_mode {glfwGetVideoMode(primary_monitor)};

            if (video_mode == nullptr) {
                SM_THROW_ERROR(InitializationError, "Could not get monitor video mode");
            }

            if (properties.native_resolution) {
                m_width = video_mode->width;
                m_height = video_mode->height;
            } else {
                if (properties.width > video_mode->width || properties.height > video_mode->height) {
                    SM_THROW_ERROR(InitializationError, "Invalid window width or height");
                }

                m_width = properties.width;
                m_height = properties.height;
            }
        } else {
            m_width = properties.width;
            m_height = properties.height;
        }

        assert(m_width > 0 && m_height > 0);

        m_window = glfwCreateWindow(m_width, m_height, properties.title, primary_monitor, nullptr);

        if (m_window == nullptr) {
            SM_THROW_ERROR(InitializationError, "Could not create window");
        }
    }

    void Window::install_callbacks() const noexcept {
        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
            auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

            win->m_evt->enqueue<WindowClosedEvent>();
        });

        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
            auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

            win->m_evt->enqueue<WindowResizedEvent>(width, height);

            // Sem_t these after firing the event
            win->m_width = width;
            win->m_height = height;
        });

        glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused) {
            auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

            win->m_evt->enqueue<WindowFocusedEvent>(static_cast<bool>(focused));
        });

        glfwSetWindowIconifyCallback(m_window, [](GLFWwindow* window, int iconified) {
            auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

            win->m_evt->enqueue<WindowIconifiedEvent>(static_cast<bool>(iconified));
        });

        glfwSetWindowPosCallback(m_window, [](GLFWwindow* window, int xpos, int ypos) {
            auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

            win->m_evt->enqueue<WindowMovedEvent>(xpos, ypos);
        });

        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

            switch (action) {
                case GLFW_PRESS:
                    if (imgui_context::on_key_pressed(key, scancode)) {
                        return;
                    }

                    win->m_evt->enqueue<KeyPressedEvent>(
                        Input::key_from_code(key),
                        false,
                        static_cast<bool>(mods & GLFW_MOD_CONTROL)
                    );

                    break;
                case GLFW_RELEASE:
                    if (imgui_context::on_key_released(key, scancode)) {
                        return;
                    }

                    win->m_evt->enqueue<KeyReleasedEvent>(Input::key_from_code(key));

                    break;
                case GLFW_REPEAT:
                    if (imgui_context::on_key_pressed(key, scancode)) {
                        return;
                    }

                    win->m_evt->enqueue<KeyPressedEvent>(
                        Input::key_from_code(key),
                        true,
                        static_cast<bool>(mods & GLFW_MOD_CONTROL)
                    );

                    break;
            }
        });

        glfwSetCharCallback(m_window, [](GLFWwindow*, unsigned int codepoint) {
            if (imgui_context::on_char_typed(codepoint)) {
                return;
            }

            // A char typed event is not defined
        });

        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int) {
            auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

            switch (action) {
                case GLFW_PRESS:
                    if (imgui_context::on_mouse_button_pressed(button)) {
                        return;
                    }

                    win->m_evt->enqueue<MouseButtonPressedEvent>(Input::mouse_button_from_code(button));

                    break;
                case GLFW_RELEASE:
                    if (imgui_context::on_mouse_button_released(button)) {
                        return;
                    }

                    win->m_evt->enqueue<MouseButtonReleasedEvent>(Input::mouse_button_from_code(button));

                    break;
            }
        });

        glfwSetScrollCallback(m_window, [](GLFWwindow* window, double, double yoffset) {
            auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

            if (imgui_context::on_mouse_wheel_scrolled(static_cast<float>(yoffset))) {
                return;
            }

            win->m_evt->enqueue<MouseWheelScrolledEvent>(static_cast<float>(yoffset));
        });

        glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
            auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

            if (imgui_context::on_mouse_moved(static_cast<float>(xpos), static_cast<float>(ypos))) {
                return;
            }

            win->m_evt->enqueue<MouseMovedEvent>(static_cast<float>(xpos), static_cast<float>(ypos));
        });
    }
}
