#pragma once

#include "application/layer.h"
#include "application/events.h"
#include "other/loader.h"
#include "nine_morris_3d/scenes/loading/loading_scene.h"

class GameLayer;
class ImGuiLayer;
class GuiLayer;

class LoadingLayer : public Layer {
public:
    LoadingLayer(unsigned int id, Application* application, LoadingScene* scene)
        : Layer(id, application), scene(scene) {};
    virtual ~LoadingLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_bind_layers() override;
    virtual void on_update(float dt) override;
    virtual void on_draw() override;
    virtual void on_event(events::Event& event) override;

    bool on_window_resized(events::WindowResizedEvent& event);

    Loader loader = Loader(app->asset_manager);

    LoadingScene* scene;
};
