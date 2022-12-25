#pragma once

#include <entt/signal/dispatcher.hpp>

#include "engine/application/input.h"

class EventDispatcher {  // TODO this probably needs to be extended in the future
public:
    EventDispatcher() = default;
    ~EventDispatcher() = default;

    EventDispatcher(const EventDispatcher&) = delete;
    EventDispatcher& operator=(const EventDispatcher&) = delete;
    EventDispatcher(EventDispatcher&&) = delete;
    EventDispatcher& operator=(EventDispatcher&&) = delete;

    template<typename E, auto F, typename T>
    void add_event(T&& instance);

    template<typename T>
    void remove_events(T&& instance);

    template<typename E, typename... Args>
    void enqueue(Args&&...);

    void update();
private:
    entt::dispatcher dispatcher;
};

template<typename E, auto F, typename T>
void EventDispatcher::add_event(T&& instance) {
    dispatcher.template sink<E>().template connect<F, T>(std::forward<T>(instance));
}

template<typename T>
void EventDispatcher::remove_events(T&& instance) {
    dispatcher.disconnect(std::forward<T>(instance));
}

template<typename E, typename... Args>
void EventDispatcher::enqueue(Args&&... args) {
    dispatcher.template enqueue<E>(std::forward<Args>(args)...);
}

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
