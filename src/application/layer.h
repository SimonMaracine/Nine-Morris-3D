#pragma once

#include "application/application.h"
#include "application/events.h"
#include "application/scene.h"
#include "other/assert.h"

/**
 * Abstract class representing a part of a scene.
 * Subclass this and implement all the game logic there.
 */
class Layer {
public:
    Layer(const std::string& id, Application* application)
        : id(id), application(application) {}
    virtual ~Layer() = default;

    virtual void on_attach() {}  // Called when the layer is attached
    virtual void on_detach() {}  // Called when the layer is detached
    virtual void on_awake() {}  // Called once when on_attach is called for the first time
    virtual void on_update(float dt) {}  // Called every frame
    virtual void on_fixed_update() {}  // Called once every 50 milliseconds
    virtual void on_event(events::Event& event) {}  // Called on every event

    bool active = true;
protected:
    template<typename T>
    T* get_layer(std::string_view id) const {
        for (Scene* scene : application->scenes) {
            for (Layer* layer : scene->layers_in_order) {
                if (layer->id == id) {
                    return static_cast<T*>(layer);
                }
            }

            for (Layer* layer : scene->overlays_in_order) {
                if (layer->id == id) {
                    return static_cast<T*>(layer);
                }
            }
        }

        ASSERT(false, "Layer not found");
        return nullptr;
    }
private:
    std::string id;
    Application* application = nullptr;
    bool on_awake_called = false;

    friend class Application;
};
