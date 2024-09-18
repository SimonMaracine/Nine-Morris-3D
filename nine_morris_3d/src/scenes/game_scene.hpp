#pragma once

#include <vector>
#include <string>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/board.hpp"
#include "point_camera_controller.hpp"
#include "ui.hpp"
#include "turn_indicator.hpp"
#include "timer.hpp"
#include "constants.hpp"

enum class GameState {
    NextPlayer,
    HumanMakeMove,
    ComputerThink,
    ComputerMakeMove,
    Over
};

class GameScene : public sm::ApplicationScene {
public:
    explicit GameScene(sm::Ctx& ctx)
        : sm::ApplicationScene(ctx) {}

    // Call these in the child scene class
    void on_start() override;
    void on_stop() override;
    void on_update() override;
    void on_fixed_update() override;
    void on_imgui_update() override;

    virtual BoardObj& get_board() = 0;
    virtual void play_move_on_board(const std::string& string) = 0;

    PointCameraController& get_camera_controller() { return m_cam_controller; }
    GamePlayer& get_player_white() { return m_player_white; }
    GamePlayer& get_player_black() { return m_player_black; }
    GameState& get_game_state() { return m_game_state; }

    void load_and_set_skybox();
    void load_and_set_board_paint_texture();
    void load_and_set_textures();

    void set_renderable_textures();
protected:
    void on_window_resized(const sm::WindowResizedEvent& event);

    void update_game_state();

    void setup_camera();
    void setup_skybox();
    void setup_lights();

    sm::Renderable setup_board() const;
    sm::Renderable setup_board_paint() const;
    std::vector<sm::Renderable> setup_nodes(unsigned int count) const;
    std::vector<sm::Renderable> setup_white_pieces(unsigned int count) const;
    std::vector<sm::Renderable> setup_black_pieces(unsigned int count) const;
    TurnIndicator setup_turn_indicator() const;
    Timer setup_timer() const;

    void load_skybox_texture_data() const;
    void load_board_paint_texture_data() const;
    void load_all_texture_data() const;

    std::shared_ptr<sm::GlTexture> load_board_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_board_paint_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_board_normal_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_white_piece_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_black_piece_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_piece_normal_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTextureCubemap> load_skybox_texture_cubemap(bool reload = false) const;

    bool m_engine_started {false};
    bool m_game_started {false};
    GameState m_game_state {GameState::NextPlayer};
    GamePlayer m_player_white {};
    GamePlayer m_player_black {};
    glm::vec3 m_default_camera_position {};

    PointCameraController m_cam_controller;
    Ui m_ui;

    sm::Camera m_cam;
    sm::Camera2D m_cam_2d;
    sm::DirectionalLight m_directional_light;
    sm::ShadowBox m_shadow_box;

    std::shared_ptr<sm::GlTextureCubemap> m_skybox;
};
