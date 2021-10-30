#pragma once

#include "application/scene.h"
#include "application/application.h"
#include "nine_morris_3d/options.h"

class LoadingScene : public Scene {
public:
    LoadingScene(unsigned int id, Application* application)
        : Scene(id, application) {}
    virtual ~LoadingScene() = default;

    virtual void on_enter() override;
    virtual void on_exit() override;

    options::Options options;
};
