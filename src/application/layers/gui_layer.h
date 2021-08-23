#pragma once

#include "application/layer.h"
#include "application/layers/game_layer.h"

#define HOVERING_GUI hovering_gui = true

class GuiLayer : public Layer {
public:
    GuiLayer(unsigned int id, Application* application)
        : Layer(id, application) {};
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

    bool hovering_gui = false;
    GameLayer* game_layer;
};
