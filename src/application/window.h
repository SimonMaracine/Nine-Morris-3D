#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "application/application_data.h"
#include "application/icon_image.h"

struct GLFWwindow;
struct GLFWcursor;

class Window {
public:
    Window(ApplicationData* data);
    ~Window();

    void update() const;

    GLFWwindow* get_handle() const;
    double get_time() const;

    void set_vsync(int interval) const;
    void set_icons(const std::vector<std::unique_ptr<IconImage>>& icons) const;  // TODO rework this
    unsigned int add_cursor(std::unique_ptr<IconImage> cursor, int x_hotspot, int y_hotspot);
    void set_cursor(unsigned int handle) const;
private:
    GLFWwindow* window = nullptr;

    std::unordered_map<unsigned int, GLFWcursor*> cursors;
};

