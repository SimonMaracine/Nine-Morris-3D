#pragma once

#include <string>

#include "application/application.h"
#include "application/layer.h"
#include "application/events.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/buffer.h"
#include "opengl/renderer/renderer.h"
#include "other/model.h"
#include "other/loader.h"
#include "nine_morris_3d/scenes/game/game_scene.h"

class ImGuiLayer;

class GameLayer : public Layer {
public:
    GameLayer(unsigned int id, Application* application, GameScene* scene)
        : Layer(id, application), scene(scene) {};
    virtual ~GameLayer() = default;

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

    void render_skybox();
    void setup_light();
    void setup_camera();
    void setup_board();
    void setup_board_paint();
    void setup_pieces();
    void render_pieces();
    void render_nodes();
    void render_to_depth();
    void setup_shadows();
    void setup_quad2d_projection();
    void setup_quad3d_projection_view();

    void set_scene_framebuffer(int samples);
    void set_textures_quality(const std::string& quality);
    void set_skybox(const std::string& skybox);
    void load_game();

    float mouse_wheel = 0.0f;
    float dx = 0.0f;
    float dy = 0.0f;
    float last_mouse_x = 0.0f;
    float last_mouse_y = 0.0f;

    GameScene* scene = nullptr;

    ImGuiLayer* imgui_layer = nullptr;
};
