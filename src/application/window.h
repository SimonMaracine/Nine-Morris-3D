#pragma once

#include "application/application_data.h"

struct GLFWwindow;

class Window {
public:
    Window(ApplicationData* data);
    ~Window();

    void update() const;

    GLFWwindow* get_handle() const;
    double get_time() const;

    void set_vsync(int interval) const;
private:
    GLFWwindow* window;
};
