#pragma once

#include "application/layer.h"
#include "nine_morris_3d/scenes/game/game_scene.h"

class GameLayer;

class GuiLayer : public Layer {
public:
    GuiLayer(unsigned int id, Application* application, GameScene* scene)
        : Layer(id, application), scene(scene) {};
    virtual ~GuiLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_bind_layers() override;
    virtual void on_update(float dt) override;
    virtual void on_draw() override;
    virtual void on_event(events::Event& event) override;

    bool on_mouse_scrolled(events::MouseScrolledEvent& event);
    bool on_mouse_moved(events::MouseMovedEvent& event);
    bool on_mouse_button_pressed(events::MouseButtonPressedEvent& event);
    bool on_mouse_button_released(events::MouseButtonReleasedEvent& event);
    bool on_window_resized(events::WindowResizedEvent& event);

    void imgui_start();
    void imgui_update(float dt);
    void imgui_end();

    GameScene* scene;

    GameLayer* game_layer;
};
