#pragma once

#include "application/layer.h"

class GameLayer;

#define RESET_HOVERING_GUI hovering_gui = false
#define HOVERING_GUI hovering_gui = true

#define DEFAULT_BROWN ImVec4(0.6f, 0.35f, 0.12f, 1.0f)
#define DARK_BROWN ImVec4(0.4f, 0.25f, 0.10f, 1.0f)
#define LIGHT_BROWN ImVec4(0.68f, 0.42f, 0.12f, 1.0f)

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
    bool can_undo = false;
    GameLayer* game_layer;
};
