#pragma once

#include <entt/signal/dispatcher.hpp>
#include <spdlog/fmt/fmt.h>

#include "engine/application/input.h"

/**
 * Main class for managing application-level events.
 */
class EventDispatcher {
public:
    EventDispatcher() = default;
    ~EventDispatcher() = default;

    EventDispatcher(const EventDispatcher&) = delete;
    EventDispatcher& operator=(const EventDispatcher&) = delete;
    EventDispatcher(EventDispatcher&&) = delete;
    EventDispatcher& operator=(EventDispatcher&&) = delete;

    template<typename E, auto F, typename T>
    void connect(T&& instance);

    template<typename T>
    void disconnect(T&& instance);

    template<typename E>
    void discard();

    template<typename E, typename... Args>
    void enqueue(Args&&...);

    inline void update() {
        dispatcher.update();
    }
private:
    entt::dispatcher dispatcher;
};

template<typename E, auto F, typename T>
void EventDispatcher::connect(T&& instance) {
    dispatcher.template sink<E>().template connect<F, T>(std::forward<T>(instance));
}

template<typename T>
void EventDispatcher::disconnect(T&& instance) {
    dispatcher.disconnect(std::forward<T>(instance));
}

template<typename E>
void EventDispatcher::discard() {
    dispatcher.clear<E>();
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

struct WindowFocusedEvent {
    bool focused;
};

struct KeyPressedEvent {
    Input::Key key;
    bool repeat;
    bool ctrl;
};

struct KeyReleasedEvent {
    Input::Key key;
};

struct MouseButtonPressedEvent {
    Input::MouseButton button;
};

struct MouseButtonReleasedEvent {
    Input::MouseButton button;
};

struct MouseScrolledEvent {
    float scroll;
};

struct MouseMovedEvent {
    float mouse_x;
    float mouse_y;
};

#define EVENT_FORMATTER(EVENT, ...) \
    template<> \
    struct fmt::formatter<EVENT> { \
        constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { \
            if (ctx.begin() != ctx.end()) { \
                throw format_error("invalid format"); \
            } \
        \
            return ctx.begin(); \
        } \
        \
        template <typename FormatContext> \
        auto format(const EVENT& event, FormatContext& ctx) const -> decltype(ctx.out()) { \
            return fmt::format_to(ctx.out(), __VA_ARGS__); \
        } \
    };

EVENT_FORMATTER(WindowClosedEvent, "WindowClosedEvent()")
EVENT_FORMATTER(WindowResizedEvent, "WindowResizedEvent({}, {})", event.width, event.height)
EVENT_FORMATTER(WindowFocusedEvent, "WindowFocusedEvent({})", event.focused)
EVENT_FORMATTER(KeyPressedEvent, "KeyPressedEvent({}, {})", static_cast<int>(event.key), event.repeat)
EVENT_FORMATTER(KeyReleasedEvent, "KeyReleasedEvent({})", static_cast<int>(event.key))
EVENT_FORMATTER(MouseButtonPressedEvent, "MouseButtonPressedEvent({})", static_cast<int>(event.button))
EVENT_FORMATTER(MouseButtonReleasedEvent, "MouseButtonReleasedEvent({})", static_cast<int>(event.button))
EVENT_FORMATTER(MouseScrolledEvent, "MouseScrolledEvent({})", event.scroll)
EVENT_FORMATTER(MouseMovedEvent, "MouseMovedEvent({}, {})", event.mouse_x, event.mouse_y)
