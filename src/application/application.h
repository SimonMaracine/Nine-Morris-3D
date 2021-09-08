#pragma once

#include <memory>
#include <vector>
#include <string>

#include "application/application_data.h"
#include "application/events.h"
#include "application/window.h"
#include "other/asset_manager.h"

constexpr int VERSION_MAJOR = 0;
constexpr int VERSION_MINOR = 1;
constexpr int VERSION_PATCH = 0;

class Layer;

class Application {
public:
    Application(int width, int height, const std::string& title);
    ~Application();

    void run();
    void on_event(events::Event& event);

    float update_frame_counter();
    void push_layer(Layer* layer);
    void add_asset(unsigned int id, const std::string& file_path);

    bool running = true;
    double fps = 0.0;
    ApplicationData data;
    AssetManager asset_manager;
    std::shared_ptr<Window> window = nullptr;
private:
    bool on_window_closed(events::WindowClosedEvent& event);
    bool on_window_resized(events::WindowResizedEvent& event);

    std::vector<Layer*> layer_stack;

    friend class Layer;
};
