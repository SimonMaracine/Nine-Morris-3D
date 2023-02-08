#include <resmanager/resmanager.h>

#include "engine/scene/object_manager.h"

ObjectManager::~ObjectManager() {
    destroy();
}

void ObjectManager::remove(Id id) {
    delete objects.at(id);
    objects.erase(id);
}

void ObjectManager::clear() {
    destroy();
    objects.clear();
}

void ObjectManager::destroy() {
    for (const auto& [_, object] : objects) {
        delete object;
    }
}
