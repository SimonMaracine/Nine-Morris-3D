#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "engine/application/window.h"
#include "engine/application/application.h"
#include "engine/application/application_data.h"
#include "engine/application/event.h"
#include "engine/application/platform.h"
#include "engine/application/input.h"
#include "engine/graphics/texture_data.h"
#include "engine/other/logging.h"
#include "engine/other/exit.h"

Window::Window(Application* app) {
    if (!glfwInit()) {
        REL_CRITICAL("Could not initialize GLFW, exiting...");
        application_exit::panic();
    }

#ifdef NM3D_PLATFORM_DEBUG
    glfwSetErrorCallback([](int error, const char* description) {
        DEB_CRITICAL("({}) GLFW Error Callback: {}", error, description);
    });
#endif

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, app->data().resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#ifdef NM3D_PLATFORM_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    DEB_INFO("Using OpenGL debug context");
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

    window = create_window(app);

    if (window == nullptr) {
        REL_CRITICAL("Could not create window, exiting...");
        application_exit::panic();
    }

    DEB_INFO("Initialized GLFW and created window and OpenGL context");

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        REL_CRITICAL("Could not initialize GLAD, exiting...");
        application_exit::panic();
    }

    glfwSwapInterval(1);
    glfwSetWindowUserPointer(window, &app->app_data);
    glfwSetWindowSizeLimits(window, app->data().min_width, app->data().min_height, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        event::WindowClosedEvent event;
        data->on_event(event);
        // data->app->evt.enqueue<WindowClosedEvent>();
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        data->width = width;
        data->height = height;

        event::WindowResizedEvent event;
        event.width = width;
        event.height = height;
        data->on_event(event);
        // data->app->evt.enqueue<WindowResizedEvent>(width, height);
    });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int, int action, int mods) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        switch (action) {
            case GLFW_PRESS: {
                event::KeyPressedEvent event;
                event.key = static_cast<input::Key>(key);
                event.repeat = false;
                event.control = static_cast<bool>(mods & GLFW_MOD_CONTROL);
                data->on_event(event);
                // data->app->evt.enqueue<KeyPressedEvent>(
                //     static_cast<input::Key>(key), false, static_cast<bool>(mods & GLFW_MOD_CONTROL)
                // );
                break;
            }
            case GLFW_RELEASE: {
                event::KeyReleasedEvent event;
                event.key = static_cast<input::Key>(key);
                data->on_event(event);
                // data->app->evt.enqueue<KeyReleasedEvent>(static_cast<input::Key>(key));
                break;
            }
            case GLFW_REPEAT: {
                event::KeyPressedEvent event;
                event.key = static_cast<input::Key>(key);
                event.repeat = true;
                event.control = static_cast<bool>(mods & GLFW_MOD_CONTROL);
                data->on_event(event);
                // data->app->evt.enqueue<KeyPressedEvent>(
                //     static_cast<input::Key>(key), true, static_cast<bool>(mods & GLFW_MOD_CONTROL)
                // );
                break;
            }
        }
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        switch (action) {
            case GLFW_PRESS: {
                event::MouseButtonPressedEvent event;
                event.button = static_cast<input::MouseButton>(button);
                data->on_event(event);
                // data->app->evt.enqueue<MouseButtonPressedEvent>(
                //     static_cast<input::MouseButton>(button)
                // );
                break;
            }
            case GLFW_RELEASE: {
                event::MouseButtonReleasedEvent event;
                event.button = static_cast<input::MouseButton>(button);
                data->on_event(event);
                // data->app->evt.enqueue<MouseButtonReleasedEvent>(
                //     static_cast<input::MouseButton>(button)
                // );
                break;
            }
        }
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double, double yoffset) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        event::MouseScrolledEvent event;
        event.scroll = static_cast<float>(yoffset);
        data->on_event(event);
        // data->app->evt.enqueue<MouseScrolledEvent>(static_cast<float>(yoffset));
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        event::MouseMovedEvent event;
        event.mouse_x = static_cast<float>(xpos);
        event.mouse_y = static_cast<float>(ypos);
        data->on_event(event);
        // data->app->evt.enqueue<MouseMovedEvent>(
        //     static_cast<float>(xpos), static_cast<float>(ypos)
        // );
    });
}

Window::~Window() {
    for (const auto& cursor : cursors) {
        glfwDestroyCursor(cursor.second);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    DEB_INFO("Destroyed window and OpenGL context and terminated GLFW");
}

void Window::update() {
    glfwPollEvents();
    glfwSwapBuffers(window);
}

GLFWwindow* Window::get_handle() {
    return window;
}

double Window::get_time() {
    return glfwGetTime();
}

std::vector<Monitor> Window::get_monitors() {
    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);

    if (monitors == nullptr) {
        REL_CRITICAL("Could not retrieve monitors, exiting...");
        application_exit::panic();
    }

    std::vector<Monitor> result;

    for (int i = 0; i < count; i++) {
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
    glfwSwapInterval(interval);
}

unsigned int Window::add_cursor(std::unique_ptr<TextureData>&& cursor, int x_hotspot, int y_hotspot) {
    const TextureData::Image data = cursor->get_data();

    GLFWimage image = {
        data.width,
        data.height,
        data.pixels
    };

    GLFWcursor* glfw_cursor = glfwCreateCursor(&image, x_hotspot, y_hotspot);

    if (glfw_cursor == nullptr) {
        REL_ERROR("Could not create custom cursor `{}`", cursor->get_file_path());
    }

    static unsigned int id = 0;

    cursors[++id] = glfw_cursor;
    return id;
}

void Window::set_cursor(unsigned int handle) {
    if (handle == 0) {
        glfwSetCursor(window, nullptr);
        return;
    }

#if defined(NM3D_PLATFORM_DEBUG)
    GLFWcursor* cursor;
    try {
        cursor = cursors.at(handle);
    } catch (const std::out_of_range&) {
        DEB_CRITICAL("Invalid handle `{}`, exiting...", handle);
        application_exit::panic();
    }
    glfwSetCursor(window, cursor);
#elif defined(NM3D_PLATFORM_RELEASE)
    GLFWcursor* cursor = cursors[handle];
    glfwSetCursor(window, cursor);
#endif
}

void Window::set_icons(std::initializer_list<std::unique_ptr<TextureData>> icons) {
    std::vector<GLFWimage> glfw_icons;
    glfw_icons.reserve(icons.size());

    for (const std::unique_ptr<TextureData>& icon : icons) {
        const TextureData::Image data = icon->get_data();

        GLFWimage image = {
            data.width,
            data.height,
            data.pixels
        };

        glfw_icons.push_back(image);
    }

    glfwSetWindowIcon(window, glfw_icons.size(), glfw_icons.data());
}

GLFWwindow* Window::create_window(Application* app) {
    GLFWmonitor* primary_monitor = nullptr;
    int width = 0, height = 0;

    if (app->data().fullscreen) {
        primary_monitor = glfwGetPrimaryMonitor();

        if (app->data().native_resolution) {
            const GLFWvidmode* video_mode = glfwGetVideoMode(primary_monitor);
            width = video_mode->width;
            height = video_mode->height;

            // Set the real resolution
            app->app_data.width = video_mode->width;
            app->app_data.height = video_mode->height;
        } else {
            width = app->data().width;  // FIXME maybe this could be larger than monitor's native resolution, which would crash the game
            height = app->data().height;
        }
    } else {
        width = app->data().width;
        height = app->data().height;
    }

    return glfwCreateWindow(width, height, app->data().title.c_str(), primary_monitor, nullptr);
}

std::pair<int, int> Monitor::get_resolution() {
    const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);

    return std::make_pair(video_mode->width, video_mode->height);
}

const char* Monitor::get_name() {
    return glfwGetMonitorName(monitor);
}

void destroy_glfw_context() {
    glfwTerminate();
}
