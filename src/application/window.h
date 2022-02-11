#pragma once

#include <string>

#include <stb_image.h>

#include "application/application_data.h"

struct GLFWwindow;
struct GLFWimage;
struct GLFWcursor;

enum class CustomCursor {
    None,
    Arrow,
    Cross
};

class Window {
public:
    Window(ApplicationData* data);
    ~Window();

    void update() const;

    GLFWwindow* get_handle() const;
    double get_time() const;

    void set_vsync(int interval) const;
    void set_custom_cursor(CustomCursor cursor) const;
private:
    GLFWwindow* window = nullptr;
    GLFWcursor* arrow_cursor = nullptr;
    GLFWcursor* cross_cursor = nullptr;

    CustomCursor cursor = CustomCursor::Arrow;
};

class IconImage {
public:
    IconImage(const std::string& file_path);
    ~IconImage();

    GLFWimage get_data() const;
private:
    int width = 0, height = 0, channels = 0;
    stbi_uc* data = nullptr;

    std::string file_path;
};
