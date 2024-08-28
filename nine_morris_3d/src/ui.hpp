#pragma once

#include <string>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

class Ui {
public:
    void update(sm::Ctx& ctx);

    bool get_show_information() const { return m_show_information; }
private:
    void main_menu_bar(sm::Ctx& ctx);

    std::string m_last_saved_game_date {"No Date"};
    int m_game_mode {};
    int m_white_player {};
    int m_black_player {};
    float m_master_volume {};
    float m_music_volume {};
    int m_skybox {};
    float m_camera_sensitivity {};
    bool m_enable_music {};
    bool m_vsync {};
    bool m_custom_cursor {};
    bool m_save_on_exit {};
    bool m_hide_timer {};
    bool m_labeled_board {};
    bool m_loading_skybox {false};
    bool m_show_information {true};
    bool m_can_undo {false};
    bool m_can_redo {false};
};
