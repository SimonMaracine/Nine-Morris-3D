#pragma once

#include <entt/entt.hpp>

#include "nine_morris_3d_engine/application/application_data.h"
#include "nine_morris_3d_engine/application/events.h"
#include "nine_morris_3d_engine/application/window.h"
#include "nine_morris_3d_engine/ecs/system.h"
#include "nine_morris_3d_engine/graphics/renderer/renderer.h"
#include "nine_morris_3d_engine/graphics/renderer/gui_renderer.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/framebuffer.h"
#include "nine_morris_3d_engine/other/loader.h"

// class Layer;
class Scene;

class Application {
protected:
    Application(int width, int height, std::string_view title, std::string_view info_file,
            std::string_view log_file, std::string_view application_name);
    virtual ~Application();  // TODO maybe make not subclassable
public:
    // Call this to run the application, after all scenes have been defined
    void run();

    // Scene management functions
    void add_scene(Scene* scene, bool start = false);
    // void set_starting_scene(Scene* scene);
    void change_scene(std::string_view name);

    // Framebuffer management functions
    void add_framebuffer(std::shared_ptr<Framebuffer> framebuffer);  // TODO this shouldn't be here
    void purge_framebuffers();

    // This needs to be called whenever a layer is set active or not, so that it gets processed
    void update_active_layers();

    // Systems
    // void add_system(std::unique_ptr<System>&& system);

    double get_fps() { return fps; }
    float get_delta() { return delta; }
    unsigned int get_frames() { return frames; }
    float get_mouse_wheel() { return mouse_wheel; }
    float get_dx() { return dx; }
    float get_dy() { return dy; }
    float get_last_mouse_x() { return last_mouse_x; }
    float get_last_mouse_y() { return last_mouse_y; }

    // Public variables accessible by all the code
    bool running = true;
    ApplicationData app_data;
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<GuiRenderer> gui_renderer;
    // Camera camera;
    entt::registry registry;
    entt::dispatcher event_dispatcher;
private:
    // void on_event(events::Event& event);
    float update_frame_counter();
    unsigned int calculate_fixed_update();
    void check_changed_scene();

    // // These implement scenes in the game
    // void push_layer(Layer* layer);
    // void pop_layer();
    // void push_overlay(Layer* layer);
    // void pop_overlay();

    void on_start(Scene* scene);

    void on_window_closed(const WindowClosedEvent& event);
    void on_window_resized(const WindowResizedEvent& event);
    void on_mouse_scrolled(const MouseScrolledEvent& event);
    void on_mouse_moved(const MouseMovedEvent& event);

    double fps = 0.0;
    float delta = 0.0f;
    unsigned int frames = 0;  // TODO remove this, if not needed

    // Data for the layer and scene system
    std::vector<Scene*> scenes;
    Scene* current_scene = nullptr;

    // std::vector<Layer*> _layer_stack;  // Stores all normal layers in the current scene
    // std::vector<Layer*> _overlay_stack;  // Layers that are updated only after 3D rendering
    // std::vector<Layer*> active_layer_stack;  // Only active layers in current scene
    // std::vector<Layer*> active_overlay_stack;

    bool changed_scene = false;
    Scene* to_scene = nullptr;

    // Keep track of all framebuffers to resize them, if needed
    std::vector<std::weak_ptr<Framebuffer>> framebuffers;

    // Input stuff
    float mouse_wheel = 0.0f;
    float dx = 0.0f;
    float dy = 0.0f;
    float last_mouse_x = 0.0f;
    float last_mouse_y = 0.0f;

    friend class Layer;
};
