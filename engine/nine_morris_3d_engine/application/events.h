#pragma once

#include "nine_morris_3d_engine/application/input.h"

// #define BIND(function) std::bind(&function, this, std::placeholders::_1)
// #define STRINGIFY(event_type) #event_type

    // enum EventType {
    //     WindowClosed, WindowResized,
    //     KeyPressed, KeyReleased,
    //     MouseButtonPressed, MouseButtonReleased, MouseScrolled, MouseMoved
    // };

    // /**
    //  * Abstract event class. Every event derives this class.
    //  * Used in on_event() in Application.
    //  */
    // class Event {
    // public:
    //     Event() = default;
    //     virtual ~Event() = default;

    //     virtual EventType get_type() = 0;
    //     virtual const char* to_string() = 0;

    //     bool handled = false;
    // };

    // /**
    //  * Use this class to dispatch events to functions.
    //  */
    // class Dispatcher {
    // public:
    //     Dispatcher(Event& event)
    //         : event(event) {}
    //     ~Dispatcher() = default;

    //     template<typename E, typename F>
    //     void dispatch(const F& function) {
    //         // if (event.get_type() == type) {
    //             event.handled = function(static_cast<E&>(event));
    //         // }
    //     }
    // private:
    //     Event& event;
    // };

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
