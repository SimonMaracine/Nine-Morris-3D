#pragma once

#include <vector>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/standard_board.hpp"
#include "scenes/game_scene.hpp"
#include "point_camera_controller.hpp"
#include "turn_indicator.hpp"
#include "timer.hpp"
#include "ui.hpp"

class StandardGameScene : public sm::ApplicationScene, public GameScene {
public:
    explicit StandardGameScene(sm::Ctx& ctx)
        : sm::ApplicationScene(ctx) {}

    SM_SCENE_NAME("game")

    void on_start() override;
    void on_stop() override;
    void on_update() override;
    void on_fixed_update() override;
    void on_imgui_update() override;

    PointCameraController& get_camera_controller() override;
    void load_and_set_skybox() override;
    void load_and_set_board_paint_texture() override;
private:
    void on_window_resized(const sm::WindowResizedEvent& event);
    void on_key_released(const sm::KeyReleasedEvent& event);
    void on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event);
    void on_mouse_button_released(const sm::MouseButtonReleasedEvent& event);

    void setup_camera();
    void setup_skybox();
    void setup_lights();
    void setup_turn_indicator();
    void setup_timer();
    void setup_renderables();
    sm::Renderable setup_board() const;
    sm::Renderable setup_board_paint() const;
    std::vector<sm::Renderable> setup_nodes() const;
    std::vector<sm::Renderable> setup_white_pieces() const;
    std::vector<sm::Renderable> setup_black_pieces() const;

    void load_skybox() const;
    void load_board_paint_texture() const;

    std::shared_ptr<sm::GlTextureCubemap> get_skybox_texture_cubemap() const;
    std::shared_ptr<sm::GlTexture> get_board_paint_texture(const sm::TextureSpecification& specification) const;

    // void setup_ground(std::shared_ptr<sm::Mesh> mesh);
    // void setup_dragon(std::shared_ptr<sm::Mesh> mesh);
    // void setup_teapot(std::shared_ptr<sm::Mesh> mesh);
    // void setup_cube(std::shared_ptr<sm::Mesh> mesh);
    // void setup_brick(std::shared_ptr<sm::Mesh> mesh, std::shared_ptr<sm::TextureData> texture_data);
    // void setup_lamp(std::shared_ptr<sm::Mesh> mesh_stand, std::shared_ptr<sm::TextureData> texture_data_stand, std::shared_ptr<sm::Mesh> mesh_bulb);
    // void setup_barrel(std::shared_ptr<sm::Mesh> mesh, std::shared_ptr<sm::TextureData> texture_data_diffuse, std::shared_ptr<sm::TextureData> texture_data_normal);
    // void setup_textured_bricks(std::shared_ptr<sm::Mesh> mesh, std::shared_ptr<sm::TextureData> texture_data);
    // void setup_texts();
    // void setup_quads();

    // void setup_sounds();

    // void reload_textures(bool srgb);
    // void load_heavy_resources();

    bool m_game_started {false};
    glm::vec3 m_default_camera_position {};

    sm::Camera m_cam;
    sm::Camera2D m_cam_2d;
    PointCameraController m_cam_controller;
    sm::DirectionalLight m_directional_light;
    // sm::PointLight point_light;
    sm::ShadowBox m_shadow_box;

    StandardBoard m_board;
    TurnIndicator m_turn_indicator;
    Timer m_timer;

    // sm::Renderable ground;
    // sm::Renderable dragon1;
    // sm::Renderable dragon2;
    // sm::Renderable teapot;
    // sm::Renderable cube;
    // sm::Renderable brick;
    // sm::Renderable lamp_stand;
    // sm::Renderable lamp_bulb;
    // sm::Renderable barrel;
    // std::vector<sm::Renderable> textured_bricks;

    // sm::Text text1;
    // sm::Text text2;
    // sm::Text text3;
    // sm::Text text4;
    // sm::Quad wait;
    // sm::Quad white;

    std::shared_ptr<sm::GlTextureCubemap> m_skybox;

    // std::shared_ptr<sm::Font> sans;
    // std::shared_ptr<sm::AlSource> emitter;
    // std::shared_ptr<sm::AlBuffer> sound_move;
    // std::shared_ptr<sm::AlBuffer> sound_place;
    // std::shared_ptr<sm::MusicTrack> relaxing;

    // bool music_playing {false};

    // bool sky {true};
    // bool blur {false};
    // bool outline {true};
    // bool color_correction {true};

#if 0
    // TODO
    sm::Renderable temp;
#endif

    Ui m_ui;
};
