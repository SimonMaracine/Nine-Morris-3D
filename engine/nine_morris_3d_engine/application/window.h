#pragma once

#include <glad/glad.h>  // Include glad just to make everything compile
#include <GLFW/glfw3.h>

#include "nine_morris_3d_engine/other/texture_data.h"

class Application;

class Window {
public:
    Window(Application* app);
    ~Window();

    // Call this to swap buffers and update events
    void update();

    GLFWwindow* get_handle();
    double get_time();

    // VSync, cursor and icon API
    void set_vsync(int interval);
    unsigned int add_cursor(std::unique_ptr<TextureData> cursor, int x_hotspot, int y_hotspot);
    void set_cursor(unsigned int handle);

    template<size_t Count>
    void set_icons(const std::array<std::unique_ptr<TextureData>, Count>& icons) {
        std::array<GLFWimage, Count> glfw_icons;

        for (size_t i = 0; i < icons.size(); i++) {
            Image data = icons[i]->get_data();
            glfw_icons[i] = *reinterpret_cast<GLFWimage*>(&data);
        }

        glfwSetWindowIcon(window, glfw_icons.size(), glfw_icons.data());
    }
private:
    GLFWwindow* window = nullptr;

    std::unordered_map<unsigned int, GLFWcursor*> cursors;
};
