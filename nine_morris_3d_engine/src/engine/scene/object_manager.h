#pragma once

#include <resmanager/resmanager.h>

#include "engine/scene/object.h"

class ObjectManager {
private:
    using Id = resmanager::HashedStr64;
public:
    ObjectManager() = default;
    ~ObjectManager();

    ObjectManager(const ObjectManager&) = delete;
    ObjectManager& operator=(const ObjectManager&) = delete;
    ObjectManager(ObjectManager&&) = delete;
    ObjectManager& operator=(ObjectManager&&) = delete;

    template<typename T, typename... Args>
    T* add(Id id, Args&&... args);

    void remove(Id id);

    template<typename T>
    T* get(Id id);

    void clear();
private:
    void destroy();

    std::unordered_map<Id, Object*, resmanager::Hash<Id>> objects;
};

template<typename T, typename... Args>
T* ObjectManager::add(Id id, Args&&... args) {
    T* new_object = new T(std::forward<Args>(args)...);
    objects[id] = new_object;

    return new_object;
}

template<typename T>
T* ObjectManager::get(Id id) {
    return static_cast<T*>(objects.at(id));
}
