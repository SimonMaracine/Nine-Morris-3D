#include "nine_morris_3d_engine/application/window.hpp"

#include <vector>
#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "nine_morris_3d_engine/application/application.hpp"
#include "nine_morris_3d_engine/application/events.hpp"
#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/input.hpp"
#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/application/context.hpp"
#include "nine_morris_3d_engine/graphics/opengl/debug.hpp"
#include "nine_morris_3d_engine/graphics/imgui_context.hpp"

namespace sm {
    std::pair<int, int> Monitors::get_resolution(std::size_t index) const {
        assert(index < count);

        const GLFWvidmode* video_mode {glfwGetVideoMode(monitors[index])};

        return std::make_pair(video_mode->width, video_mode->height);
    }

    std::pair<float, float> Monitors::get_content_scale(std::size_t index) const {
        assert(index < count);

        float xscale, yscale;
        glfwGetMonitorContentScale(monitors[index], &xscale, &yscale);

        return std::make_pair(xscale, yscale);
    }

    const char* Monitors::get_name(std::size_t index) const {  // TODO used?
        assert(index < count);

        const char* name {glfwGetMonitorName(monitors[index])};

        if (name == nullptr) {
            SM_THROW_ERROR(OtherError, "Could not retrieve monitor name");
        }

        return name;
    }

    namespace internal {
        Window::Window(const ApplicationProperties& properties, EventDispatcher* evt)
            : evt(evt) {
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
            glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
            glfwWindowHint(GLFW_RESIZABLE, properties.resizable ? GLFW_TRUE : GLFW_FALSE);
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    #ifndef SM_BUILD_DISTRIBUTION
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
            LOG_INFO("Using OpenGL debug context");
    #else
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
    #endif

            window = create_window(properties);

            if (window == nullptr) {
                SM_THROW_ERROR(InitializationError, "Could not create window");
            }

            LOG_INFO("Created window and OpenGL context");

            glfwMakeContextCurrent(window);

            if (!gladLoadGL()) {
                SM_THROW_ERROR(InitializationError, "Could not initialize GLAD");
            }

    #ifndef SM_BUILD_DISTRIBUTION
            opengl_debug::initialize();
    #endif

            glfwSwapInterval(1);
            glfwSetWindowUserPointer(window, this);
            glfwSetWindowSizeLimits(
                window,
                properties.min_width,
                properties.min_height,
                GLFW_DONT_CARE,
                GLFW_DONT_CARE
            );

            install_callbacks();

            LOG_INFO("Installed window input callbacks");
        }

        Window::~Window() {
            for (const auto& cursor : cursors) {
                glfwDestroyCursor(cursor.second);
            }

            glfwDestroyWindow(window);
            glfwTerminate();

            LOG_INFO("Destroyed window and OpenGL context and terminated GLFW");
        }

        void Window::update() const {
            glfwPollEvents();
            glfwSwapBuffers(window);
        }

        const Monitors& Window::get_monitors() {
            int count {};
            GLFWmonitor** connected_monitors {glfwGetMonitors(&count)};

            if (connected_monitors == nullptr) {
                SM_THROW_ERROR(OtherError, "Could not retrieve monitors");
            }

            monitors.count = static_cast<std::size_t>(count);
            monitors.monitors = connected_monitors;

            return monitors;
        }

        int Window::get_width() const {
            return width;
        }

        int Window::get_height() const {
            return height;
        }

        void Window::show() const {
            glfwShowWindow(window);
        }

        void Window::set_vsync(int interval) const {
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

            cursors[id] = cursor;
        }

        void Window::set_cursor(Id id) const {
            static constexpr auto null {resmanager::HashedStr64("null")};

            if (id == null) {
                glfwSetCursor(window, nullptr);
                return;
            }

            GLFWcursor* cursor {cursors.at(id)};
            glfwSetCursor(window, cursor);
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

            glfwSetWindowIcon(window, static_cast<int>(icons.size()), icons.data());
        }

        double Window::get_time() {
            return glfwGetTime();  // FIXME this can return 0.0 on error
        }

        GLFWwindow* Window::get_handle() const {
            return window;
        }

        GLFWwindow* Window::create_window(const ApplicationProperties& properties) {
            GLFWmonitor* primary_monitor {nullptr};

            if (properties.fullscreen) {
                primary_monitor = glfwGetPrimaryMonitor();

                if (properties.native_resolution) {
                    const GLFWvidmode* video_mode {glfwGetVideoMode(primary_monitor)};

                    width = video_mode->width;
                    height = video_mode->height;
                } else {
                    width = properties.width;  // FIXME maybe this could be larger than monitor's native resolution, which would crash the game
                    height = properties.height;
                }
            } else {
                width = properties.width;
                height = properties.height;
            }

            assert(width > 0 && height > 0);

            return glfwCreateWindow(width, height, properties.title.c_str(), primary_monitor, nullptr);
        }

        void Window::install_callbacks() const {
            glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
                auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

                win->evt->enqueue<WindowClosedEvent>();
            });

            glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
                auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

                win->evt->enqueue<WindowResizedEvent>(width, height);

                // Set these after firing the event
                win->width = width;
                win->height = height;
            });

            glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focused) {
                auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

                win->evt->enqueue<WindowFocusedEvent>(static_cast<bool>(focused));
            });

            glfwSetWindowIconifyCallback(window, [](GLFWwindow* window, int iconified) {
                auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

                win->evt->enqueue<WindowIconifiedEvent>(static_cast<bool>(iconified));
            });

            glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

                switch (action) {
                    case GLFW_PRESS:
                        if (imgui_context::on_key_pressed(key, scancode)) {
                            return;
                        }

                        win->evt->enqueue<KeyPressedEvent>(
                            Input::key_from_code(key),
                            false,
                            static_cast<bool>(mods & GLFW_MOD_CONTROL)
                        );

                        break;
                    case GLFW_RELEASE:
                        if (imgui_context::on_key_released(key, scancode)) {
                            return;
                        }

                        win->evt->enqueue<KeyReleasedEvent>(Input::key_from_code(key));

                        break;
                    case GLFW_REPEAT:
                        if (imgui_context::on_key_pressed(key, scancode)) {
                            return;
                        }

                        win->evt->enqueue<KeyPressedEvent>(
                            Input::key_from_code(key),
                            true,
                            static_cast<bool>(mods & GLFW_MOD_CONTROL)
                        );

                        break;
                }
            });

            glfwSetCharCallback(window, [](GLFWwindow*, unsigned int codepoint) {
                if (imgui_context::on_char_typed(codepoint)) {
                    return;
                }

                // A char typed event is not defined
            });

            glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int) {
                auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

                switch (action) {
                    case GLFW_PRESS:
                        if (imgui_context::on_mouse_button_pressed(button)) {
                            return;
                        }

                        win->evt->enqueue<MouseButtonPressedEvent>(Input::mouse_button_from_code(button));

                        break;
                    case GLFW_RELEASE:
                        if (imgui_context::on_mouse_button_released(button)) {
                            return;
                        }

                        win->evt->enqueue<MouseButtonReleasedEvent>(Input::mouse_button_from_code(button));

                        break;
                }
            });

            glfwSetScrollCallback(window, [](GLFWwindow* window, double, double yoffset) {
                auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

                if (imgui_context::on_mouse_wheel_scrolled(static_cast<float>(yoffset))) {
                    return;
                }

                win->evt->enqueue<MouseWheelScrolledEvent>(static_cast<float>(yoffset));
            });

            glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
                auto* win {static_cast<Window*>(glfwGetWindowUserPointer(window))};

                if (imgui_context::on_mouse_moved(static_cast<float>(xpos), static_cast<float>(ypos))) {
                    return;
                }

                win->evt->enqueue<MouseMovedEvent>(static_cast<float>(xpos), static_cast<float>(ypos));
            });
        }
    }
}
