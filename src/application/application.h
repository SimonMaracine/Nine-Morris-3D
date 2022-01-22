#pragma once

#include <memory>
#include <string>
#include <vector>

#include "application/application_data.h"
#include "application/events.h"
#include "application/window.h"
#include "graphics/renderer/renderer.h"
#include "graphics/renderer/framebuffer.h"
#include "other/loader.h"
#include "nine_morris_3d/assets_load.h"

constexpr int VERSION_MAJOR = 0;
constexpr int VERSION_MINOR = 1;
constexpr int VERSION_PATCH = 0;

class Layer;
class Scene;

class Application {
public:
    Application(int width, int height, const std::string& title);
    ~Application();

    void run();

    void add_scene(Scene* scene);
    void set_starting_scene(Scene* scene);
    void change_scene(unsigned int id);
    void push_layer(Layer* layer, Scene* scene);

    void add_framebuffer(std::shared_ptr<Framebuffer> framebuffer);
    void purge_framebuffers();

    static int get_width();
    static int get_height();

    bool running = true;
    double fps = 0.0;
    ApplicationData data;
    std::shared_ptr<AssetsLoad> assets_load;
    std::shared_ptr<Window> window;
    renderer::Storage* storage = nullptr;
private:
    void on_event(events::Event& event);
    float update_frame_counter();

    bool on_window_closed(events::WindowClosedEvent& event);
    bool on_window_resized(events::WindowResizedEvent& event);

    std::vector<Scene*> scenes;
    Scene* current_scene = nullptr;

    bool changed_scene = false;
    Scene* to_scene = nullptr;

    std::vector<std::weak_ptr<Framebuffer>> framebuffers;

    static int* width;
    static int* height;

    friend class Layer;
};
