#pragma once

#include <resmanager/resmanager.hpp>

#include "engine/application_base/context.h"
#include "engine/application_base/events.h"
#include "engine/application_base/window.h"
#include "engine/application_base/input.h"
#include "engine/audio/context.h"
#include "engine/graphics/opengl/framebuffer.h"
#include "engine/graphics/renderer/renderer.h"
#include "engine/graphics/renderer/gui_renderer.h"
#include "engine/graphics/identifier.h"
#include "engine/other/resource_manager.h"
#include "engine/other/random_gen.h"
#include "engine/other/encrypt.h"

namespace sm {
    class Application;
    struct ApplicationProperties;
    class Scene;

    /**
     * Wrapper struct around public functionality exposed to user.
     */
    class Ctx final {
    private:
        using SceneId = resmanager::HashedStr64;
        Application* application = nullptr;
    public:
        Ctx() = default;
        ~Ctx() = default;

        Ctx(const Ctx&) = delete;
        Ctx& operator=(const Ctx&) = delete;
        Ctx(Ctx&&) = delete;
        Ctx& operator=(Ctx&&) = delete;

        void change_scene(SceneId id);
        const Scene* get_current_scene();

        // Framebuffer management functions
        void add_framebuffer(std::shared_ptr<gl::Framebuffer> framebuffer);
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

        std::unique_ptr<Window> window;  // The last* object destroyed in an application instance
        std::unique_ptr<OpenAlContext> snd;  // Sound context
        std::unique_ptr<Renderer> r3d;  // 3D renderer
        std::unique_ptr<GuiRenderer> r2d;  // 2D renderer
        std::unique_ptr<RandomGenerator> rng;  // Random number generator
        Input input;  // Self explanatory
        EventDispatcher evt;  // Manager of application events
        ResourcesCache res;  // Global cache of resources
        Identifier idt;  // Generator of IDs

        friend class Application;
    };
}
