#pragma once

#include "application/layer.h"
#include "nine_morris_3d/scenes/game/game_scene.h"

class GameLayer;
class GuiLayer;

class ImGuiLayer : public Layer {
public:
    ImGuiLayer(unsigned int id, Application* application, GameScene* scene)
        : Layer(id, application), scene(scene) {};
    virtual ~ImGuiLayer() = default;

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

    bool hovering_gui = false;
    bool can_undo = false;
    bool show_info = false;

    GameScene* scene = nullptr;
    GameLayer* game_layer = nullptr;
    GuiLayer* gui_layer = nullptr;
};
