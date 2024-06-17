#pragma once

namespace sm {
    struct DearImGuiContext {
        static void initialize(void* window_handle);
        static void uninitialize();

        static void begin_frame();
        static void end_frame();

        static bool on_mouse_wheel_scrolled(float yoffset);
        static bool on_mouse_moved(float xpos, float ypos);
        static bool on_mouse_button_pressed(int button);
        static bool on_mouse_button_released(int button);
        static bool on_key_pressed(int key, int scancode);
        static bool on_key_released(int key, int scancode);
        static bool on_char_typed(unsigned int codepoint);
    };
}
