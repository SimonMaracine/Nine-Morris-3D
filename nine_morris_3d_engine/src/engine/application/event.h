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

namespace event {
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

    struct Event {
        EventType type = EventType::None;
        bool skip = false;
    };

    class Dispatcher {
    public:
        Dispatcher(Event& event)
            : event(event) {}
        ~Dispatcher() = default;

        Dispatcher(const Dispatcher&) = delete;
        Dispatcher& operator=(const Dispatcher&) = delete;
        Dispatcher(Dispatcher&&) = delete;
        Dispatcher& operator=(Dispatcher&&) = delete;

        template<typename E, typename F>
        void dispatch(const F& handler) {
            if (event.type == E::static_type()) {
                event.skip = handler(static_cast<E&>(event));
            }
        }
    private:
        Event& event;
    };

    #define EVENT_IMPLEMENT(EventName, Type) \
        inline EventName() { type = EventType::Type; } \
        static EventType static_type() { return EventType::Type; } \
        static const char* static_name() { return #Type; }

    struct WindowClosedEvent : Event {
        EVENT_IMPLEMENT(WindowClosedEvent, WindowClosed)
    };

    struct WindowResizedEvent : Event {
        EVENT_IMPLEMENT(WindowResizedEvent, WindowResized)

        int width;
        int height;
    };

    struct KeyPressedEvent : Event {
        EVENT_IMPLEMENT(KeyPressedEvent, KeyPressed)

        input::Key key;
        bool repeat;
        bool control;
    };

    struct KeyReleasedEvent : Event {
        EVENT_IMPLEMENT(KeyReleasedEvent, KeyReleased)

        input::Key key;
    };

    struct MouseButtonPressedEvent : Event {
        EVENT_IMPLEMENT(MouseButtonPressedEvent, MouseButtonPressed)

        input::MouseButton button;
    };

    struct MouseButtonReleasedEvent : Event {
        EVENT_IMPLEMENT(MouseButtonReleasedEvent, MouseButtonReleased)

        input::MouseButton button;
    };

    struct MouseScrolledEvent : Event {
        EVENT_IMPLEMENT(MouseScrolledEvent, MouseScrolled)

        float scroll;
    };

    struct MouseMovedEvent : Event {
        EVENT_IMPLEMENT(MouseMovedEvent, MouseMoved)

        float mouse_x;
        float mouse_y;
    };
}
