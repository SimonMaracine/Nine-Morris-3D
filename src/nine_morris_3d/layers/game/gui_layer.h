#pragma once

#include <memory>

#include "application/layer.h"
#include "nine_morris_3d/scenes/game/game_scene.h"

class GuiLayer : public Layer {
public:
    GuiLayer(unsigned int id, GameScene* scene)
        : Layer(id), scene(scene) {}
    virtual ~GuiLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_bind_layers() override;
    virtual void on_update(float dt) override;
    virtual void on_draw() override;
    virtual void on_event(events::Event& event) override;

    void render_turn_indicator();
    void render_timer();

    GameScene* scene = nullptr;
};
