#pragma once

#include <memory>

#include "application/layer.h"
#include "application/events.h"
#include "other/texture_data.h"
#include "other/loader.h"
#include "nine_morris_3d/scenes/loading/loading_scene.h"
#include "nine_morris_3d/assets_load.h"

class LoadingLayer : public Layer {
public:
    LoadingLayer(unsigned int id, Application* application, LoadingScene* scene)
        : Layer(id, application), scene(scene) {}
    virtual ~LoadingLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_bind_layers() override;
    virtual void on_update(float dt) override;
    virtual void on_draw() override;
    virtual void on_event(events::Event& event) override;

    bool on_window_resized(events::WindowResizedEvent& event);

    std::unique_ptr<Loader<AssetsLoad>> loader;

    LoadingScene* scene = nullptr;
};
