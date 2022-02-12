#pragma once

#include <string>
#include <memory>
#include <utility>

#include "application/layer.h"
#include "application/events.h"
#include "nine_morris_3d/assets_data.h"
#include "other/texture_data.h"
#include "other/loader.h"

class LoadingLayer : public Layer {
public:
    LoadingLayer(std::string&& id, Application* app)
        : Layer(std::move(id), app) {}
    virtual ~LoadingLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_update(float dt) override;
    virtual void on_draw() override;

    std::unique_ptr<Loader<AssetsData>> loader;
};
