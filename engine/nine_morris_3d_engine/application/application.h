#pragma once

#include <entt/entt.hpp>

#include "nine_morris_3d_engine/application/application_data.h"
#include "nine_morris_3d_engine/application/events.h"
#include "nine_morris_3d_engine/application/window.h"
#include "nine_morris_3d_engine/application/scene.h"
#include "nine_morris_3d_engine/application/application_builder.h"
#include "nine_morris_3d_engine/graphics/renderer/renderer.h"
#include "nine_morris_3d_engine/graphics/renderer/gui_renderer.h"
#include "nine_morris_3d_engine/graphics/opengl/framebuffer.h"
#include "nine_morris_3d_engine/other/resource_manager.h"

class Application final {
private:
    using UserFunc = std::function<void(Application*)>;
public:
    struct DummyUserFunc {
        constexpr void operator()(Application*) {}  // Do nothing
    };

    struct DummyUserData {};
    static std::any dummy_user_data();

    Application(const ApplicationBuilder& builder, std::any& user_data,
        const UserFunc& start = DummyUserFunc {}, const UserFunc& stop = DummyUserFunc {});
    ~Application();

    // Call this to run the application, after all scenes have been defined; it can return an exit code
    int run();

    // Scene management functions
    void add_scene(std::unique_ptr<Scene>&& scene, bool start = false);
    void change_scene(std::string_view name);

    // Framebuffer management functions
    void add_framebuffer(std::shared_ptr<Framebuffer> framebuffer);
    void purge_framebuffers();

    // Data management
    template<typename Data>
    Data& user_data() { return std::any_cast<Data&>(_user_data); }

    const ApplicationData& data() { return app_data; }
    void destroy_user_data() { _user_data.reset(); }

    double get_fps() { return fps; }
    float get_delta() { return delta; }

    // Public fields accessible by all the code
    bool running = true;
    int exit_code = 0;
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<GuiRenderer> gui_renderer;
    entt::dispatcher evt;  // Event dispatcher
    Resources res;  // Global cache of resources
private:
    float update_frame_counter();
    unsigned int calculate_fixed_update();
    void check_changed_scene();

    void renderer_3d_functionality();
    void renderer_2d_functionality();
    void renderer_imgui_functionality();

    void prepare_scenes();
    void on_start(Scene* scene);

    void on_window_closed(const WindowClosedEvent& event);
    void on_window_resized(const WindowResizedEvent& event);

    void on_imgui_mouse_scrolled(const MouseScrolledEvent& event);
    void on_imgui_mouse_moved(const MouseMovedEvent& event);
    void on_imgui_mouse_button_pressed(const MouseButtonPressedEvent& event);
    void on_imgui_mouse_button_released(const MouseButtonReleasedEvent& event);

    ApplicationBuilder builder;
    std::any& _user_data;
    ApplicationData app_data;
    UserFunc start;
    UserFunc stop;

    double fps = 0.0;
    float delta = 0.0f;

    // Data for the scene system
    std::vector<std::unique_ptr<Scene>> scenes;
    Scene* current_scene = nullptr;

    bool changed_scene = false;
    Scene* to_scene = nullptr;

    // Data for modular rendering
    struct DummyFunction {
        constexpr void operator()() {}  // Do nothing
    };

    std::function<void()> renderer_3d = DummyFunction {};
    std::function<void()> renderer_2d = DummyFunction {};
    std::function<void()> renderer_imgui = DummyFunction {};

    // Keep track of all framebuffers to resize them, if needed
    std::vector<std::weak_ptr<Framebuffer>> framebuffers;

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
