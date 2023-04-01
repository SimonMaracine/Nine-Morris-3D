#pragma once

#include <resmanager/resmanager.h>

#include "engine/application/application_properties.h"
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
    using UserFunc = std::function<void(Ctx*)>;
    using SceneId = resmanager::HashedStr64;
    using RendererFunc = std::function<void()>;
public:
    static void preinitialize(std::string_view app_name, std::string_view log_file, std::string_view info_file);

    Application(const ApplicationBuilder& builder, void* user_data = nullptr);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    // Call this to launch the application; it can return an exit code
    int run(SceneId start_scene_id);

    // Add scenes to the application before calling run()
    template<typename S>
    void add_scene() {
        scenes.push_back(std::make_unique<S>());
    }

    void set_start_function(const UserFunc& start);
    void set_stop_function(const UserFunc& stop);

    // API accessible to the user
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
    void user_start_function();
    void user_stop_function();
    void initialize_r3d();
    void initialize_r2d();
    void initialize_dear_imgui();
    void initialize_audio();
    void initialize_random_generator();

    void on_window_closed(const WindowClosedEvent&);
    void on_window_resized(const WindowResizedEvent& event);

    ApplicationBuilder builder;
    ApplicationProperties properties;
    UserFunc start = dummy::UserFunc {};
    UserFunc stop = dummy::UserFunc {};

    // Data for the scene system
    std::vector<std::unique_ptr<Scene>> scenes;
    Scene* current_scene = nullptr;

    bool changed_scene = false;  // Flag set when the user requested a scene change
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
    friend class Ctx;
};
