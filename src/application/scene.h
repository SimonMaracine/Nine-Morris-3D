#pragma once

#include <vector>
#include <string>
#include <utility>

class Application;
class Layer;

class Scene {
public:
    Scene(std::string&& id)
        : id(std::move(id)) {}
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
