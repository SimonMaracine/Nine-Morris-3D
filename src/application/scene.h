#pragma once

#include <vector>
#include <string>
#include <utility>

class Application;
class Layer;

/**
 * Class representing a scene and containing all the layers in that scene.
 * Add layers to scenes and add scenes to application. Currrent scene must be set before run().
 */
class Scene {
public:
    Scene(std::string&& id)
        : id(std::move(id)) {}
    ~Scene() = default;

    void add_layer(Layer* layer) {
        layers_in_order.push_back(layer);
    }
private:
    std::string id;
    std::vector<Layer*> layers_in_order;

    friend class Application;
    friend class Layer;
};
