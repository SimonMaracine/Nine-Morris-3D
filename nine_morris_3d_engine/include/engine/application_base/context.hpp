#pragma once

#include <memory>

#include "engine/application_base/events.hpp"
#include "engine/application_base/window.hpp"
#include "engine/application_base/tasks.hpp"
#include "engine/application_base/properties.hpp"
#include "engine/application_base/id.hpp"
#include "engine/audio/context.hpp"
#include "engine/graphics/renderer.hpp"
#include "engine/other/resources_cache.hpp"
#include "engine/other/random_gen.hpp"

namespace sm {
    class Application;

    // Wrapper around functionality exposed to the user
    class Ctx {
    public:
        explicit Ctx(const ApplicationProperties& properties)
            : win(properties, &evt), rnd(properties.width, properties.height), snd(properties.audio) {}

        ~Ctx() = default;

        Ctx(const Ctx&) = delete;
        Ctx& operator=(const Ctx&) = delete;
        Ctx(Ctx&&) = delete;
        Ctx& operator=(Ctx&&) = delete;

        void change_scene(Id id);

        template<typename T>
        T& data() {
            return *static_cast<T*>(user_data);
        }

        bool running {true};
        int exit_code {};
        float delta {};
        float fps {};

        EventDispatcher evt;  // Manager of application events
        Window win;  // One of the last objects destroyed in an application instance
        Renderer rnd;  // Renderer for 3D, 2D and debug
        OpenAlContext snd;  // Audio context
        ResourcesCache res;  // Global cache of resources
        TaskManager tsk;  // Manager of general purpose procedures
        RandomGenerator rng;  // Random number generator
    private:
        void* user_data {nullptr};  // Arbitrary data defined by the user
        Application* application {nullptr};

        friend class Application;
    };
}
