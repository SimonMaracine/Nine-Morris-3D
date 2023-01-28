#pragma once

#include <resmanager/resmanager.h>

class Application;

/**
 * Class representing an entire scene in the game.
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

    virtual void on_start() {}  // Called when the scene is started
    virtual void on_stop() {}  // Called when the scene is stopped
    virtual void on_awake() {}  // Called once before on_start is called for the first time
    virtual void on_update() {}  // Called every frame
    virtual void on_fixed_update() {}  // Called once every 50 milliseconds
    virtual void on_imgui_update() {}  // Called every frame for ImGui only

    std::string_view get_name() const { return name; }
    SceneId get_id() const { return id; }
protected:
    Application* app = nullptr;
private:
    const std::string name;
    const SceneId id;
    bool on_awake_called = false;

    friend class Application;
};
