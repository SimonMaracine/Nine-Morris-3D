#pragma once

#include <vector>
#include <string>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/board.hpp"
#include "point_camera_controller.hpp"
#include "ui.hpp"
#include "clock.hpp"
#include "move_list.hpp"
#include "game_options.hpp"
#include "constants.hpp"

enum class GameState {
    Ready,
    Start,
    NextTurn,
    HumanThinking,
    ComputerStartThinking,
    ComputerThinking,
    Stop,
    Over
};

class GameScene : public sm::ApplicationScene {
public:
    explicit GameScene(sm::Ctx& ctx)
        : sm::ApplicationScene(ctx) {}

    void on_start() override;
    void on_stop() override;
    void on_update() override;
    void on_fixed_update() override;
    void on_imgui_update() override;

    virtual void connect_events() = 0;
    virtual void scene_setup() = 0;
    virtual void scene_update() = 0;
    virtual void scene_fixed_update() = 0;
    virtual void scene_imgui_update() = 0;

    virtual GameOptions& get_game_options() = 0;
    virtual BoardObj& get_board() = 0;
    virtual GamePlayer get_player_type() const = 0;
    virtual void reset() = 0;
    virtual void reset(const std::string& string) = 0;
    virtual void play_move(const std::string& string) = 0;
    virtual void timeout(PlayerColor color) = 0;
    virtual void resign(PlayerColor color) = 0;
    virtual void accept_draw_offer() = 0;
    virtual void time_control_options_window() = 0;

    PointCameraController& get_camera_controller() { return m_cam_controller; }
    GameState& get_game_state() { return m_game_state; }
    const Clock& get_clock() const { return m_clock; }
    const MoveList& get_move_list() const { return m_move_list; }
    bool& get_draw_offered_by_remote() { return m_draw_offered_by_remote; }

    void load_and_set_skybox();
    void load_and_set_textures();
    virtual void set_scene_textures() = 0;
    virtual void load_all_texture_data() const = 0;
protected:
    void on_window_resized(const sm::WindowResizedEvent& event);

    void update_game_state();

    void setup_camera();
    void setup_skybox();
    void setup_lights();

    void load_skybox_texture_data() const;
    std::shared_ptr<sm::GlTextureCubemap> load_skybox_texture_cubemap(bool reload = false) const;

    GameState m_game_state {GameState::Ready};
    glm::vec3 m_default_camera_position {};
    Clock m_clock;
    MoveList m_move_list;
    bool m_draw_offered_by_remote {false};

    PointCameraController m_cam_controller;
    Ui m_ui;

    sm::Camera m_cam;
    sm::Camera2D m_cam_2d;
    sm::DirectionalLight m_directional_light;
    sm::ShadowBox m_shadow_box;

    std::shared_ptr<sm::GlTextureCubemap> m_skybox;
    // sm::Quad m_wait_indicator;  // TODO
};
