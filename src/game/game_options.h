#pragma once

#include "game/constants.h"

namespace game_options {
    static constexpr const char* GAME_OPTIONS_FILE = "options.json";

    static constexpr const char* FIELD = "field";
    static constexpr const char* AUTUMN = "autumn";

    constexpr int HUMAN = static_cast<int>(GamePlayer::Human);
    constexpr int COMPUTER = static_cast<int>(GamePlayer::Computer);

    struct GameOptions {
        // These are default values
        int samples = 2;
        int anisotropic_filtering = 4;  // TODO maybe move this to launcher too
        bool vsync = true;
        bool save_on_exit = true;
        std::string skybox = FIELD;
        bool custom_cursor = true;
        float sensitivity = 1.0f;
        bool hide_timer = false;
        bool labeled_board = true;
        int white_player = HUMAN;
        int black_player = COMPUTER;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(
                samples,
                anisotropic_filtering,
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

    std::tuple<bool, std::string> validate(const GameOptions& options);
}
