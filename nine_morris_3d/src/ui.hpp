#pragma once

#include <string>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "scenes/game_scene.hpp"
#include "options.hpp"

class Ui {
public:
    bool get_show_information() const { return m_show_information; }

    void update(sm::Ctx& ctx, GameScene& game_scene);
private:
    void main_menu_bar(sm::Ctx& ctx, GameScene& game_scene);

    // Local options data
    // When changed, update the options from the global data
    Options m_options;

    std::string m_last_saved_game_date {"No Date"};
    bool m_loading_skybox {false};
    bool m_show_information {true};
    bool m_can_undo {false};
    bool m_can_redo {false};
};
