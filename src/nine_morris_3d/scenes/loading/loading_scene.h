#pragma once

#include "application/scene.h"
#include "nine_morris_3d/options.h"

class LoadingScene : public Scene {
public:
    LoadingScene(unsigned int id)
        : Scene(id) {}
    virtual ~LoadingScene() = default;

    virtual void on_enter() override;
    virtual void on_exit() override;

    options::Options options;
};
