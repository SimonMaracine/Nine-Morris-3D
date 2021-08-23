#pragma once

struct GLFWwindow;
struct ApplicationData;

class Window {
public:
    Window(int width, int height, ApplicationData* data);
    ~Window();

    void update() const;

    GLFWwindow* get_handle() const;
    double get_time() const;
    
    void set_vsync(int interval) const;
private:
    GLFWwindow* window;
};
