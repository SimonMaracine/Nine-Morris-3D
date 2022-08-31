#pragma once

#include <entt/entt.hpp>
#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

struct UpdateCursorEvent {};

struct UpdateCursorSystem : System<UpdateCursorEvent> {
    virtual void run_once(UpdateCursorEvent& e) override;
};
