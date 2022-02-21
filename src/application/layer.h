#pragma once

#include <cassert>
#include <string>
#include <utility>

#include "application/application.h"
#include "application/events.h"
#include "application/scene.h"

/**
 * Abstract class representing a part of a scene.
 * Subclass this and implement all the game logic there.
 */
class Layer {
public:
    Layer(std::string&& id, Application* application)
        : id(std::move(id)), application(application) {}
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
    T* get_layer(const std::string& id) const {
        for (Scene* scene : application->scenes) {
            for (Layer* layer : scene->layers_in_order) {
                if (layer->id == id) {
                    return static_cast<T*>(layer);
                }
            }
        }

        assert(false);
        return nullptr;
    }
private:
    std::string id;
    Application* application = nullptr;
};
