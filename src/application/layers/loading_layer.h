#pragma once

#include "application/layer.h"

class GameLayer;
class ImGuiLayer;
class GuiLayer;

class LoadingLayer : public Layer {
public:
    LoadingLayer(unsigned int id, Application* application)
        : Layer(id, application) {};
    virtual ~LoadingLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_bind_layers() override;
    virtual void on_update(float dt) override;
    virtual void on_draw() override;
    virtual void on_event(events::Event& event) override;

    Loader loader = Loader(application->asset_manager);

    GameLayer* game_layer;
    ImGuiLayer* imgui_layer;
    GuiLayer* gui_layer;
};
