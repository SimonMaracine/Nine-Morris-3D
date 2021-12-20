#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "application/window.h"
#include "application/application_data.h"
#include "application/events.h"
#include "other/logging.h"

Window::Window(ApplicationData* data) {
    if (!glfwInit()) {
        spdlog::critical("Could not initialize GLFW");
        std::exit(1);
    }

#ifndef NDEBUG
    glfwSetErrorCallback([](int error, const char* description) {
        spdlog::critical("[ID: {}] {}", error, description);
    });
#endif

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    spdlog::info("Using OpenGL debug context");
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

    window = glfwCreateWindow(data->width, data->height, data->title.c_str(), nullptr, nullptr);
    if (!window) {
        spdlog::critical("Could not create window");
        std::exit(1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        spdlog::critical("Could not initialize GLAD");
        std::exit(1);
    }

    glfwSwapInterval(1);
    glfwSetWindowUserPointer(window, data);

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        ApplicationData* data = (ApplicationData*) glfwGetWindowUserPointer(window);

        events::WindowClosedEvent event;
        data->event_function(event);
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        ApplicationData* data = (ApplicationData*) glfwGetWindowUserPointer(window);

        events::WindowResizedEvent event (width, height);
        data->width = width;
        data->height = height;
        data->event_function(event);
    });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        ApplicationData* data = (ApplicationData*) glfwGetWindowUserPointer(window);

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
        ApplicationData* data = (ApplicationData*) glfwGetWindowUserPointer(window);

        if (action == GLFW_PRESS) {
            events::MouseButtonPressedEvent event (button);
            data->event_function(event);
        } else if (action == GLFW_RELEASE) {
            events::MouseButtonReleasedEvent event (button);
            data->event_function(event);
        }
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        ApplicationData* data = (ApplicationData*) glfwGetWindowUserPointer(window);

        events::MouseScrolledEvent event ((float) yoffset);
        data->event_function(event);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        ApplicationData* data = (ApplicationData*) glfwGetWindowUserPointer(window);

        events::MouseMovedEvent event ((float) xpos, (float) ypos);
        data->event_function(event);
    });

    atexit(glfwTerminate);  // TODO rework this
}

Window::~Window() {
    glfwDestroyWindow(window);
}

void Window::update() const {
    glfwPollEvents();
    glfwSwapBuffers(window);
}

GLFWwindow* Window::get_handle() const {
    return window;
}

void Window::set_vsync(int interval) const {
    glfwSwapInterval(interval);
}

double Window::get_time() const {
    return glfwGetTime();
}
