#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "application/window.h"
#include "application/application_data.h"
#include "application/events.h"
#include "application/platform.h"
#include "other/logging.h"

Window::Window(ApplicationData* data) {
    if (!glfwInit()) {
        REL_CRITICAL("Could not initialize GLFW, exiting...");
        exit(1);
    }

#ifdef NINE_MORRIS_3D_DEBUG
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
#ifdef NINE_MORRIS_3D_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    DEB_INFO("Using OpenGL debug context");
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

    window = glfwCreateWindow(data->width, data->height, data->title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        REL_CRITICAL("Could not create window, exiting...");
        exit(1);
    }

    DEB_INFO("Initialized GLFW and created window");

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        REL_CRITICAL("Could not initialize GLAD, exiting...");
        exit(1);
    }

    glfwSwapInterval(1);
    glfwSetWindowUserPointer(window, data);

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        events::WindowClosedEvent event;
        data->event_function(event);
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        events::WindowResizedEvent event (width, height);
        data->width = width;
        data->height = height;
        data->event_function(event);
    });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        switch (action) {
            case GLFW_PRESS: {
                events::KeyPressedEvent event (key);
                data->event_function(event);
                break;
            }
            case GLFW_RELEASE: {
                events::KeyReleasedEvent event (key);
                data->event_function(event);
                break;
            }
            case GLFW_REPEAT: {
                events::KeyPressedEvent event (key);
                data->event_function(event);
                break;
            }
        }
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        switch (action) {
            case GLFW_PRESS: {
                events::MouseButtonPressedEvent event (button);
                data->event_function(event);
                break;
            }
            case GLFW_RELEASE: {
                events::MouseButtonReleasedEvent event (button);
                data->event_function(event);
                break;
            }       
        }
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        events::MouseScrolledEvent event (static_cast<float>(yoffset));
        data->event_function(event);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        ApplicationData* data = static_cast<ApplicationData*>(glfwGetWindowUserPointer(window));

        events::MouseMovedEvent event (static_cast<float>(xpos), static_cast<float>(ypos));
        data->event_function(event);
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
    GLFWimage data = cursor->get_data_glfw();

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

#if defined(NINE_MORRIS_3D_DEBUG)
    GLFWcursor* cursor;
    try {
        cursor = cursors.at(handle);
    } catch (const std::out_of_range&) {
        DEB_CRITICAL("Invalid handle: {}, exiting...", handle);
        exit(1);
    }
    glfwSetCursor(window, cursor);
#elif defined(NINE_MORRIS_3D_RELEASE)
    GLFWcursor* cursor = cursors[handle];
    glfwSetCursor(window, cursor);
#endif
}
