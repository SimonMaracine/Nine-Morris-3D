#include "nine_morris_3d_engine/application/input_codes.hpp"

#include <SDL3/SDL.h>

namespace sm {
    unsigned int key_to_sdl_keycode(Key key) noexcept {
        switch (key) {
            case Key::Unknown: return SDLK_UNKNOWN;
            case Key::Return: return SDLK_RETURN;
            case Key::Escape: return SDLK_ESCAPE;
            case Key::Backspace: return SDLK_BACKSPACE;
            case Key::Tab: return SDLK_TAB;
            case Key::Space: return SDLK_SPACE;
            case Key::Exclaim: return SDLK_EXCLAIM;
            case Key::Dblapostrophe: return SDLK_DBLAPOSTROPHE;
            case Key::Hash: return SDLK_HASH;
            case Key::Dollar: return SDLK_DOLLAR;
            case Key::Percent: return SDLK_PERCENT;
            case Key::Ampersand: return SDLK_AMPERSAND;
            case Key::Apostrophe: return SDLK_APOSTROPHE;
            case Key::Leftparen: return SDLK_LEFTPAREN;
            case Key::Rightparen: return SDLK_RIGHTPAREN;
            case Key::Asterisk: return SDLK_ASTERISK;
            case Key::Plus: return SDLK_PLUS;
            case Key::Comma: return SDLK_COMMA;
            case Key::Minus: return SDLK_MINUS;
            case Key::Period: return SDLK_PERIOD;
            case Key::Slash: return SDLK_SLASH;
            case Key::_0: return SDLK_0;
            case Key::_1: return SDLK_1;
            case Key::_2: return SDLK_2;
            case Key::_3: return SDLK_3;
            case Key::_4: return SDLK_4;
            case Key::_5: return SDLK_5;
            case Key::_6: return SDLK_6;
            case Key::_7: return SDLK_7;
            case Key::_8: return SDLK_8;
            case Key::_9: return SDLK_9;
            case Key::Colon: return SDLK_COLON;
            case Key::Semicolon: return SDLK_SEMICOLON;
            case Key::Less: return SDLK_LESS;
            case Key::Equals: return SDLK_EQUALS;
            case Key::Greater: return SDLK_GREATER;
            case Key::Question: return SDLK_QUESTION;
            case Key::At: return SDLK_AT;
            case Key::Leftbracket: return SDLK_LEFTBRACKET;
            case Key::Backslash: return SDLK_BACKSLASH;
            case Key::Rightbracket: return SDLK_RIGHTBRACKET;
            case Key::Caret: return SDLK_CARET;
            case Key::Underscore: return SDLK_UNDERSCORE;
            case Key::Grave: return SDLK_GRAVE;
            case Key::A: return SDLK_A;
            case Key::B: return SDLK_B;
            case Key::C: return SDLK_C;
            case Key::D: return SDLK_D;
            case Key::E: return SDLK_E;
            case Key::F: return SDLK_F;
            case Key::G: return SDLK_G;
            case Key::H: return SDLK_H;
            case Key::I: return SDLK_I;
            case Key::J: return SDLK_J;
            case Key::K: return SDLK_K;
            case Key::L: return SDLK_L;
            case Key::M: return SDLK_M;
            case Key::N: return SDLK_N;
            case Key::O: return SDLK_O;
            case Key::P: return SDLK_P;
            case Key::Q: return SDLK_Q;
            case Key::R: return SDLK_R;
            case Key::S: return SDLK_S;
            case Key::T: return SDLK_T;
            case Key::U: return SDLK_U;
            case Key::V: return SDLK_V;
            case Key::W: return SDLK_W;
            case Key::X: return SDLK_X;
            case Key::Y: return SDLK_Y;
            case Key::Z: return SDLK_Z;
            case Key::Leftbrace: return SDLK_LEFTBRACE;
            case Key::Pipe: return SDLK_PIPE;
            case Key::Rightbrace: return SDLK_RIGHTBRACE;
            case Key::Tilde: return SDLK_TILDE;
            case Key::Delete: return SDLK_DELETE;
            case Key::Plusminus: return SDLK_PLUSMINUS;
            case Key::Capslock: return SDLK_CAPSLOCK;
            case Key::F1: return SDLK_F1;
            case Key::F2: return SDLK_F2;
            case Key::F3: return SDLK_F3;
            case Key::F4: return SDLK_F4;
            case Key::F5: return SDLK_F5;
            case Key::F6: return SDLK_F6;
            case Key::F7: return SDLK_F7;
            case Key::F8: return SDLK_F8;
            case Key::F9: return SDLK_F9;
            case Key::F10: return SDLK_F10;
            case Key::F11: return SDLK_F11;
            case Key::F12: return SDLK_F12;
            case Key::Printscreen: return SDLK_PRINTSCREEN;
            case Key::Scrolllock: return SDLK_SCROLLLOCK;
            case Key::Pause: return SDLK_PAUSE;
            case Key::Insert: return SDLK_INSERT;
            case Key::Home: return SDLK_HOME;
            case Key::Pageup: return SDLK_PAGEUP;
            case Key::End: return SDLK_END;
            case Key::Pagedown: return SDLK_PAGEDOWN;
            case Key::Right: return SDLK_RIGHT;
            case Key::Left: return SDLK_LEFT;
            case Key::Down: return SDLK_DOWN;
            case Key::Up: return SDLK_UP;
            case Key::Numlockclear: return SDLK_NUMLOCKCLEAR;
            case Key::LCtrl: return SDLK_LCTRL;
            case Key::LShift: return SDLK_LSHIFT;
            case Key::LAlt: return SDLK_LALT;
            case Key::Rctrl: return SDLK_RCTRL;
            case Key::RShift: return SDLK_RSHIFT;
            case Key::RAlt: return SDLK_RALT;
        }

        return SDLK_UNKNOWN;
    }

    Key sdl_keycode_to_key(unsigned int keycode) noexcept {
        switch (keycode) {
            case SDLK_UNKNOWN: return Key::Unknown;
            case SDLK_RETURN: return Key::Return;
            case SDLK_ESCAPE: return Key::Escape;
            case SDLK_BACKSPACE: return Key::Backspace;
            case SDLK_TAB: return Key::Tab;
            case SDLK_SPACE: return Key::Space;
            case SDLK_EXCLAIM: return Key::Exclaim;
            case SDLK_DBLAPOSTROPHE: return Key::Dblapostrophe;
            case SDLK_HASH: return Key::Hash;
            case SDLK_DOLLAR: return Key::Dollar;
            case SDLK_PERCENT: return Key::Percent;
            case SDLK_AMPERSAND: return Key::Ampersand;
            case SDLK_APOSTROPHE: return Key::Apostrophe;
            case SDLK_LEFTPAREN: return Key::Leftparen;
            case SDLK_RIGHTPAREN: return Key::Rightparen;
            case SDLK_ASTERISK: return Key::Asterisk;
            case SDLK_PLUS: return Key::Plus;
            case SDLK_COMMA: return Key::Comma;
            case SDLK_MINUS: return Key::Minus;
            case SDLK_PERIOD: return Key::Period;
            case SDLK_SLASH: return Key::Slash;
            case SDLK_0: return Key::_0;
            case SDLK_1: return Key::_1;
            case SDLK_2: return Key::_2;
            case SDLK_3: return Key::_3;
            case SDLK_4: return Key::_4;
            case SDLK_5: return Key::_5;
            case SDLK_6: return Key::_6;
            case SDLK_7: return Key::_7;
            case SDLK_8: return Key::_8;
            case SDLK_9: return Key::_9;
            case SDLK_COLON: return Key::Colon;
            case SDLK_SEMICOLON: return Key::Semicolon;
            case SDLK_LESS: return Key::Less;
            case SDLK_EQUALS: return Key::Equals;
            case SDLK_GREATER: return Key::Greater;
            case SDLK_QUESTION: return Key::Question;
            case SDLK_AT: return Key::At;
            case SDLK_LEFTBRACKET: return Key::Leftbracket;
            case SDLK_BACKSLASH: return Key::Backslash;
            case SDLK_RIGHTBRACKET: return Key::Rightbracket;
            case SDLK_CARET: return Key::Caret;
            case SDLK_UNDERSCORE: return Key::Underscore;
            case SDLK_GRAVE: return Key::Grave;
            case SDLK_A: return Key::A;
            case SDLK_B: return Key::B;
            case SDLK_C: return Key::C;
            case SDLK_D: return Key::D;
            case SDLK_E: return Key::E;
            case SDLK_F: return Key::F;
            case SDLK_G: return Key::G;
            case SDLK_H: return Key::H;
            case SDLK_I: return Key::I;
            case SDLK_J: return Key::J;
            case SDLK_K: return Key::K;
            case SDLK_L: return Key::L;
            case SDLK_M: return Key::M;
            case SDLK_N: return Key::N;
            case SDLK_O: return Key::O;
            case SDLK_P: return Key::P;
            case SDLK_Q: return Key::Q;
            case SDLK_R: return Key::R;
            case SDLK_S: return Key::S;
            case SDLK_T: return Key::T;
            case SDLK_U: return Key::U;
            case SDLK_V: return Key::V;
            case SDLK_W: return Key::W;
            case SDLK_X: return Key::X;
            case SDLK_Y: return Key::Y;
            case SDLK_Z: return Key::Z;
            case SDLK_LEFTBRACE: return Key::Leftbrace;
            case SDLK_PIPE: return Key::Pipe;
            case SDLK_RIGHTBRACE: return Key::Rightbrace;
            case SDLK_TILDE: return Key::Tilde;
            case SDLK_DELETE: return Key::Delete;
            case SDLK_PLUSMINUS: return Key::Plusminus;
            case SDLK_CAPSLOCK: return Key::Capslock;
            case SDLK_F1: return Key::F1;
            case SDLK_F2: return Key::F2;
            case SDLK_F3: return Key::F3;
            case SDLK_F4: return Key::F4;
            case SDLK_F5: return Key::F5;
            case SDLK_F6: return Key::F6;
            case SDLK_F7: return Key::F7;
            case SDLK_F8: return Key::F8;
            case SDLK_F9: return Key::F9;
            case SDLK_F10: return Key::F10;
            case SDLK_F11: return Key::F11;
            case SDLK_F12: return Key::F12;
            case SDLK_PRINTSCREEN: return Key::Printscreen;
            case SDLK_SCROLLLOCK: return Key::Scrolllock;
            case SDLK_PAUSE: return Key::Pause;
            case SDLK_INSERT: return Key::Insert;
            case SDLK_HOME: return Key::Home;
            case SDLK_PAGEUP: return Key::Pageup;
            case SDLK_END: return Key::End;
            case SDLK_PAGEDOWN: return Key::Pagedown;
            case SDLK_RIGHT: return Key::Right;
            case SDLK_LEFT: return Key::Left;
            case SDLK_DOWN: return Key::Down;
            case SDLK_UP: return Key::Up;
            case SDLK_NUMLOCKCLEAR: return Key::Numlockclear;
            case SDLK_LCTRL: return Key::LCtrl;
            case SDLK_LSHIFT: return Key::LShift;
            case SDLK_LALT: return Key::LAlt;
            case SDLK_RCTRL: return Key::Rctrl;
            case SDLK_RSHIFT: return Key::RShift;
            case SDLK_RALT: return Key::RAlt;
        }

        return Key::Unknown;
    }

    int button_to_sdl_button(Button button) noexcept {
        switch (button) {
            case Button::Left: return SDL_BUTTON_LEFT;
            case Button::Middle: return SDL_BUTTON_MIDDLE;
            case Button::Right: return SDL_BUTTON_RIGHT;
            case Button::X1: return SDL_BUTTON_X1;
            case Button::X2: return SDL_BUTTON_X2;
        }
    }

    Button sdl_button_to_button(int button) noexcept {
        switch (button) {
            case SDL_BUTTON_LEFT: return Button::Left;
            case SDL_BUTTON_MIDDLE: return Button::Middle;
            case SDL_BUTTON_RIGHT: return Button::Right;
            case SDL_BUTTON_X1: return Button::X1;
            case SDL_BUTTON_X2: return Button::X2;
        }
    }
}
