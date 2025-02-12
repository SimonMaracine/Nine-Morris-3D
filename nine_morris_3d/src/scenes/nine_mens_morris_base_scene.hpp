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
    void reset_board(const std::string& string) override;
    bool second_player_starting() override;
    unsigned int clock_time(int time_enum) override;
    void play_move(const std::string& string) override;
    void timeout(PlayerColor color) override;
    void resign(PlayerColor color) override;
    void accept_draw() override;
    void time_control_options_window() override;
    void start_engine() override;
    void load_game_icons() override;
    void reload_scene_texture_data() const override;
    void reload_and_set_scene_textures() override;

    virtual bool twelve_mens_morris() const = 0;
private:
    std::shared_ptr<sm::ModelNode> setup_board() const;
    std::shared_ptr<sm::ModelNode> setup_paint() const;
    NineMensMorrisBoard::NodeModels setup_nodes() const;
    NineMensMorrisBoard::PieceModels setup_white_pieces() const;
    NineMensMorrisBoard::PieceModels setup_black_pieces() const;

    std::shared_ptr<sm::GlTexture> load_board_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_paint_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_board_normal_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_piece_white_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_piece_black_diffuse_texture(bool reload = false) const;
    std::shared_ptr<sm::GlTexture> load_piece_normal_texture(bool reload = false) const;

    NineMensMorrisBoard setup_models();
    int pieces_count() const;

    NineMensMorrisBoard m_board;
};
