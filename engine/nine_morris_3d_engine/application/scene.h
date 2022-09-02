#pragma once

#include "nine_morris_3d_engine/application/events.h"
#include "nine_morris_3d_engine/ecs/system.h"

class Application;

/**
 * Class representing a scene in the game.
 */
class Scene {
public:
    Scene(std::string_view name)
        : name(name) {}
    virtual ~Scene() = default;

    void add_system(std::unique_ptr<System<>>&& system);

    virtual void on_start(Application* app) {}  // Called when the scene is started
    virtual void on_stop(Application* app) {}  // Called when the scene is stopped
    virtual void on_awake(Application* app) {}  // Called once before on_start is called for the first time
    virtual void on_update(Application* app) {}  // Called every frame
    virtual void on_fixed_update(Application* app) {}  // Called once every 50 milliseconds
    virtual void on_imgui_update(Application* app) {}  // Called every frame for ImGui only
private:
    std::string name;
    bool on_awake_called = false;

    // All user defined traditional systems
    std::vector<std::unique_ptr<System<>>> systems;

    friend class Application;
};
