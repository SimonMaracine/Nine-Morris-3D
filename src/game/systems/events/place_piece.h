#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

struct PlacePieceEvent {
    hover::Id hovered_id;
};

struct PlacePieceSystem : System<PlacePieceEvent> {
    virtual void run_once(PlacePieceEvent& e) override;
};
