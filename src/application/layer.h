#pragma once

#include "application/events.h"
#include "application/application.h"

class Application;

class Layer {
public:
    Layer(unsigned int id, Application* application)
        : application(application), id(id) {}
    virtual ~Layer() = default;

    virtual void on_attach() {}
    virtual void on_detach() {}
    virtual void on_bind_layers() {}
    virtual void on_update(float dt) {}
    virtual void on_draw() {}
    virtual void on_event(events::Event& event) {}

    bool active = true;
protected:
    template<typename T>
    T* get_layer(unsigned int id) {
        for (unsigned int i = 0; i < application->layer_stack.size(); i++) {
            if (application->layer_stack[i]->id == id) {
                return (T*) application->layer_stack[i];
            }
        }

        assert(false);
    }

    Application* application;
private:
    unsigned int id;
};
