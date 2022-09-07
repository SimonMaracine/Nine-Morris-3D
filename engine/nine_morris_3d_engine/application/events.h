#pragma once

#include "nine_morris_3d_engine/application/input.h"

struct WindowClosedEvent {};

struct WindowResizedEvent {
    int width;
    int height;
};

struct KeyPressedEvent {
    input::Key key;
    bool repeat;
    bool control;
};

struct KeyReleasedEvent {
    input::Key key;
};

struct MouseButtonPressedEvent {
    input::MouseButton button;
};

struct MouseButtonReleasedEvent {
    input::MouseButton button;
};

struct MouseScrolledEvent {
    float scroll;
};

struct MouseMovedEvent {
    float mouse_x;
    float mouse_y;
};
