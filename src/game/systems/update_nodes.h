#pragma once

#include <entt/entt.hpp>
#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

struct UpdateNodesSystem : System<> {
    virtual void run() override;
};
