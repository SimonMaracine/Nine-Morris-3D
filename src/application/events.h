#pragma once

#define BIND(function) std::bind(&function, this, std::placeholders::_1)

namespace events {
    enum EventType {
        WindowClosed, WindowResized,
        KeyPressed, KeyReleased,
        MouseButtonPressed, MouseButtonReleased, MouseScrolled, MouseMoved
    };

    /**
     * Abstract event class. Every event derives this class.
     * Used in on_event() in application.
     */
    class Event {
    public:
        virtual EventType get_type() = 0;
        virtual const char* to_string() = 0;

        bool handled = false;
    };

    /**
     * Use this class to dispatch events to functions.
     */
    class Dispatcher {
    public:
        Dispatcher(Event& event)
            : event(event) {}

        template<typename E, typename F>
        void dispatch(EventType type, const F& function) {
            if (event.get_type() == type) {
                event.handled = function((E&) event);
            }
        }
    private:
        Event& event;
    };

    class WindowClosedEvent : public Event {
    public:
        WindowClosedEvent() = default;

        virtual EventType get_type() override {
            return WindowClosed;
        }

        virtual const char* to_string() override {
            return "WindowClosed";
        }
    };

    class WindowResizedEvent : public Event {
    public:
        WindowResizedEvent(int width, int height)
            : width(width), height(height) {}

        virtual EventType get_type() override {
            return WindowResized;
        }

        virtual const char* to_string() override {
            return "WindowResized";
        }

        int width, height;
    };

    class KeyPressedEvent : public Event {
    public:
        KeyPressedEvent(int key)
            : key(key) {}

        virtual EventType get_type() override {
            return KeyPressed;
        }

        virtual const char* to_string() override {
            return "KeyPressed";
        }

        int key;
    };

    class KeyReleasedEvent : public Event {
    public:
        KeyReleasedEvent(int key)
            : key(key) {}

        virtual EventType get_type() override {
            return KeyReleased;
        }

        virtual const char* to_string() override {
            return "KeyReleased";
        }

        int key;
    };

    class MouseButtonPressedEvent : public Event {
    public:
        MouseButtonPressedEvent(int button)
            : button(button) {}

        virtual EventType get_type() override {
            return MouseButtonPressed;
        }

        virtual const char* to_string() override {
            return "MouseButtonPressed";
        }

        int button;
    };

    class MouseButtonReleasedEvent : public Event {
    public:
        MouseButtonReleasedEvent(int button)
            : button(button) {}

        virtual EventType get_type() override {
            return MouseButtonReleased;
        }

        virtual const char* to_string() override {
            return "MouseButtonReleased";
        }

        int button;
    };

    class MouseScrolledEvent : public Event {
    public:
        MouseScrolledEvent(float scroll)
            : scroll(scroll) {}

        virtual EventType get_type() override {
            return MouseScrolled;
        }

        virtual const char* to_string() override {
            return "MouseScrolled";
        }

        float scroll;
    };

    class MouseMovedEvent : public Event {
    public:
        MouseMovedEvent(float mouse_x, float mouse_y)
            : mouse_x(mouse_x), mouse_y(mouse_y) {}

        virtual EventType get_type() override {
            return MouseMoved;
        }

        virtual const char* to_string() override {
            return "MouseMoved";
        }

        float mouse_x, mouse_y;
    };
}
