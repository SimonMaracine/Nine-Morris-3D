#pragma once

#include <entt/signal/dispatcher.hpp>
#include <spdlog/fmt/fmt.h>

#include "engine/application_base/input.hpp"

namespace sm {
    class Application;

    /*
        Main class for managing application-level events
    */
    class EventDispatcher {
    public:
        EventDispatcher() = default;
        ~EventDispatcher() = default;

        EventDispatcher(const EventDispatcher&) = delete;
        EventDispatcher& operator=(const EventDispatcher&) = delete;
        EventDispatcher(EventDispatcher&&) = delete;
        EventDispatcher& operator=(EventDispatcher&&) = delete;

        template<typename E, auto F, typename... T>
        void connect(T&&... value_or_instance) {
            dispatcher.template sink<E>().template connect<F>(value_or_instance...);
        }

        template<typename E, auto F, typename... T>
        void disconnect(T&&... value_or_instance) {
            dispatcher.template sink<E>().template disconnect<F>(value_or_instance...);
        }

        template<typename T>
        void disconnect(T& value_or_instance) {
            dispatcher.disconnect(value_or_instance);
        }

        template<typename T>
        void disconnect(T* value_or_instance) {
            dispatcher.disconnect(value_or_instance);
        }

        template<typename E, typename... Args>
        void enqueue(Args&&... args) {
            dispatcher.template enqueue<E>(std::forward<Args>(args)...);
        }

        template<typename E>
        void discard() {
            dispatcher.clear<E>();
        }
    private:
        void update() {
            dispatcher.update();
        }

        entt::dispatcher dispatcher;

        friend class Application;
    };

    struct WindowClosedEvent {};

    struct WindowResizedEvent {
        int width;
        int height;
    };

    struct WindowFocusedEvent {
        bool focused;
    };

    struct KeyPressedEvent {
        Key key;
        bool repeat;
        bool ctrl;
    };

    struct KeyReleasedEvent {
        Key key;
    };

    struct MouseButtonPressedEvent {
        MouseButton button;
    };

    struct MouseButtonReleasedEvent {
        MouseButton button;
    };

    struct MouseWheelScrolledEvent {
        float scroll;
    };

    struct MouseMovedEvent {
        float mouse_x;
        float mouse_y;
    };
}

#define SM_EVENT_FORMATTER(EVENT, ...) \
    template<> \
    struct fmt::formatter<EVENT> { \
        constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { \
            if (ctx.begin() != ctx.end()) { \
                throw format_error("Invalid format"); \
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

SM_EVENT_FORMATTER(sm::WindowClosedEvent, "WindowClosedEvent")
SM_EVENT_FORMATTER(sm::WindowResizedEvent, "WindowResizedEvent({}, {})", event.width, event.height)
SM_EVENT_FORMATTER(sm::WindowFocusedEvent, "WindowFocusedEvent({})", event.focused)
SM_EVENT_FORMATTER(sm::KeyPressedEvent, "KeyPressedEvent({}, {})", static_cast<int>(event.key), event.repeat)
SM_EVENT_FORMATTER(sm::KeyReleasedEvent, "KeyReleasedEvent({})", static_cast<int>(event.key))
SM_EVENT_FORMATTER(sm::MouseButtonPressedEvent, "MouseButtonPressedEvent({})", static_cast<int>(event.button))
SM_EVENT_FORMATTER(sm::MouseButtonReleasedEvent, "MouseButtonReleasedEvent({})", static_cast<int>(event.button))
SM_EVENT_FORMATTER(sm::MouseWheelScrolledEvent, "MouseWheelScrolledEvent({})", event.scroll)
SM_EVENT_FORMATTER(sm::MouseMovedEvent, "MouseMovedEvent({}, {})", event.mouse_x, event.mouse_y)
