#pragma once

#include <spdlog/fmt/fmt.h>

#include "nine_morris_3d_engine/application/internal/input_codes.hpp"

// Application-level events

namespace sm {
    struct WindowClosedEvent {};

    struct WindowResizedEvent {
        int width {};
        int height {};
    };

    struct WindowFocusGainedEvent {};

    struct WindowFocusLostEvent {};

    struct WindowMouseEnteredEvent {};

    struct WindowMouseLeftEvent {};

    struct WindowRestoredEvent {};

    struct WindowMinimizedEvent {};

    struct KeyPressedEvent {
        internal::Key key {};
        bool repeat {};
        bool ctrl {};
        bool shift {};
        bool alt {};
    };

    struct KeyReleasedEvent {
        internal::Key key {};
    };

    struct MouseButtonPressedEvent {
        internal::Button button {};
        float x {};
        float y {};
    };

    struct MouseButtonReleasedEvent {
        internal::Button button {};
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

SM_EVENT_FORMATTER(sm::WindowClosedEvent, "WindowClosedEvent")
SM_EVENT_FORMATTER(sm::WindowResizedEvent, "WindowResizedEvent({}, {})", event.width, event.height)
SM_EVENT_FORMATTER(sm::WindowFocusGainedEvent, "WindowFocusGainedEvent")
SM_EVENT_FORMATTER(sm::WindowFocusLostEvent, "WindowFocusLostEvent")
SM_EVENT_FORMATTER(sm::WindowMouseEnteredEvent, "WindowMouseEnteredEvent")
SM_EVENT_FORMATTER(sm::WindowMouseLeftEvent, "WindowMouseLeftEvent")
SM_EVENT_FORMATTER(sm::WindowRestoredEvent, "WindowRestoredEvent")
SM_EVENT_FORMATTER(sm::WindowMinimizedEvent, "WindowMinimizedEvent")
SM_EVENT_FORMATTER(sm::KeyPressedEvent, "KeyPressedEvent({}, {})", static_cast<int>(event.key), event.repeat)
SM_EVENT_FORMATTER(sm::KeyReleasedEvent, "KeyReleasedEvent({})", static_cast<int>(event.key))
SM_EVENT_FORMATTER(sm::MouseButtonPressedEvent, "MouseButtonPressedEvent({})", static_cast<int>(event.button))
SM_EVENT_FORMATTER(sm::MouseButtonReleasedEvent, "MouseButtonReleasedEvent({})", static_cast<int>(event.button))
SM_EVENT_FORMATTER(sm::MouseWheelScrolledEvent, "MouseWheelScrolledEvent({})", event.scroll)
SM_EVENT_FORMATTER(sm::MouseMovedEvent, "MouseMovedEvent({}, {})", event.x, event.y)
