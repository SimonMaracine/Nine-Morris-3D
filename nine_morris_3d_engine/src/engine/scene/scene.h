#pragma once

#include <resmanager/resmanager.h>

#include "engine/application/event.h"
#include "engine/scene/scene_list.h"
#include "engine/scene/object_manager.h"

class Application;

class Layer {
private:
    using OnEvent = std::function<bool(event::Event&)>;
    using OnUpdate = std::function<void()>;
public:
    OnEvent on_event;  // Called for every event
    OnUpdate on_update;  // Called every frame
    OnUpdate on_fixed_update;  // Called once every 50 milliseconds
    OnUpdate on_imgui_update;  // Called every frame for ImGui only
};

/**
 * Class representing an entire scene of a game. Doesn't need to be heap-allocated.
 */
class Scene {
private:
    using SceneId = resmanager::HashedStr64;
public:
    Scene(const std::string& name)
        : name(name), id(SceneId(name)) {}
    virtual ~Scene() = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;

    Layer& layer(size_t index) { return layer_stack.at(index); }

    virtual void on_bind() = 0;  // Called at initialization for defining layer updates
    virtual void on_start() {}  // Called when the scene is entered
    virtual void on_stop() {}  // Called when the scene is exited
    virtual void on_awake() {}  // Called once before on_start is called for the first time
    // virtual void on_update() {}  // Called every frame
    // virtual void on_fixed_update() {}  // Called once every 50 milliseconds
    // virtual void on_imgui_update() {}  // Called every frame for ImGui only

    std::string_view get_name() const { return name; }
    SceneId get_id() const { return id; }

    SceneList scene_list;  // List of all objects to be rendered
    ObjectManager objects;  // Storage for all the renderable objects

    Application* app = nullptr;
private:
    inline void _on_stop() {  // Used to clean up objects
        scene_list.clear();
        objects.clear();
    }

    std::string name;
    SceneId id;
    bool on_awake_called = false;
    std::array<Layer, 4> layer_stack;

    friend class Application;
};
