#pragma once

#include "application/layer.h"
#include "application/scenes/loading/loading_scene.h"
#include "other/loader.h"

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

    Loader loader = Loader(app->asset_manager);

    LoadingScene* scene;
};
