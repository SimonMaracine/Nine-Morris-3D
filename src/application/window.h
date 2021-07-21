#pragma once

struct GLFWwindow;
struct ApplicationData;

class Window {
public:
    Window(int width, int height, ApplicationData* data);
    ~Window();

    void update() const;

    GLFWwindow* get_handle() const;
private:
    GLFWwindow* window;
};
