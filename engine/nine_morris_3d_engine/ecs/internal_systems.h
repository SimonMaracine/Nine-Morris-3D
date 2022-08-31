#pragma once

#include <entt/entt.hpp>

#include "nine_morris_3d_engine/ecs/system.h"

class Application;

struct CameraSystem : System<> {
    virtual void run() override;
};

struct CameraProjectionSystem : System<> {
    virtual void run() override;
};

struct ModelRenderSystem : System<> {
    virtual void signal() override;
};

struct QuadRenderSystem : System<> {
    virtual void signal() override;
};

struct GuiImageSystem : System<> {
    virtual void signal() override;
};

struct GuiTextSystem : System<> {
    virtual void signal() override;
};
