#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/nine_mens_morris/nine_mens_morris_board.hpp"
#include "scenes/game_scene.hpp"
#include "game_options.hpp"

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

    void connect_events() override;
    void scene_setup() override;
    void scene_update() override;
    void scene_fixed_update() override;
    void scene_imgui_update() override;

    BoardObj& get_board() override;
    GamePlayer get_player_type() const override;
    void reset() override;
    void reset(const std::string& string) override;
    void play_move(const std::string& string) override;
    void timeout(PlayerColor color) override;
    void resign(PlayerColor color) override;
    void accept_draw_offer() override;
    void time_control_options_window() override;

    void set_scene_textures() override;
    void load_all_texture_data() const override;

    virtual bool twelve_mens_morris_mode() const = 0;
private:
    sm::Renderable setup_board() const;
    sm::Renderable setup_paint() const;
    std::vector<sm::Renderable> setup_nodes() const;
    std::vector<sm::Renderable> setup_white_pieces() const;
    std::vector<sm::Renderable> setup_black_pieces() const;

    std::shared_ptr<sm::GlTexture> load_board_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_paint_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_board_normal_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_white_piece_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_black_piece_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_piece_normal_texture(bool reload = false) const;

    NineMensMorrisBoard setup_renderables();

    NineMensMorrisBoard m_board;
};
