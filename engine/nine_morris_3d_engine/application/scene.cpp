#include "nine_morris_3d_engine/application/scene.h"
#include "nine_morris_3d_engine/ecs/system.h"

void Scene::add_system(std::unique_ptr<System<>>&& system) {
    systems.push_back(std::move(system));
}
