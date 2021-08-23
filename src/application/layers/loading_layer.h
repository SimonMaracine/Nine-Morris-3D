#pragma once

#include "application/layer.h"
#include "application/layers/game_layer.h"

class LoadingLayer : public Layer {
public:
    LoadingLayer(unsigned int id, Application* application)
        : Layer(id, application) {};
    virtual ~LoadingLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_update(float dt) override;
    virtual void on_draw() override;
    virtual void on_event(events::Event& event) override;

    GameLayer* game_layer;
};
