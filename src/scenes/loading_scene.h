#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/options.h"

struct LoadingScene : public Scene {
    LoadingScene()
        : Scene("loading") {}

    virtual void on_start() override;
    virtual void on_stop() override;
    virtual void on_update() override;

    std::unique_ptr<ConcurrentLoader<options::Options>> loader;
};
