#pragma once

#include "application/application_data.h"
#include "application/events.h"
#include "application/window.h"
#include "graphics/renderer/main_renderer.h"
#include "graphics/renderer/gui_renderer.h"
#include "graphics/renderer/opengl/framebuffer.h"
#include "nine_morris_3d/assets_data.h"
#include "other/loader.h"

class Layer;
class Scene;

/**
 * Abstract main class of the game. Contains all the data the game can have.
 * To be derived from another class and instantiated in main().
*/
class Application {
protected:
    Application(int width, int height, std::string_view title);
    virtual ~Application();
public:
    // Call this to run the application, after all layers and scenes have been defined
    void run();

    // Scene management functions
    void add_scene(Scene* scene);
    void set_starting_scene(Scene* scene);
    void change_scene(std::string_view id);

    // Framebuffer management functions
    void add_framebuffer(std::shared_ptr<Framebuffer> framebuffer);
    void purge_framebuffers();

    // This needs to be called whenever a layer is set active or not, so that it gets processed
    void update_active_layers();

    // Public variables accessible by all the code.
    bool running = true;
    double fps = 0.0;
    unsigned int frames = 0;  // TODO remove this, if not needed
    ApplicationData app_data;
    std::shared_ptr<AssetsData> assets_data;
    std::shared_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<GuiRenderer> gui_renderer;
    Camera camera;
private:
    void on_event(events::Event& event);
    float update_frame_counter();
    unsigned int calculate_fixed_update();
    void check_changed_scene();

    // These implement scenes in the game
    void push_layer(Layer* layer);
    void pop_layer();
    void push_overlay(Layer* layer);
    void pop_overlay();

    bool on_window_closed(events::WindowClosedEvent& event);
    bool on_window_resized(events::WindowResizedEvent& event);

    // Data for the layer and scene system
    std::vector<Scene*> scenes;
    Scene* current_scene = nullptr;

    std::vector<Layer*> _layer_stack;  // Stores all normal layers in the current scene
    std::vector<Layer*> _overlay_stack;  // Layers that are updated only after 3D rendering
    std::vector<Layer*> active_layer_stack;  // Only active layers in current scene
    std::vector<Layer*> active_overlay_stack;

    bool changed_scene = false;
    Scene* to_scene = nullptr;

    // Keep track of all framebuffers to resize them, if needed
    std::vector<std::weak_ptr<Framebuffer>> framebuffers;

    friend class Layer;
};
