#pragma once

#include <spdlog/fmt/fmt.h>

#include "nine_morris_3d_engine/application/input_codes.hpp"

// Application-level events

namespace sm {
    struct WindowClosedEvent {};

    struct WindowResizedEvent {
        int width {};
        int height {};
    };

    struct WindowFocusedEvent {
        bool focused {};
    };

    struct WindowIconifiedEvent {
        bool iconified {};
    };

    struct WindowMovedEvent {
        int x {};
        int y {};
    };

    struct KeyPressedEvent {
        Key key {};
        bool repeat {};
        bool ctrl {};
        bool shift {};
        bool alt {};
    };

    struct KeyReleasedEvent {
        Key key {};
    };

    struct MouseButtonPressedEvent {
        Button button {};
        float x {};
        float y {};
    };

    struct MouseButtonReleasedEvent {
        Button button {};
        float x {};
        float y {};
    };

    struct MouseWheelScrolledEvent {
        float scroll {};
    };

    struct MouseMovedEvent {
        float x {};
        float y {};
        float xrel {};
        float yrel {};
    };
}

#define SM_EVENT_FORMATTER(EventType, ...) \
    template<> \
    struct fmt::formatter<EventType> : formatter<std::string_view> { \
        template <typename FormatContext> \
        auto format(const EventType& event, FormatContext& ctx) const -> decltype(ctx.out()) { \
            return fmt::format_to(ctx.out(), __VA_ARGS__); \
        } \
    };

// FIXME
SM_EVENT_FORMATTER(sm::WindowClosedEvent, "WindowClosedEvent")
SM_EVENT_FORMATTER(sm::WindowResizedEvent, "WindowResizedEvent({}, {})", event.width, event.height)
SM_EVENT_FORMATTER(sm::WindowFocusedEvent, "WindowFocusedEvent({})", event.focused)
SM_EVENT_FORMATTER(sm::WindowIconifiedEvent, "WindowIconifiedEvent({})", event.iconified)
SM_EVENT_FORMATTER(sm::WindowMovedEvent, "WindowMovedEvent({}, {})", event.position_x, event.position_y)
SM_EVENT_FORMATTER(sm::KeyPressedEvent, "KeyPressedEvent({}, {})", static_cast<int>(event.key), event.repeat)
SM_EVENT_FORMATTER(sm::KeyReleasedEvent, "KeyReleasedEvent({})", static_cast<int>(event.key))
SM_EVENT_FORMATTER(sm::MouseButtonPressedEvent, "MouseButtonPressedEvent({})", static_cast<int>(event.button))
SM_EVENT_FORMATTER(sm::MouseButtonReleasedEvent, "MouseButtonReleasedEvent({})", static_cast<int>(event.button))
SM_EVENT_FORMATTER(sm::MouseWheelScrolledEvent, "MouseWheelScrolledEvent({})", event.scroll)
SM_EVENT_FORMATTER(sm::MouseMovedEvent, "MouseMovedEvent({}, {})", event.mouse_x, event.mouse_y)
