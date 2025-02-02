#pragma once

#ifdef __GNUG__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wconversion"
#endif

#include <entt/signal/dispatcher.hpp>

#ifdef __GNUG__
    #pragma GCC diagnostic pop
#endif

namespace sm::internal {
    // Application-level event system
    class EventDispatcher {
    public:
        // Add an event handler
        template<typename E, auto F, typename... T>
        void connect(T&&... value_or_instance) {
            m_dispatcher.template sink<E>().template connect<F>(value_or_instance...);
        }

        // Remove an event handler
        template<typename E, auto F, typename... T>
        void disconnect(T&&... value_or_instance) {
            m_dispatcher.template sink<E>().template disconnect<F>(value_or_instance...);
        }

        // Remove an event handler
        template<typename T>
        void disconnect(T& value_or_instance) {
            m_dispatcher.disconnect(value_or_instance);
        }

        // Remove an event handler
        template<typename T>
        void disconnect(T* value_or_instance) {
            m_dispatcher.disconnect(value_or_instance);
        }

        // Enqueue an event
        template<typename E, typename... Args>
        void enqueue(Args&&... args) {
            m_dispatcher.template enqueue<E>(std::forward<Args>(args)...);
        }

        // Clear current events
        template<typename E>
        void clear() {
            m_dispatcher.clear<E>();
        }

        // Execute events
        void update() {
            m_dispatcher.update();
        }
    private:
        entt::dispatcher m_dispatcher;
    };
}
