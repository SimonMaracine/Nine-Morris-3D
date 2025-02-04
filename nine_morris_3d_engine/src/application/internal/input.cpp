#include "nine_morris_3d_engine/application/internal/input.hpp"

// #include <GLFW/glfw3.h>
#include <SDL3/SDL.h>

namespace sm::internal {
    bool is_key_pressed(Key key) noexcept {
        // return glfwGetKey(m_window_handle, static_cast<int>(key)) == GLFW_PRESS;

        const bool* state {SDL_GetKeyboardState(nullptr)};

        return state[SDL_GetScancodeFromKey(key_to_sdl_keycode(key), nullptr)];
    }

    bool is_button_pressed(Button button) noexcept {
        // return glfwGetMouseButton(m_window_handle, static_cast<int>(button)) == GLFW_PRESS;

        const SDL_MouseButtonFlags state {SDL_GetMouseState(nullptr, nullptr)};

        return state & SDL_BUTTON_MASK(button_to_sdl_button(button));
    }

    std::pair<float, float> get_mouse_position() noexcept {
        float x {}, y {};

        SDL_GetMouseState(&x, &y);

        return std::make_pair(x, y);

        // double x, y;
        // glfwGetCursorPos(m_window_handle, &x, &y);

        // return std::make_pair(static_cast<float>(x), static_cast<float>(y));
    }

    // Key Input::key_from_code(int code) noexcept {
    //     return static_cast<Key>(code);
    // }

    // MouseButton Input::mouse_button_from_code(int code) noexcept {
    //     return static_cast<MouseButton>(code);
    // }
}
