#pragma once

#include <vector>

#include <entt/entt.hpp>

class Layer;

class Scene {
public:
    Scene(unsigned int id)
        : id(id) {}
    virtual ~Scene() = default;

    virtual void on_enter() {}
    virtual void on_exit() {}

    entt::registry registry;
private:
    unsigned int id;
    std::vector<Layer*> layer_stack;

    friend class Application;
    friend class Layer;
};
