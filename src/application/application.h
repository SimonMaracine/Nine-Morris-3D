#pragma once

#include <memory>
#include <vector>

#include <entt/entt.hpp>

#include "application/application_data.h"
#include "application/events.h"
#include "application/window.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/buffer.h"
#include "opengl/renderer/framebuffer.h"
#include "opengl/renderer/renderer.h"
#include "other/model.h"
#include "other/loader.h"
#include "ecs_and_game/game.h"

constexpr int VERSION_MAJOR = 0;
constexpr int VERSION_MINOR = 1;
constexpr int VERSION_PATCH = 0;

class Layer;

class Application {
public:
    Application(int width, int height);
    ~Application();

    void run();
    void on_event(events::Event& event);

    float update_frame_counter();

    void push_layer(Layer* layer);

    bool running = true;
    std::shared_ptr<Window> window = nullptr;
    ApplicationData data;
    std::vector<Layer*> layer_stack;
    double fps = 0.0;

    bool on_window_closed(events::WindowClosedEvent& event);
    bool on_window_resized(events::WindowResizedEvent& event);
};
