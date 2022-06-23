#pragma once

class Application;
class Layer;

/**
 * Class representing a scene and containing all the layers in that scene.
 */
class Scene {
public:
    Scene(const std::string& id)
        : id(id) {}
    ~Scene() = default;

    void add_layer(Layer* layer) {
        layers_in_order.push_back(layer);
    }

    void add_overlay(Layer* layer) {
        overlays_in_order.push_back(layer);
    }
private:
    std::string id;
    std::vector<Layer*> layers_in_order;
    std::vector<Layer*> overlays_in_order;

    friend class Application;
    friend class Layer;
};
