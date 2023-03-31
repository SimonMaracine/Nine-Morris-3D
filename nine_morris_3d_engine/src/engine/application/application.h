#pragma once

#include <resmanager/resmanager.h>

#include "engine/application/application_data.h"
#include "engine/application/events.h"
#include "engine/application/window.h"
#include "engine/application/application_builder.h"
#include "engine/application/context.h"
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

    // Call this to launch the application after all scenes have been defined; it can return an exit code
    int run(SceneId start_scene_id);

    // Scene management functions
    template<typename S>
    void add_scene() { scenes.push_back(std::make_unique<S>()); }

    // API accessible to user
    Ctx ctx;
protected:
    float update_frame_counter();
    unsigned int calculate_fixed_update();
    void check_changed_scene();

    void r3d_function();
    void r2d_function();
    void dear_imgui_function();

    void prepare_scenes(SceneId start_scene_id);
    void on_start(Scene* scene);
    void user_start();
    void user_stop();
    void initialize_r3d();
    void initialize_r2d();
    void initialize_dear_imgui();
    void initialize_audio();

    void on_window_closed(const WindowClosedEvent&);
    void on_window_resized(const WindowResizedEvent& event);

    ApplicationBuilder builder;
    std::any* _user_data = nullptr;
    ApplicationData app_data;
    UserFunc start;
    UserFunc stop;

    // Data for the scene system
    std::vector<std::unique_ptr<Scene>> scenes;
    Scene* current_scene = nullptr;

    bool changed_scene = false;
    Scene* to_scene = nullptr;  // Next scene to enter

    // Data for modular rendering
    RendererFunc r3d_update = dummy::ProcFunc {};
    RendererFunc r2d_update = dummy::ProcFunc {};
    RendererFunc dear_imgui_update = dummy::ProcFunc {};

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
    friend struct Ctx;
};
