#pragma once

namespace sm::internal {
    namespace imgui_context {
        void initialize(void* window, void* context);
        void uninitialize();

        void begin_frame();
        void end_frame();

        void process_event(const void* event);
        bool want_capture_mouse();
        bool want_capture_keyboard();
        void invalidate_texture();

        // bool on_mouse_wheel_scrolled(float yoffset);
        // bool on_mouse_moved(float xpos, float ypos);
        // bool on_mouse_button_pressed(int button);
        // bool on_mouse_button_released(int button);
        // bool on_key_pressed(int key, int scancode);
        // bool on_key_released(int key, int scancode);
        // bool on_char_typed(unsigned int codepoint);
    }
}
