#pragma once

#include <vector>

class Application;
class Layer;

class Scene {
public:
    Scene(unsigned int id, Application* application)
        : app(application), id(id) {}
    virtual ~Scene() = default;

    virtual void on_enter() {}
    virtual void on_exit() {}
protected:
    Application* app;
private:
    unsigned int id;
    std::vector<Layer*> layer_stack;

    friend class Application;
    friend class Layer;
};
