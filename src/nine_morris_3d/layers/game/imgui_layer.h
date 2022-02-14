#pragma once

#include <string>
#include <utility>

#include <imgui.h>

#include "application/layer.h"

class GameLayer;
class GuiLayer;

class ImGuiLayer : public Layer {
public:
    ImGuiLayer(std::string&& id, Application* app)
        : Layer(std::move(id), app) {}
    virtual ~ImGuiLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_bind_layers() override;
    virtual void on_update(float dt) override;
    virtual void on_event(events::Event& event) override;

    bool on_mouse_scrolled(events::MouseScrolledEvent& event);
    bool on_mouse_moved(events::MouseMovedEvent& event);
    bool on_mouse_button_pressed(events::MouseButtonPressedEvent& event);
    bool on_mouse_button_released(events::MouseButtonReleasedEvent& event);
    bool on_window_resized(events::WindowResizedEvent& event);

    void draw_game_over();
    void draw_about_screen();
    void draw_debug(float dt);

    bool hovering_gui = false;
    bool can_undo = false;
    bool show_info = false;
    bool about_mode = false;
    std::string last_save_date;

    ImFont* info_font = nullptr;
    ImFont* windows_font = nullptr;

    GameLayer* game_layer = nullptr;
    GuiLayer* gui_layer = nullptr;
};
