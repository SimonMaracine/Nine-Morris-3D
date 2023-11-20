#include <memory>
#include <initializer_list>
#include <utility>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <resmanager/resmanager.hpp>

#include "engine/application_base/window.hpp"
#include "engine/application_base/application.hpp"
#include "engine/application_base/application_properties.hpp"
#include "engine/application_base/events.hpp"
#include "engine/application_base/platform.hpp"
#include "engine/application_base/input.hpp"
#include "engine/application_base/panic.hpp"
#include "engine/graphics/imgui_context.hpp"
#include "engine/graphics/texture_data.hpp"
#include "engine/other/logging.hpp"

#define APPLICATION_DATA(VARIABLE) \
    const ApplicationProperties* VARIABLE { \
        static_cast<ApplicationProperties*>(glfwGetWindowUserPointer(window)) \
    };

namespace sm {
    Window::Window(Application* application) {
        if (!glfwInit()) {
            LOG_DIST_CRITICAL("Could not initialize GLFW");
            throw InitializationError;
        }

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
        glfwWindowHint(GLFW_RESIZABLE, application->properties.resizable ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#ifndef SM_BUILD_DISTRIBUTION
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        LOG_INFO("Using OpenGL debug context");
#else
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

        window = create_window(application);

        if (window == nullptr) {
            LOG_DIST_CRITICAL("Could not create window");
            throw InitializationError;
        }

        LOG_INFO("Initialized GLFW and created window and OpenGL context");

        glfwMakeContextCurrent(window);

        if (!gladLoadGL()) {
            LOG_DIST_CRITICAL("Could not initialize GLAD");
            throw InitializationError;
        }

        glfwSwapInterval(1);
        glfwSetWindowUserPointer(window, &application->properties);
        glfwSetWindowSizeLimits(
            window,
            application->properties.min_width,
            application->properties.min_height,
            GLFW_DONT_CARE,
            GLFW_DONT_CARE
        );

        install_callbacks();

        LOG_INFO("Created window and installed input callbacks");
    }

    Window::~Window() {
        for (const auto& cursor : cursors) {
            glfwDestroyCursor(cursor.second);
        }

        glfwDestroyWindow(window);
        glfwTerminate();

        LOG_INFO("Destroyed window and OpenGL context and terminated GLFW");
    }

    void Window::update() {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    GLFWwindow* Window::get_handle() {
        return window;
    }

    std::vector<Monitor> Window::get_monitors() {
        int count;
        GLFWmonitor** monitors {glfwGetMonitors(&count)};

        if (monitors == nullptr) {
            LOG_DIST_CRITICAL("Could not retrieve monitors");
            throw OtherError;
        }

        std::vector<Monitor> result;

        for (int i {0}; i < count; i++) {
            Monitor monitor;
            monitor.monitor = monitors[i];

            result.push_back(monitor);
        }

        return result;
    }

    void Window::show() {
        glfwShowWindow(window);
    }

    void Window::set_vsync(int interval) {
        SM_ASSERT(interval >= 0, "Invalid interval");

        glfwSwapInterval(interval);
    }

    void Window::add_cursor(CursorId id, std::unique_ptr<TextureData>&& cursor, int x_hotspot, int y_hotspot) {
        const TextureData::Image data {cursor->get_data()};

        GLFWimage image {
            data.width,
            data.height,
            data.pixels
        };

        GLFWcursor* glfw_cursor {glfwCreateCursor(&image, x_hotspot, y_hotspot)};

        if (glfw_cursor == nullptr) {
            LOG_DIST_ERROR("Could not create custom cursor `{}`", cursor->get_file_path());
        }

        cursors[id] = glfw_cursor;
    }

    void Window::set_cursor(CursorId id) {
        static constexpr auto null {resmanager::HashedStr64("null")};

        if (id == null) {
            glfwSetCursor(window, nullptr);
            return;
        }

        GLFWcursor* cursor {cursors.at(id)};
        glfwSetCursor(window, cursor);
    }

    void Window::set_icons(std::initializer_list<std::unique_ptr<TextureData>> icons) {
        std::vector<GLFWimage> glfw_icons;
        glfw_icons.reserve(icons.size());

        for (const std::unique_ptr<TextureData>& icon : icons) {
            const TextureData::Image data {icon->get_data()};

            const GLFWimage image {
                data.width,
                data.height,
                data.pixels
            };

            glfw_icons.push_back(image);
        }

        glfwSetWindowIcon(window, glfw_icons.size(), glfw_icons.data());
    }

    double Window::get_time() {
        return glfwGetTime();
    }

    GLFWwindow* Window::create_window(Application* application) {
        GLFWmonitor* primary_monitor {nullptr};
        int width {0};
        int height {0};

        if (application->properties.fullscreen) {
            primary_monitor = glfwGetPrimaryMonitor();

            if (application->properties.native_resolution) {
                const GLFWvidmode* video_mode {glfwGetVideoMode(primary_monitor)};

                width = video_mode->width;
                height = video_mode->height;

                // Set the real resolution
                application->properties.width = video_mode->width;
                application->properties.height = video_mode->height;
            } else {
                width = application->properties.width;  // FIXME maybe this could be larger than monitor's native resolution, which would crash the game
                height = application->properties.height;
            }
        } else {
            width = application->properties.width;
            height = application->properties.height;
        }

        return glfwCreateWindow(width, height, application->properties.title.c_str(), primary_monitor, nullptr);
    }

    void Window::install_callbacks() {
        glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
            APPLICATION_DATA(data)

            data->ctx->evt.enqueue<WindowClosedEvent>();
        });

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
            ApplicationProperties* data {static_cast<ApplicationProperties*>(glfwGetWindowUserPointer(window))};

            data->width = width;
            data->height = height;

            data->ctx->evt.enqueue<WindowResizedEvent>(width, height);
        });

        glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focused) {
            APPLICATION_DATA(data)

            data->ctx->evt.enqueue<WindowFocusedEvent>(static_cast<bool>(focused));
        });

        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            APPLICATION_DATA(data)

            switch (action) {
                case GLFW_PRESS: {
                    if (ImGuiContext::on_key_pressed(key, scancode)) {
                        return;
                    }

                    data->ctx->evt.enqueue<KeyPressedEvent>(
                        Input::key_from_code(key),
                        false,
                        static_cast<bool>(mods & GLFW_MOD_CONTROL)
                    );
                    break;
                }
                case GLFW_RELEASE: {
                    if (ImGuiContext::on_key_released(key, scancode)) {
                        return;
                    }

                    data->ctx->evt.enqueue<KeyReleasedEvent>(Input::key_from_code(key));
                    break;
                }
                case GLFW_REPEAT: {
                    if (ImGuiContext::on_key_pressed(key, scancode)) {
                        return;
                    }

                    data->ctx->evt.enqueue<KeyPressedEvent>(
                        Input::key_from_code(key),
                        true,
                        static_cast<bool>(mods & GLFW_MOD_CONTROL)
                    );
                    break;
                }
            }
        });

        glfwSetCharCallback(window, [](GLFWwindow*, unsigned int codepoint) {
            if (ImGuiContext::on_char_typed(codepoint)) {
                return;
            }

            // A char typed event is not defined
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int) {
            APPLICATION_DATA(data)

            switch (action) {
                case GLFW_PRESS: {
                    if (ImGuiContext::on_mouse_button_pressed(button)) {
                        return;
                    }

                    data->ctx->evt.enqueue<MouseButtonPressedEvent>(Input::mouse_button_from_code(button));
                    break;
                }
                case GLFW_RELEASE: {
                    if (ImGuiContext::on_mouse_button_released(button)) {
                        return;
                    }

                    data->ctx->evt.enqueue<MouseButtonReleasedEvent>(Input::mouse_button_from_code(button));
                    break;
                }
            }
        });

        glfwSetScrollCallback(window, [](GLFWwindow* window, double, double yoffset) {
            APPLICATION_DATA(data)

            if (ImGuiContext::on_mouse_scrolled(static_cast<float>(yoffset))) {
                return;
            }

            data->ctx->evt.enqueue<MouseScrolledEvent>(static_cast<float>(yoffset));
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
            APPLICATION_DATA(data)

            if (ImGuiContext::on_mouse_moved(static_cast<float>(xpos), static_cast<float>(ypos))) {
                return;
            }

            data->ctx->evt.enqueue<MouseMovedEvent>(static_cast<float>(xpos), static_cast<float>(ypos));
        });
    }

    std::pair<int, int> Monitor::get_resolution() const {
        const GLFWvidmode* video_mode {glfwGetVideoMode(monitor)};

        return std::make_pair(video_mode->width, video_mode->height);
    }

    std::pair<float, float> Monitor::get_content_scale() const {
        float xscale, yscale;
        glfwGetMonitorContentScale(monitor, &xscale, &yscale);

        return std::make_pair(xscale, yscale);
    }

    const char* Monitor::get_name() const {  // TODO used?
        const char* name {glfwGetMonitorName(monitor)};

        if (name == nullptr) {
            LOG_DIST_CRITICAL("Could not retrieve monitor name");
            throw OtherError;
        }

        return name;
    }
}
