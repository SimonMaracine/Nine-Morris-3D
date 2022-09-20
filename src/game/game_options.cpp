#include "game/game_options.h"

namespace game_options {
    std::pair<bool, std::string> validate(const GameOptions& options) {
        if (options.samples != 1 && options.samples != 2 && options.samples != 4) {
            return {false, "Options file is invalid: samples"};
        }

        if (options.anisotropic_filtering != 0 && options.anisotropic_filtering != 4
                && options.anisotropic_filtering != 8) {
            return {false, "Options file is invalid: anisotropic_filtering"};
        }

        if (options.skybox != FIELD && options.skybox != AUTUMN) {
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

        return {true, ""};
    }
}
