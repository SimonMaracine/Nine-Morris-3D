#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "nine_morris_3d_engine/application/window.h"
#include "nine_morris_3d_engine/application/application.h"
#include "nine_morris_3d_engine/application/application_data.h"
#include "nine_morris_3d_engine/application/events.h"
#include "nine_morris_3d_engine/application/platform.h"
#include "nine_morris_3d_engine/application/input.h"
#include "nine_morris_3d_engine/other/logging.h"

Window::Window(Application* app) {
    if (!glfwInit()) {
        REL_CRITICAL("Could not initialize GLFW, exiting...");
        exit(1);
    }

#ifdef PLATFORM_GAME_DEBUG
    glfwSetErrorCallback([](int error, const char* description) {
        DEB_CRITICAL("[GLFW Error Callback: {}] {}", error, description);
    });
#endif

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, app->app_data.resizable ? GLFW_TRUE : GLFW_FALSE);
#ifdef PLATFORM_GAME_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    DEB_INFO("Using OpenGL debug context");
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

    window = create_window(app);

    if (window == nullptr) {
        REL_CRITICAL("Could not create window, exiting...");
        exit(1);
    }

    DEB_INFO("Initialized GLFW and created window");

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        REL_CRITICAL("Could not initialize GLAD, exiting...");
        exit(1);
    }

    glfwSwapInterval(1);
    glfwSetWindowUserPointer(window, &app->app_data);
    glfwSetWindowSizeLimits(window, app->app_data.min_width, app->app_data.min_height, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        data->app->evt.enqueue<WindowClosedEvent>();
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        data->width = width;
        data->height = height;

        data->app->evt.enqueue<WindowResizedEvent>(width, height);
    });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int, int action, int mods) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        switch (action) {
            case GLFW_PRESS: {
                data->app->evt.enqueue<KeyPressedEvent>(
                    static_cast<input::Key>(key), false, mods & GLFW_MOD_CONTROL
                );
                break;
            }
            case GLFW_RELEASE: {
                data->app->evt.enqueue<KeyReleasedEvent>(static_cast<input::Key>(key));
                break;
            }
            case GLFW_REPEAT: {
                data->app->evt.enqueue<KeyPressedEvent>(
                    static_cast<input::Key>(key), true, mods & GLFW_MOD_CONTROL
                );
                break;
            }
        }
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        switch (action) {
            case GLFW_PRESS: {
                data->app->evt.enqueue<MouseButtonPressedEvent>(
                    static_cast<input::MouseButton>(button)
                );
                break;
            }
            case GLFW_RELEASE: {
                data->app->evt.enqueue<MouseButtonReleasedEvent>(
                    static_cast<input::MouseButton>(button)
                );
                break;
            }       
        }
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double, double yoffset) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        data->app->evt.enqueue<MouseScrolledEvent>(static_cast<float>(yoffset));
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        data->app->evt.enqueue<MouseMovedEvent>(
            static_cast<float>(xpos), static_cast<float>(ypos)
        );
    });
}

Window::~Window() {
    glfwDestroyWindow(window);
    for (const auto& cursor : cursors) {
        glfwDestroyCursor(cursor.second);
    }
    glfwTerminate();

    DEB_INFO("Terminated GLFW and destroyed window");
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
        exit(1);
    }

    std::vector<Monitor> result;

    for (int i = 0; i < count; i++) {
        Monitor monitor;
        monitor.monitor = monitors[i];

        result.push_back(monitor);
    }

    return result;
}

void Window::set_vsync(int interval) {
    glfwSwapInterval(interval);
}

unsigned int Window::add_cursor(std::unique_ptr<TextureData> cursor, int x_hotspot, int y_hotspot) {
    const Image data = cursor->get_data();

    GLFWcursor* glfw_cursor = glfwCreateCursor(
        reinterpret_cast<const GLFWimage*>(&data), x_hotspot, y_hotspot
    );

    if (glfw_cursor == nullptr) {
        REL_ERROR("Could not create custom cursor '{}'", cursor->get_file_path());
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

#if defined(PLATFORM_GAME_DEBUG)
    GLFWcursor* cursor;
    try {
        cursor = cursors.at(handle);
    } catch (const std::out_of_range&) {
        DEB_CRITICAL("Invalid handle: {}, exiting...", handle);
        exit(1);
    }
    glfwSetCursor(window, cursor);
#elif defined(PLATFORM_GAME_RELEASE)
    GLFWcursor* cursor = cursors[handle];
    glfwSetCursor(window, cursor);
#endif
}

GLFWwindow* Window::create_window(Application* app) {
    GLFWmonitor* primary_monitor = nullptr;
    int width = 0, height = 0;

    if (app->app_data.fullscreen) {
        primary_monitor = glfwGetPrimaryMonitor();

        if (app->app_data.native_resolution) {
            const GLFWvidmode* video_mode = glfwGetVideoMode(primary_monitor);
            width = video_mode->width;
            height = video_mode->height;
        } else {
            width = app->app_data.width;  // FIXME maybe this could be larger than monitor's native resolution, which would crash the game
            height = app->app_data.height;
        }
    } else {
        width = app->app_data.width;
        height = app->app_data.height;
    }

    return glfwCreateWindow(width, height, app->app_data.title.c_str(), primary_monitor, nullptr);
}

std::pair<int, int> Monitor::get_resolution() {
    const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);

    return std::make_pair(video_mode->width, video_mode->height);
}

const char* Monitor::get_name() {
    return glfwGetMonitorName(monitor);
}
