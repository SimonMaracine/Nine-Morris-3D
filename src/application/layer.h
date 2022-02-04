#pragma once

#include <cassert>

#include "application/events.h"
#include "application/application.h"
#include "application/scene.h"

class Application;

class Layer {
public:
    Layer(unsigned int id)
        : id(id) {}
    virtual ~Layer() = default;

    virtual void on_attach() {}
    virtual void on_detach() {}
    virtual void on_bind_layers() {}
    virtual void on_update(float dt) {}
    virtual void on_fixed_update() {}
    virtual void on_draw() {}
    virtual void on_event(events::Event& event) {}

    bool active = true;
protected:
    template<typename T>
    T* get_layer(unsigned int id, Scene* scene) const {
        for (unsigned int i = 0; i < scene->layer_stack.size(); i++) {
            if (scene->layer_stack[i]->id == id) {
                return static_cast<T*>(scene->layer_stack[i]);
            }
        }

        assert(false);
        return nullptr;
    }
private:
    unsigned int id = 0;
};
