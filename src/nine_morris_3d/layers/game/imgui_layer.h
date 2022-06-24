#pragma once

#include <imgui.h>

#include "application/layer.h"
#include "application/platform.h"

class GameLayer;
class GuiLayer;

class ImGuiLayer : public Layer {
public:
    ImGuiLayer(const std::string& id, Application* app)
        : Layer(id, app) {}
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
    void draw_game_over_message(const char* message1, std::string_view message2);
    void draw_about();
    void draw_could_not_load_game();
    void draw_no_last_game();

#ifdef PLATFORM_GAME_DEBUG
    void draw_debug(float dt);
#endif

    int get_texture_quality_option(std::string_view option);
    int get_skybox_option(std::string_view option);

    bool hovering_gui = false;
    bool can_undo = false;
    bool can_redo = false;
    bool show_info = false;
    bool show_about = false;
    bool show_could_not_load_game = false;
    bool show_no_last_game = false;
    std::string last_save_game_date;

    std::string info_file_path;
    std::string save_game_file_path;

    GameLayer* game_layer = nullptr;
    GuiLayer* gui_layer = nullptr;
};
