#include "game/game_options.h"

namespace game_options {
    std::pair<bool, std::string> validate(const GameOptions& options) {
        if (options.skybox != FIELD && options.skybox != AUTUMN && options.skybox != NONE) {
            return {false, "Options file is invalid: skybox"};
        }

        if (options.sensitivity < 0.5f || options.sensitivity > 2.0f) {
            return {false, "Options file is invalid: sensitivity"};
        }

        if (options.white_player != HUMAN && options.white_player != COMPUTER) {
            return {false, "Options file is invalid: white_player"};
        }

        if (options.black_player != HUMAN && options.black_player != COMPUTER) {
            return {false, "Options file is invalid: black_player"};
        }

        if (options.master_volume < 0.0f || options.master_volume > 1.0f) {
            return {false, "Options file is invalid: master_volume"};
        }

        if (options.music_volume < 0.0f || options.music_volume > 1.0f) {
            return {false, "Options file is invalid: music_volume"};
        }

        if (options.scene != STANDARD && options.scene != JUMP && options.scene != JUMP_PLUS) {
            return {false, "Options file is invalid: scene"};
        }

        return {true, ""};
    }
}
