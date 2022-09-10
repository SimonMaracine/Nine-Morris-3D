#pragma once

class Application;

/**
 * Class representing an entire scene in the game.
 */
class Scene {
public:
    Scene(std::string_view name)
        : name(name) {}
    virtual ~Scene() = default;

    virtual void on_start() {}  // Called when the scene is started
    virtual void on_stop() {}  // Called when the scene is stopped
    virtual void on_awake() {}  // Called once before on_start is called for the first time
    virtual void on_update() {}  // Called every frame
    virtual void on_fixed_update() {}  // Called once every 50 milliseconds
    virtual void on_imgui_update() {}  // Called every frame for ImGui only
protected:
    Application* app = nullptr;
private:
    std::string name;
    bool on_awake_called = false;

    friend class Application;
};
