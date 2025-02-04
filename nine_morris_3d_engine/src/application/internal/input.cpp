#include "nine_morris_3d_engine/application/internal/input.hpp"

#include <SDL3/SDL.h>

namespace sm::internal {
    bool is_key_pressed(Key key) {
        const bool* state {SDL_GetKeyboardState(nullptr)};

        return state[SDL_GetScancodeFromKey(key_to_sdl_keycode(key), nullptr)];
    }

    bool is_button_pressed(Button button) {
        const SDL_MouseButtonFlags state {SDL_GetMouseState(nullptr, nullptr)};

        return state & SDL_BUTTON_MASK(button_to_sdl_button(button));
    }

    std::pair<float, float> get_mouse_position() {
        float x {}, y {};

        SDL_GetMouseState(&x, &y);

        return std::make_pair(x, y);
    }
}
