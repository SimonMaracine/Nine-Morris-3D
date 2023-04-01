#pragma once

#include <resmanager/resmanager.h>

#include "engine/application/context.h"
#include "engine/application/events.h"
#include "engine/application/window.h"
#include "engine/audio/context.h"
#include "engine/graphics/opengl/framebuffer.h"
#include "engine/graphics/renderer/renderer.h"
#include "engine/graphics/renderer/gui_renderer.h"
#include "engine/other/resource_manager.h"

class Application;
class Scene;
struct ApplicationProperties;

/**
 * Wrapper struct around public functionality exposed to user.
 */
class Ctx {
private:
    using SceneId = resmanager::HashedStr64;
    Application* application = nullptr;
public:
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
    ApplicationProperties* properties = nullptr;  // Application data like window width and height

    std::unique_ptr<Window> window;  // The last* object destroyed in an application instance
    std::unique_ptr<OpenAlContext> snd;  // Sound context
    std::unique_ptr<Renderer> r3d;  // 3D renderer
    std::unique_ptr<GuiRenderer> r2d;  // 2D renderer
    EventDispatcher evt;  // Manager of application events
    ResourcesCache res;  // Global cache of resources

    friend class Application;
};
