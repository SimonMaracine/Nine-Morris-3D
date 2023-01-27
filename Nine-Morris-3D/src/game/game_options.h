#pragma once

#include "other/constants.h"

namespace game_options {
    static constexpr const char* GAME_OPTIONS_FILE = "options.dat";

    static constexpr int NONE = static_cast<int>(Skybox::None);
    static constexpr int FIELD = static_cast<int>(Skybox::Field);
    static constexpr int AUTUMN = static_cast<int>(Skybox::Autumn);

    static constexpr int HUMAN = static_cast<int>(GamePlayer::Human);
    static constexpr int COMPUTER = static_cast<int>(GamePlayer::Computer);

    static constexpr int STANDARD = static_cast<int>(GameMode::Standard);
    static constexpr int JUMP = static_cast<int>(GameMode::Jump);
    static constexpr int JUMP_PLUS = static_cast<int>(GameMode::JumpPlus);

    static constexpr bool DEFAULT_VSYNC = true;
    static constexpr bool DEFAULT_SAVE_ON_EXIT = true;
    static constexpr int DEFAULT_SKYBOX = FIELD;
    static constexpr bool DEFAULT_CUSTOM_CURSOR = true;
    static constexpr float DEFAULT_SENSITIVITY = 1.0f;
    static constexpr bool DEFAULT_HIDE_TIMER = false;
    static constexpr bool DEFAULT_LABELED_BOARD = true;
    static constexpr int DEFAULT_WHITE_PLAYER = HUMAN;
    static constexpr int DEFAULT_BLACK_PLAYER = COMPUTER;
    static constexpr float DEFAULT_MASTER_VOLUME = 1.0f;
    static constexpr float DEFAULT_MUSIC_VOLUME = 0.7f;
    static constexpr bool DEFAULT_ENABLE_MUSIC = false;  // TODO temporary to false!
    static constexpr int DEFAULT_SCENE = STANDARD;

    struct GameOptions {
        bool vsync = DEFAULT_VSYNC;
        bool save_on_exit = DEFAULT_SAVE_ON_EXIT;
        int skybox = DEFAULT_SKYBOX;
        bool custom_cursor = DEFAULT_CUSTOM_CURSOR;
        float sensitivity = DEFAULT_SENSITIVITY;
        bool hide_timer = DEFAULT_HIDE_TIMER;
        bool labeled_board = DEFAULT_LABELED_BOARD;
        int white_player = DEFAULT_WHITE_PLAYER;
        int black_player = DEFAULT_BLACK_PLAYER;
        float master_volume = DEFAULT_MASTER_VOLUME;
        float music_volume = DEFAULT_MUSIC_VOLUME;
        bool enable_music = DEFAULT_ENABLE_MUSIC;
        int scene = DEFAULT_SCENE;  // TODO maybe store these as hashes

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
                black_player,
                master_volume,
                music_volume,
                enable_music,
                scene
            );
        }
    };

    std::pair<bool, std::string> validate(const GameOptions& options);
}
