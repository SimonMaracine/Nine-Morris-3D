#pragma once

#include <entt/entt.hpp>

class Application;

struct _DummyEvent {};

template<typename E = _DummyEvent>
struct System {
public:
    System(entt::registry& registry, Application* app)
        : registry(registry), app(app) {}

    virtual void run() {}
    virtual void run_once(E& e) {}
    virtual void signal() {}
protected:
    entt::registry& registry;
    Application* app;
};
