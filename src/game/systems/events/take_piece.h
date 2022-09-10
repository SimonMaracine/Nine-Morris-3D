#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

struct TakePieceEvent {
    hover::Id hovered_id;
};

struct TakePieceSystem : System<TakePieceEvent> {
    virtual void run_once(TakePieceEvent& e) override;
};
