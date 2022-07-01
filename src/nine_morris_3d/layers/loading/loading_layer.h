#pragma once

#include "application/layer.h"
#include "nine_morris_3d/assets_data.h"
#include "nine_morris_3d/options.h"
#include "other/loader.h"

class LoadingLayer : public Layer {
public:
    LoadingLayer(std::string_view id, Application* app)
        : Layer(id, app) {}
    virtual ~LoadingLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_update(float dt) override;

    std::unique_ptr<Loader<AssetsData, options::Options>> loader;
};
