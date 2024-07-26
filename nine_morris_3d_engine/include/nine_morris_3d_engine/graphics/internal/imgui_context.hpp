#pragma once

struct GLFWwindow;

namespace sm::internal {
    namespace imgui_context {
        void initialize(GLFWwindow* window_handle) noexcept;
        void uninitialize() noexcept;

        void begin_frame() noexcept;
        void end_frame() noexcept;

        bool on_mouse_wheel_scrolled(float yoffset) noexcept;
        bool on_mouse_moved(float xpos, float ypos) noexcept;
        bool on_mouse_button_pressed(int button) noexcept;
        bool on_mouse_button_released(int button) noexcept;
        bool on_key_pressed(int key, int scancode) noexcept;
        bool on_key_released(int key, int scancode) noexcept;
        bool on_char_typed(unsigned int codepoint) noexcept;
    };
}
