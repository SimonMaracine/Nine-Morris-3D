#pragma once

#include <entt/signal/dispatcher.hpp>

#include "engine/application/input.h"

// class EventDispatcher {  // TODO this probably needs to be extended in the future
// public:
//     EventDispatcher() = default;
//     ~EventDispatcher() = default;

//     EventDispatcher(const EventDispatcher&) = delete;
//     EventDispatcher& operator=(const EventDispatcher&) = delete;
//     EventDispatcher(EventDispatcher&&) = delete;
//     EventDispatcher& operator=(EventDispatcher&&) = delete;

//     template<typename E, auto F, typename T>
//     void add_event(T&& instance);

//     template<typename T>
//     void remove_events(T&& instance);

//     template<typename E>
//     void discard_events();

//     template<typename E, typename... Args>
//     void enqueue(Args&&...);

//     void update();
// private:
//     entt::dispatcher dispatcher;
// };

// template<typename E, auto F, typename T>
// void EventDispatcher::add_event(T&& instance) {
//     dispatcher.template sink<E>().template connect<F, T>(std::forward<T>(instance));
// }

// template<typename T>
// void EventDispatcher::remove_events(T&& instance) {
//     dispatcher.disconnect(std::forward<T>(instance));
// }

// template<typename E>
// void EventDispatcher::discard_events() {
//     dispatcher.clear<E>();
// }

// template<typename E, typename... Args>
// void EventDispatcher::enqueue(Args&&... args) {
//     dispatcher.template enqueue<E>(std::forward<Args>(args)...);
// }

namespace events {
    enum class EventType {
        None,
        WindowClosed,
        WindowResized,
        KeyPressed,
        KeyReleased,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseScrolled,
        MouseMoved,
        Custom
    };

    template<typename E>
    class Dispatcher {
    public:
        Dispatcher(E& event)
            : event(event) {}
    private:
        E& event;
    };

    struct Event {
        EventType type = EventType::None;
        bool skipped = false;
    };

    struct WindowClosedEvent : Event {};

    struct WindowResizedEvent : Event {
        int width;
        int height;
    };

    struct KeyPressedEvent : Event {
        input::Key key;
        bool repeat;
        bool control;
    };

    struct KeyReleasedEvent : Event {
        input::Key key;
    };

    struct MouseButtonPressedEvent : Event {
        input::MouseButton button;
    };

    struct MouseButtonReleasedEvent : Event {
        input::MouseButton button;
    };

    struct MouseScrolledEvent : Event {
        float scroll;
    };

    struct MouseMovedEvent : Event {
        float mouse_x;
        float mouse_y;
    };
}
