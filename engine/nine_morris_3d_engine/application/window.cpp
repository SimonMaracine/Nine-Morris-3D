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

constexpr int MIN_WIDTH = 512;  // TODO maybe make this user made
constexpr int MIN_HEIGHT = 288;

Window::Window(Application* app)
    : app(app) {
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
#ifdef PLATFORM_GAME_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    DEB_INFO("Using OpenGL debug context");
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

    window = glfwCreateWindow(app->app_data.width, app->app_data.height, app->app_data.title.c_str(), nullptr, nullptr);
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
    glfwSetWindowSizeLimits(window, MIN_WIDTH, MIN_HEIGHT, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        // const WindowClosedEvent event;
        // data->event_function(event);
        data->app->event_dispatcher.enqueue<WindowClosedEvent>();
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        // const WindowResizedEvent event (width, height);
        data->width = width;
        data->height = height;
        // data->event_function(event);

        data->app->event_dispatcher.enqueue<WindowResizedEvent>(width, height);
    });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        switch (action) {
            case GLFW_PRESS: {
                // const KeyPressedEvent event (static_cast<input::Key>(key), false, mods & GLFW_MOD_CONTROL);
                // data->event_function(event);
                data->app->event_dispatcher.enqueue<KeyPressedEvent>(
                    static_cast<input::Key>(key), false, mods & GLFW_MOD_CONTROL
                );
                break;
            }
            case GLFW_RELEASE: {
                // const KeyReleasedEvent event (static_cast<input::Key>(key));
                // data->event_function(event);
                data->app->event_dispatcher.enqueue<KeyReleasedEvent>(static_cast<input::Key>(key));
                break;
            }
            case GLFW_REPEAT: {
                // const KeyPressedEvent event (static_cast<input::Key>(key), true, mods & GLFW_MOD_CONTROL);
                // data->event_function(event);
                data->app->event_dispatcher.enqueue<KeyPressedEvent>(
                    static_cast<input::Key>(key), true, mods & GLFW_MOD_CONTROL
                );
                break;
            }
        }
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        switch (action) {
            case GLFW_PRESS: {
                // const MouseButtonPressedEvent event (static_cast<input::MouseButton>(button));
                // data->event_function(event);
                data->app->event_dispatcher.enqueue<MouseButtonPressedEvent>(
                    static_cast<input::MouseButton>(button)
                );
                break;
            }
            case GLFW_RELEASE: {
                // const MouseButtonReleasedEvent event (static_cast<input::MouseButton>(button));
                // data->event_function(event);
                data->app->event_dispatcher.enqueue<MouseButtonReleasedEvent>(
                    static_cast<input::MouseButton>(button)
                );
                break;
            }       
        }
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        // const MouseScrolledEvent event (static_cast<float>(yoffset));
        // data->event_function(event);
        data->app->event_dispatcher.enqueue<MouseScrolledEvent>(static_cast<float>(yoffset));
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        // const MouseMovedEvent event (static_cast<float>(xpos), static_cast<float>(ypos));
        // data->event_function(event);
        data->app->event_dispatcher.enqueue<MouseMovedEvent>(
            static_cast<float>(xpos), static_cast<float>(ypos)
        );
    });
}

Window::~Window() {
    glfwDestroyWindow(window);
    for (const std::pair<unsigned int, GLFWcursor*>& cursor : cursors) {
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

void Window::set_vsync(int interval) {
    glfwSwapInterval(interval);
}

unsigned int Window::add_cursor(std::unique_ptr<TextureData> cursor, int x_hotspot, int y_hotspot) {
    const GLFWimage data = cursor->get_data_glfw();

    GLFWcursor* glfw_cursor = glfwCreateCursor(&data, x_hotspot, y_hotspot);
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
