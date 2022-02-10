#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "application/window.h"
#include "application/application_data.h"
#include "application/events.h"
#include "application/platform.h"
#include "other/logging.h"
#include "other/user_data.h"

static std::string path(const char* file_path) {
#if defined(NINE_MORRIS_3D_DEBUG)
      // Use relative path for both operating systems
      return std::string(file_path);
#elif defined(NINE_MORRIS_3D_RELEASE)
    #if defined(NINE_MORRIS_3D_LINUX)
      std::string path = std::string("/usr/share/") + APP_NAME_LINUX + "/" + file_path;
      return path;
    #elif defined(NINE_MORRIS_3D_WINDOWS)
      // Just use relative path
      return std::string(file_path);
    #endif
#endif
}

Window::Window(ApplicationData* data) {
    if (!glfwInit()) {
        REL_CRITICAL("Could not initialize GLFW, exiting...");
        std::exit(1);
    }

#ifdef NINE_MORRIS_3D_DEBUG
    glfwSetErrorCallback([](int error, const char* description) {
        DEB_CRITICAL("[ID: {}] {}", error, description);
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
        std::exit(1);
    }

    DEB_INFO("Initialized GLFW and created window");

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        REL_CRITICAL("Could not initialize GLAD, exiting...");
        std::exit(1);
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

        if (action == GLFW_PRESS) {
            events::MouseButtonPressedEvent event (button);
            data->event_function(event);
        } else if (action == GLFW_RELEASE) {
            events::MouseButtonReleasedEvent event (button);
            data->event_function(event);
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

    IconImage icons[5] = {
        IconImage(path("data/icons/512x512/ninemorris3d.png")),
        IconImage(path("data/icons/256x256/ninemorris3d.png")),
        IconImage(path("data/icons/128x128/ninemorris3d.png")),
        IconImage(path("data/icons/64x64/ninemorris3d.png")),
        IconImage(path("data/icons/32x32/ninemorris3d.png"))
    };

    GLFWimage glfw_icons[5];
    glfw_icons[0] = icons[0].get_data();
    glfw_icons[1] = icons[1].get_data();
    glfw_icons[2] = icons[2].get_data();
    glfw_icons[3] = icons[3].get_data();
    glfw_icons[4] = icons[4].get_data();

    glfwSetWindowIcon(window, 5, glfw_icons);

    IconImage cursors[2] = {
        IconImage(path("data/cursors/arrow.png")),
        IconImage(path("data/cursors/cross.png"))
    };

    GLFWimage glfw_cursors[2];
    glfw_cursors[0] = cursors[0].get_data();
    glfw_cursors[1] = cursors[1].get_data();

    arrow_cursor = glfwCreateCursor(&glfw_cursors[0], 4, 1);
    if (arrow_cursor == nullptr) {
        REL_ERROR("Could not create custom cursor");
    }

    cross_cursor = glfwCreateCursor(&glfw_cursors[1], 8, 8);
    if (cross_cursor == nullptr) {
        REL_ERROR("Could not create custom cursor");
    }
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwDestroyCursor(arrow_cursor);
    glfwDestroyCursor(cross_cursor);
    glfwTerminate();

    DEB_INFO("Terminated GLFW and destroyed window");
}

void Window::update() const {
    glfwPollEvents();
    glfwSwapBuffers(window);
}

GLFWwindow* Window::get_handle() const {
    return window;
}

double Window::get_time() const {
    return glfwGetTime();
}

void Window::set_vsync(int interval) const {
    glfwSwapInterval(interval);
}

void Window::set_custom_cursor(CustomCursor cursor) const {
    switch (cursor) {
        case CustomCursor::None:
            glfwSetCursor(window, nullptr);
            break;
        case CustomCursor::Arrow:
            glfwSetCursor(window, arrow_cursor);
            break;
        case CustomCursor::Cross:
            glfwSetCursor(window, cross_cursor);
            break;
    }
}

// --- IconImage

IconImage::IconImage(const std::string& file_path) {
    data = stbi_load(file_path.c_str(), &width, &height, &channels, 4);

    if (data == nullptr) {
        REL_CRITICAL("Could not load icon image '{}', exiting...", file_path.c_str());
        std::exit(1);
    }

    this->file_path = file_path;

    DEB_INFO("Loaded icon image data '{}'", file_path.c_str());
}

IconImage::~IconImage() {
    stbi_image_free(data);

    DEB_INFO("Freed icon image data '{}'", file_path.c_str());
}

GLFWimage IconImage::get_data() const {
    GLFWimage image;
    image.width = width;
    image.height = height;
    image.pixels = data;

    return image;
}
