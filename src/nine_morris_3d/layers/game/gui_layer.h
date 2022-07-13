#pragma once

#include "application/layer.h"
#include "nine_morris_3d/timer.h"

class GameLayer;

class GuiLayer : public Layer {
public:
    GuiLayer(std::string_view id, Application* app)
        : Layer(id, app) {}
    virtual ~GuiLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_awake() override;
    virtual void on_update(float dt) override;

    Timer timer;
    bool show_wait_indicator = false;
    bool show_computer_thinking_indicator = false;

    std::shared_ptr<gui::Image> turn_indicator;
    std::shared_ptr<gui::Text> timer_text;
    std::shared_ptr<gui::Image> wait_indicator;
    std::shared_ptr<gui::Image> computer_thinking_indicator;
    std::shared_ptr<Texture> t;

    GameLayer* game_layer = nullptr;
};
