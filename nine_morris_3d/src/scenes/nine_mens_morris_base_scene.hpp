#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/nine_mens_morris/nine_mens_morris_board.hpp"
#include "scenes/game_scene.hpp"
#include "game_options.hpp"
#include "constants.hpp"

class NineMensMorrisBaseScene : public GameScene {
public:
    explicit NineMensMorrisBaseScene(sm::Ctx& ctx)
        : GameScene(ctx) {}

    void connect_events() override;
    void scene_setup() override;
    void scene_update() override;
    void scene_fixed_update() override;
    void scene_imgui_update() override;

    GameOptions& get_game_options() override;
    BoardObj& get_board() override;
    GamePlayer get_board_player_type() const override;
    void reset() override;
    void reset(const std::string& string) override;
    void play_move(const std::string& string) override;
    void timeout(PlayerColor color) override;
    void time_control_options_window() override;

    void set_scene_textures() override;
    void load_all_texture_data() const override;

    virtual bool twelve_mens_morris_mode() const = 0;
private:
    void on_key_released(const sm::KeyReleasedEvent& event);
    void on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event);
    void on_mouse_button_released(const sm::MouseButtonReleasedEvent& event);

    sm::Renderable setup_board() const;
    sm::Renderable setup_board_paint() const;
    std::vector<sm::Renderable> setup_nodes(unsigned int count) const;
    std::vector<sm::Renderable> setup_white_pieces(unsigned int count) const;
    std::vector<sm::Renderable> setup_black_pieces(unsigned int count) const;

    std::shared_ptr<sm::GlTexture> load_board_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_board_paint_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_board_normal_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_white_piece_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_black_piece_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_piece_normal_texture(bool reload = false) const;

    NineMensMorrisBoard setup_renderables();

    NineMensMorrisBoard m_board;
    GameOptions m_game_options {NineMensMorrisTime::_10min};
};
