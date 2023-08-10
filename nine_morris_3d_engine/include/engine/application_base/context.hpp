#pragma once

#include <memory>

#include <resmanager/resmanager.hpp>

#include "engine/application_base/context.hpp"
#include "engine/application_base/events.hpp"
#include "engine/application_base/window.hpp"
#include "engine/application_base/input.hpp"
#include "engine/application_base/tasks.hpp"
#include "engine/audio/context.hpp"
#include "engine/graphics/opengl/framebuffer.hpp"
#include "engine/graphics/renderer/renderer.hpp"
#include "engine/graphics/renderer/gui_renderer.hpp"
#include "engine/graphics/identifier.hpp"
#include "engine/other/resource_manager.hpp"
#include "engine/other/random_gen.hpp"
#include "engine/other/encrypt.hpp"

namespace sm {
    class Application;
    struct ApplicationProperties;
    class Scene;

    /*
        Wrapper struct around public functionality exposed to user.
    */
    class Ctx {
    public:
        using SceneId = resmanager::HashedStr64;

        Ctx() = default;
        ~Ctx() = default;

        Ctx(const Ctx&) = delete;
        Ctx& operator=(const Ctx&) = delete;
        Ctx(Ctx&&) = delete;
        Ctx& operator=(Ctx&&) = delete;

        void change_scene(SceneId id);
        const Scene* get_current_scene();

        // Framebuffer management functions
        void add_framebuffer(std::shared_ptr<GlFramebuffer> framebuffer);  // TODO better way?
        void purge_framebuffers();

        // Helper function
        template<typename T>
        T& data() {
            return *static_cast<T*>(user_data);
        }

        bool running = true;
        int exit_code = 0;
        double fps = 0.0;
        float delta = 0.0f;

        void* user_data = nullptr;  // Arbitrary data defined by the user
        const ApplicationProperties* properties = nullptr;  // Ctx data like window width and height

        std::unique_ptr<Window> window;  // One of the last object destroyed in an application instance
        std::unique_ptr<OpenAlContext> snd;  // Sound context
        std::unique_ptr<Renderer> r3d;  // 3D renderer
        // std::unique_ptr<GuiRenderer> r2d;  // 2D renderer
        std::unique_ptr<RandomGenerator> rng;  // Random number generator
        Input inp;  // Self explanatory
        EventDispatcher evt;  // Manager of application events
        ResourcesCache res;  // Global cache of resources
        Identifier idt;  // Generator of IDs
        TaskManager tsk;  // Manager of general purpose procedures
    private:
        Application* application = nullptr;

        friend class Application;
    };
}
