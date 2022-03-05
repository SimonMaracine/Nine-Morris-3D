#pragma once

#include <string>
#include <utility>

#include "application/layer.h"
#include "nine_morris_3d/turn_indicator.h"
#include "nine_morris_3d/timer.h"

class GameLayer;

class GuiLayer : public Layer {
public:
    GuiLayer(std::string&& id, Application* app)
        : Layer(std::move(id), app) {}
    virtual ~GuiLayer() = default;

    virtual void on_attach() override;
    virtual void on_awake() override;
    virtual void on_update(float dt) override;

    void render_turn_indicator();
    void render_timer();

    TurnIndicator turn_indicator;
    Timer timer;

    GameLayer* game_layer = nullptr;
};
