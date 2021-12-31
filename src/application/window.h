#pragma once

#include <string>

#include <stb_image.h>

#include "application/application_data.h"

struct GLFWwindow;
struct GLFWimage;

class Window {
public:
    Window(ApplicationData* data);
    ~Window();

    void update() const;

    GLFWwindow* get_handle() const;
    double get_time() const;

    void set_vsync(int interval) const;
private:
    GLFWwindow* window = nullptr;
};

class Icon {
public:
    Icon(const std::string& file_path);
    ~Icon();

    GLFWimage get_data() const;
private:
    int width = 0, height = 0, channels = 0;
    stbi_uc* data = nullptr;

    std::string file_path;
};
