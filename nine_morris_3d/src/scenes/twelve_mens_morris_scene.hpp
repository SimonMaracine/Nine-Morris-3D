#pragma once

#include "nine_mens_morris_base_scene.hpp"

struct TwelveMensMorrisScene : NineMensMorrisBaseScene {
    explicit TwelveMensMorrisScene(sm::Ctx& ctx)
        : NineMensMorrisBaseScene(ctx) {}

    SM_SCENE_NAME("twelve_mens_morris")

    bool twelve_mens_morris() const override { return true; }
};
