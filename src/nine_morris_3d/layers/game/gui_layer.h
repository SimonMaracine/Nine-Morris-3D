#pragma once

#include "application/layer.h"
#include "nine_morris_3d/timer.h"

class GameLayer;

class GuiLayer : public Layer {
public:
    GuiLayer(std::string&& id, Application* app)
        : Layer(std::move(id), app) {}
    virtual ~GuiLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_awake() override;
    virtual void on_update(float dt) override;

    void render_turn_indicator();
    void render_timer();

    Timer timer;

    std::shared_ptr<gui::Image> turn_indicator;
    std::shared_ptr<gui::Text> timer_text;

    GameLayer* game_layer = nullptr;
};
