#pragma once

#include "nine_morris_3d_engine/application/events.h"
#include "nine_morris_3d_engine/ecs/system.h"

class Application;
// class Layer;

/**
 * Class representing a scene in the game.
 */
class Scene {
public:
    Scene(std::string_view name)
        : name(name) {}
    virtual ~Scene() = default;

    // void add_layer(Layer* layer) {
    //     layers_in_order.push_back(layer);
    // }

    // void add_overlay(Layer* layer) {
    //     overlays_in_order.push_back(layer);
    // }

    void add_system(std::unique_ptr<System<>>&& system);

    virtual void on_start(Application* app) {}  // Called when the scene is started
    virtual void on_stop(Application* app) {}  // Called when the scene is stopped
    virtual void on_awake(Application* app) {}  // Called once before on_start is called for the first time
    virtual void on_update(Application* app) {}  // Called every frame
    virtual void on_fixed_update(Application* app) {}  // Called once every 50 milliseconds
    virtual void on_imgui_update(Application* app) {}  // Called every frame for ImGui only
    // virtual void on_event(Application* app, events::Event& event) {}  // Called on every event
private:
    std::string name;
    bool on_awake_called = false;
    // std::vector<Layer*> layers_in_order;
    // std::vector<Layer*> overlays_in_order;

    // All user defined traditional systems
    std::vector<std::unique_ptr<System<>>> systems;

    friend class Application;
    // friend class Layer;
};
