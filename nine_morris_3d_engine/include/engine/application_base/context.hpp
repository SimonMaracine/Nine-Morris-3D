#pragma once

#include "engine/application_base/events.hpp"
#include "engine/application_base/window.hpp"
#include "engine/application_base/tasks.hpp"
#include "engine/application_base/properties.hpp"
#include "engine/application_base/logging.hpp"
#include "engine/application_base/file_system.hpp"
#include "engine/application_base/input.hpp"
#include "engine/application_base/id.hpp"
#include "engine/audio/context.hpp"
#include "engine/audio/music.hpp"
#include "engine/graphics/renderer.hpp"
#include "engine/graphics/shader_library.hpp"
#include "engine/other/resources_cache.hpp"
#include "engine/other/random_gen.hpp"

namespace sm {
    class Application;

    // Wrapper around functionality exposed to the user
    class Ctx {
    public:
        explicit Ctx(const ApplicationProperties& properties)
            : fs(properties.application_name, properties.assets_directory), log(properties.log_file, fs),
            win(properties, &evt), rnd(properties.width, properties.height, fs), snd(properties.audio), inp(win.get_handle()) {}

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

        // The order of these members matters

        FileSystem fs;
        Logging log;
        EventDispatcher evt;  // Application events
        Window win;  // One of the last objects destroyed in an application instance
        Renderer rnd;  // Renderer for 3D, 2D and debug
        OpenAlContext snd;
        MusicPlayer mus;
        ShaderLibrary shd;
        ResourcesCache res;  // Global cache of resources
        TaskManager tsk;
        RandomGenerator rng;
        Input inp;
    private:
        void* user_data {nullptr};  // Arbitrary data defined by the user
        Application* application {nullptr};

        friend class Application;
    };
}
