#include "engine/application_base/events.h"

namespace sm {
    void EventDispatcher::update() {
        dispatcher.update();
    }

    std::ostream& operator<<(std::ostream& stream, const WindowClosedEvent&) {
        stream << "WindowClosedEvent {}";

        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const WindowResizedEvent& event) {
        stream << "WindowResizedEvent {" << event.width << ", " << event.height << "}";

        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const WindowFocusedEvent& event) {
        stream << "WindowFocusedEvent {" << event.focused << "}";

        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const KeyPressedEvent& event) {
        stream << "KeyPressedEvent {" << static_cast<int>(event.key) << ", " << event.repeat << "}";

        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const KeyReleasedEvent& event) {
        stream << "KeyReleasedEvent {" << static_cast<int>(event.key) << "}";

        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const MouseButtonPressedEvent& event) {
        stream << "MouseButtonPressedEvent {" << static_cast<int>(event.button) << "}";

        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const MouseButtonReleasedEvent& event) {
        stream << "MouseButtonReleasedEvent {" << static_cast<int>(event.button) << "}";

        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const MouseScrolledEvent& event) {
        stream << "MouseScrolledEvent {" << event.scroll << "}";

        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const MouseMovedEvent& event) {
        stream << "MouseMovedEvent {" << event.mouse_x << ", " << event.mouse_y << "}";

        return stream;
    }
}
