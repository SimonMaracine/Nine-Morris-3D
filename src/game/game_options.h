#pragma once

#include "other/constants.h"

namespace game_options {
    static constexpr const char* GAME_OPTIONS_FILE = "options.dat";

    static constexpr int NONE = static_cast<int>(Skybox::None);
    static constexpr int FIELD = static_cast<int>(Skybox::Field);
    static constexpr int AUTUMN = static_cast<int>(Skybox::Autumn);

    static constexpr int HUMAN = static_cast<int>(GamePlayer::Human);
    static constexpr int COMPUTER = static_cast<int>(GamePlayer::Computer);

    struct GameOptions {
        // These are default values
        bool vsync = true;
        bool save_on_exit = true;
        int skybox = FIELD;
        bool custom_cursor = true;
        float sensitivity = 1.0f;
        bool hide_timer = false;
        bool labeled_board = true;
        int white_player = HUMAN;
        int black_player = COMPUTER;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(
                vsync,
                save_on_exit,
                skybox,
                custom_cursor,
                sensitivity,
                hide_timer,
                labeled_board,
                white_player,
                black_player
            );
        }
    };

    std::pair<bool, std::string> validate(const GameOptions& options);
}
