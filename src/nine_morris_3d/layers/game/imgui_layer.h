#pragma once

#include <imgui.h>

#include "application/layer.h"
#include "application/platform.h"

class GameLayer;
class GuiLayer;

class ImGuiLayer : public Layer {
public:
    ImGuiLayer(std::string&& id, Application* app)
        : Layer(std::move(id), app) {}
    virtual ~ImGuiLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_awake() override;
    virtual void on_update(float dt) override;
    virtual void on_event(events::Event& event) override;

    bool on_mouse_scrolled(events::MouseScrolledEvent& event);
    bool on_mouse_moved(events::MouseMovedEvent& event);
    bool on_mouse_button_pressed(events::MouseButtonPressedEvent& event);
    bool on_mouse_button_released(events::MouseButtonReleasedEvent& event);

    void draw_game_over();
    void draw_game_over_message(const char* message);
    void draw_about_screen();

#ifdef NINE_MORRIS_3D_DEBUG
    void draw_debug(float dt);
#endif

    bool hovering_gui = false;
    bool can_undo = false;
    bool show_info = false;
    bool about_mode = false;
    std::string last_save_date;

    GameLayer* game_layer = nullptr;
    GuiLayer* gui_layer = nullptr;
};
