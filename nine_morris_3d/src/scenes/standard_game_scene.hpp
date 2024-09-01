#pragma once

#include <vector>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/standard_board.hpp"
#include "scenes/game_scene.hpp"
#include "point_camera_controller.hpp"
#include "turn_indicator.hpp"
#include "timer.hpp"
#include "ui.hpp"

struct StandardGameScene : sm::ApplicationScene, GameScene {
    explicit StandardGameScene(sm::Ctx& ctx)
        : sm::ApplicationScene(ctx) {}

    SM_SCENE_NAME("game")

    void on_start() override;
    void on_stop() override;
    void on_update() override;
    void on_fixed_update() override;
    void on_imgui_update() override;

    PointCameraController& get_camera_controller() override;

    void on_window_resized(const sm::WindowResizedEvent& event);
    void on_key_released(const sm::KeyReleasedEvent& event);
    void on_mouse_button_released(const sm::MouseButtonReleasedEvent& event);

    void setup_camera();
    void setup_skybox();
    void setup_lights();
    void setup_turn_indicator();
    void setup_timer();
    void setup_renderables();
    sm::Renderable setup_board();
    sm::Renderable setup_board_paint();
    std::vector<sm::Renderable> setup_nodes();
    std::vector<sm::Renderable> setup_white_pieces();
    std::vector<sm::Renderable> setup_black_pieces();

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

    bool game_started {false};

    sm::Camera cam;
    sm::Camera2D cam_2d;
    PointCameraController cam_controller;
    sm::DirectionalLight directional_light;
    // sm::PointLight point_light;
    sm::ShadowBox shadow_box;

    StandardBoard board;
    TurnIndicator turn_indicator;
    Timer timer;

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

    std::shared_ptr<sm::GlTextureCubemap> field;
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

    Ui ui;
};
