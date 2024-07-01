#pragma once

#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/events.hpp"
#include "nine_morris_3d_engine/application/window.hpp"
#include "nine_morris_3d_engine/application/tasks.hpp"
#include "nine_morris_3d_engine/application/properties.hpp"
#include "nine_morris_3d_engine/application/logging_base.hpp"
#include "nine_morris_3d_engine/application/file_system.hpp"
#include "nine_morris_3d_engine/application/input.hpp"
#include "nine_morris_3d_engine/application/id.hpp"
#include "nine_morris_3d_engine/audio/context.hpp"
#include "nine_morris_3d_engine/audio/music.hpp"
#include "nine_morris_3d_engine/graphics/renderer.hpp"
#include "nine_morris_3d_engine/graphics/shader_library.hpp"
#include "nine_morris_3d_engine/graphics/scene.hpp"
#include "nine_morris_3d_engine/graphics/debug_ui.hpp"
#include "nine_morris_3d_engine/other/resources_cache.hpp"
#include "nine_morris_3d_engine/other/random_gen.hpp"

namespace sm {
    class Application;

    // Wrapper around functionality exposed to the user
    class Ctx {
    public:
        explicit Ctx(const ApplicationProperties& properties)
            : fs(properties.application_name, properties.assets_directory), log(properties.log_file, fs),
            shd({"engine_assets", properties.assets_directory}), win(properties, &evt),
            rnd(properties.width, properties.height, properties.samples, fs, shd),
            snd(properties.audio), inp(win.get_handle()) {}

        ~Ctx() = default;

        Ctx(const Ctx&) = delete;
        Ctx& operator=(const Ctx&) = delete;
        Ctx(Ctx&&) = delete;
        Ctx& operator=(Ctx&&) = delete;

        void change_scene(Id id);
        void show_info_text();

        template<typename T>
        T& global() {
            return *static_cast<T*>(user_data);
        }

        bool running {true};
        int exit_code {};
        float delta {};
        float fps {};

        // The order of these members matters

        FileSystem fs;
        Logging log;
        ShaderLibrary shd;
        EventDispatcher evt;  // Application events
        Window win;  // One of the last objects destroyed in an application instance
        Renderer rnd;  // Renderer for 3D, 2D and debug
        OpenAlContext snd;
        MusicPlayer mus;
        ResourcesCache res;  // Global cache of resources
        TaskManager tsk;
        RandomGenerator rng;
        Input inp;
        Scene scn;
    private:
#ifndef SM_BUILD_DISTRIBUTION
        DebugUi dbg;
#endif
        void* user_data {nullptr};  // Arbitrary data defined by the user
        Application* application {nullptr};

        friend class Application;
    };
}
