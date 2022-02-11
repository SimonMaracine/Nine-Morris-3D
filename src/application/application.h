#pragma once

#include <memory>
#include <string>
#include <vector>

#include "application/application_data.h"
#include "application/events.h"
#include "application/window.h"
#include "graphics/renderer/renderer.h"
#include "graphics/renderer/framebuffer.h"
#include "nine_morris_3d/assets_data.h"
#include "nine_morris_3d/options.h"
#include "other/loader.h"

constexpr int VERSION_MAJOR = 0;
constexpr int VERSION_MINOR = 1;
constexpr int VERSION_PATCH = 0;

class Layer;
class Scene;

class Application {
public:
    Application(int width, int height, const std::string& title);
    ~Application();

    static void set_pointer(Application* instance);

    void run();

    void add_scene(Scene* scene);
    void set_starting_scene(Scene* scene);
    void change_scene(const std::string& id);

    void add_framebuffer(std::shared_ptr<Framebuffer> framebuffer);
    void purge_framebuffers();

    int get_width() const;
    int get_height() const;

    bool running = true;
    double fps = 0.0;
    ApplicationData data;
    std::shared_ptr<AssetsData> assets_data;
    std::shared_ptr<Window> window;
    renderer::Storage* storage = nullptr;
    options::Options options;
private:
    void on_event(events::Event& event);
    float update_frame_counter();
    unsigned int calculate_fixed_update();

    void push_layer(Layer* layer);
    void pop_layer(Layer* layer);

    bool on_window_closed(events::WindowClosedEvent& event);
    bool on_window_resized(events::WindowResizedEvent& event);

    std::vector<Scene*> scenes;
    Scene* current_scene = nullptr;
    std::vector<Layer*> layer_stack;

    bool changed_scene = false;
    Scene* to_scene = nullptr;

    std::vector<std::weak_ptr<Framebuffer>> framebuffers;

    friend class Layer;
};
