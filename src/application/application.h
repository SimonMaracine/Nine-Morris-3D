#pragma once

#include <memory>
#include <string>
#include <vector>

#include "application/application_data.h"
#include "application/events.h"
#include "application/window.h"
#include "opengl/renderer/renderer.h"
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

    void run();

    void add_scene(Scene* scene);
    void set_starting_scene(Scene* scene);
    void change_scene(unsigned int id);
    void push_layer(Layer* layer, Scene* scene);
    // void add_asset(unsigned int id, AssetType type, const std::string& file_path);

    bool running = true;
    double fps = 0.0;
    ApplicationData data;
    // AssetManager asset_manager;
    std::shared_ptr<AssetsLoad> assets_load;
    std::shared_ptr<Window> window;
    renderer::Storage* storage;
private:
    void on_event(events::Event& event);
    float update_frame_counter();

    bool on_window_closed(events::WindowClosedEvent& event);
    bool on_window_resized(events::WindowResizedEvent& event);

    std::vector<Scene*> scenes;
    Scene* current_scene = nullptr;

    bool changed_scene = false;
    Scene* to_scene;

    friend class Layer;
};
