#pragma once

#include <vector>
#include <memory>

#include "application/application_data.h"
#include "application/icon_image.h"

struct GLFWwindow;
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
    void set_icons(const std::vector<std::unique_ptr<IconImage>>& icons);
    void set_custom_cursor(CustomCursor cursor) const;
private:
    GLFWwindow* window = nullptr;
    GLFWcursor* arrow_cursor = nullptr;
    GLFWcursor* cross_cursor = nullptr;

    CustomCursor cursor = CustomCursor::Arrow;
};

