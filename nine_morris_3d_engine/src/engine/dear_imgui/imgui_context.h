#pragma once

namespace imgui_context {
    void initialize(void* window_handle);
    void uninitialize();

    void begin_frame();
    void end_frame();

    bool on_mouse_scrolled(float yoffset);
    bool on_mouse_moved(float xpos, float ypos);
    bool on_mouse_button_pressed(int button);
    bool on_mouse_button_released(int button);
    bool on_key_pressed(int key, int scancode);
    bool on_key_released(int key, int scancode);
}
