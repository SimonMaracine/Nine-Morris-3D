#pragma once

#include <resmanager/resmanager.h>

#include "engine/application/application_data.h"
#include "engine/application/event.h"
#include "engine/application/window.h"
#include "engine/application/application_builder.h"
#include "engine/audio/context.h"
#include "engine/graphics/opengl/framebuffer.h"
#include "engine/graphics/renderer/renderer.h"
#include "engine/graphics/renderer/gui_renderer.h"
#include "engine/other/resource_manager.h"
#include "engine/other/dummy.h"
#include "engine/scene/scene.h"

class Application final {
private:
    using UserFunc = std::function<void(Application*)>;
    using SceneId = resmanager::HashedStr64;
    using RendererFunc = std::function<void()>;
public:
    static void preinitialize(std::string_view app_name, std::string_view log_file, std::string_view info_file);

    Application(const ApplicationBuilder& builder, std::any& user_data,
        const UserFunc& start = dummy::UserFunc {}, const UserFunc& stop = dummy::UserFunc {});
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    double get_fps() { return fps; }
    float get_delta() { return delta; }

    // Call this to run the application, after all scenes have been defined; it can return an exit code
    int run(SceneId start_scene_id);

    // Scene management functions
    template<typename S>
    void add_scene() { scenes.push_back(std::make_unique<S>()); }

    void change_scene(SceneId id);
    const Scene* get_current_scene() { return current_scene; }

    // Framebuffer management functions
    void add_framebuffer(std::shared_ptr<gl::Framebuffer> framebuffer);
    void purge_framebuffers();

    // Data management
    template<typename Data>
    Data& user_data() { return std::any_cast<Data&>(*_user_data); }

    const ApplicationData& data() { return app_data; }
    void destroy_user_data() { _user_data->reset(); }

    // Public fields accessible by all the code
    bool running = true;
    int exit_code = 0;
    std::unique_ptr<Window> window;  // The last* object destroyed in an application instance
    std::unique_ptr<OpenAlContext> openal;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<GuiRenderer> gui_renderer;
    EventDispatcher evt;  // Application manager of events
    ResourcesCache res;  // Global cache of resources
private:
    float update_frame_counter();
    unsigned int calculate_fixed_update();
    void check_changed_scene();

    void renderer_3d_func();
    void renderer_2d_func();
    void renderer_imgui_func();

    void prepare_scenes(SceneId start_scene_id);
    void on_start(Scene* scene);
    void user_start();
    void user_stop();
    void initialize_renderer_3d();
    void initialize_renderer_2d();
    void initialize_renderer_imgui();
    void initialize_audio();

    void on_window_closed(const WindowClosedEvent&);
    void on_window_resized(const WindowResizedEvent& event);

    void on_imgui_mouse_scrolled(const MouseScrolledEvent& event);
    void on_imgui_mouse_moved(const MouseMovedEvent& event);
    void on_imgui_mouse_button_pressed(const MouseButtonPressedEvent& event);
    void on_imgui_mouse_button_released(const MouseButtonReleasedEvent& event);

    ApplicationBuilder builder;
    std::any* _user_data = nullptr;
    ApplicationData app_data;
    UserFunc start;
    UserFunc stop;

    double fps = 0.0;
    float delta = 0.0f;

    // Data for the scene system
    std::vector<std::unique_ptr<Scene>> scenes;
    Scene* current_scene = nullptr;

    bool changed_scene = false;
    Scene* to_scene = nullptr;  // Next scene to enter

    // Data for modular rendering
    RendererFunc renderer_3d_update = dummy::ProcFunc {};
    RendererFunc renderer_2d_update = dummy::ProcFunc {};
    RendererFunc renderer_imgui_update = dummy::ProcFunc {};

    // Keep track of all framebuffers to resize them, if needed
    std::vector<std::weak_ptr<gl::Framebuffer>> framebuffers;

    // Clock variables
    struct {
        double previous_seconds = 0.0;
        int frame_count = 0;
        double total_time = 0.0;
    } frame_counter;

    struct {
        double previous_seconds = 0.0;
        double total_time = 0.0;
    } fixed_update;

    friend class Scene;
    friend class Window;
};
