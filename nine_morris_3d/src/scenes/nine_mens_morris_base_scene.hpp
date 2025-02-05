#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/nine_mens_morris/nine_mens_morris_board.hpp"
#include "scenes/game_scene.hpp"

enum NineMensMorrisTime : int {
    NineMensMorrisTime1min,
    NineMensMorrisTime3min,
    NineMensMorrisTime10min,
    NineMensMorrisTime60min,
    NineMensMorrisTimeCustom
};

class NineMensMorrisBaseScene : public GameScene {
public:
    explicit NineMensMorrisBaseScene(sm::Ctx& ctx)
        : GameScene(ctx) {}

    void scene_setup() override;
    void scene_update() override;
    void scene_fixed_update() override;
    void scene_imgui_update() override;

    BoardObj& get_board() override;
    GamePlayer get_player_type() const override;
    std::string get_setup_position() const override;
    void reset(const std::vector<std::string>& moves = {}) override;
    void reset(const std::string& string, const std::vector<std::string>& moves = {}) override;
    void play_move(const std::string& string) override;
    void timeout(PlayerColor color) override;
    void resign(PlayerColor color) override;
    void accept_draw_offer() override;
    void time_control_options_window() override;

    void load_game_icons() override;
    void reload_scene_texture_data() const override;
    void reload_and_set_scene_textures() override;

    void start_engine() override;

    virtual bool twelve_mens_morris() const = 0;
private:
    sm::Renderable setup_board() const;
    sm::Renderable setup_paint() const;
    NineMensMorrisBoard::NodeRenderables setup_nodes() const;
    NineMensMorrisBoard::PieceRenderables setup_white_pieces() const;
    NineMensMorrisBoard::PieceRenderables setup_black_pieces() const;

    std::shared_ptr<sm::GlTexture> load_board_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_paint_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_board_normal_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_piece_white_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_piece_black_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_piece_normal_texture(bool reload = false) const;

    NineMensMorrisBoard setup_renderables();
    int pieces_count() const;

    NineMensMorrisBoard m_board;
};
