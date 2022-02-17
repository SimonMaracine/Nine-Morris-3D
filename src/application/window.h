#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include <glad/glad.h>  // Include glad just to make everything compile
#include <GLFW/glfw3.h>

#include "application/application_data.h"
#include "application/icon_image.h"

class Window {
public:
    Window(ApplicationData* data);
    ~Window();

    void update() const;

    GLFWwindow* get_handle() const;
    double get_time() const;

    void set_vsync(int interval);
    unsigned int add_cursor(std::unique_ptr<IconImage> cursor, int x_hotspot, int y_hotspot);
    void set_cursor(unsigned int handle);

    template<unsigned int Count>
    void set_icons(const std::array<std::unique_ptr<IconImage>, Count>& icons) {
        std::array<GLFWimage, Count> glfw_icons;

        for (unsigned int i = 0; i < icons.size(); i++) {
            glfw_icons[i] = icons[i]->get_data();
        }

        glfwSetWindowIcon(window, glfw_icons.size(), glfw_icons.data());
    }
private:
    GLFWwindow* window = nullptr;

    std::unordered_map<unsigned int, GLFWcursor*> cursors;
};
