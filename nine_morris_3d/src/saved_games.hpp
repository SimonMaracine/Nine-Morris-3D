#pragma once

#include <filesystem>
#include <vector>
#include <utility>
#include <stdexcept>
#include <ctime>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <nine_morris_3d_engine/external/cereal.h++>
#include <protocol.hpp>

#include "version.hpp"

using namespace sm::localization_literals;

struct SavedGame {
    enum class GameType {
        Local,
        LocalVsComputer,
        Online
    };

    enum class Ending {
        WinnerWhite,
        WinnerBlack,
        Draw
    };

    protocol::ClockTime initial_time {};
    GameType game_type {};
    Ending ending {};
    std::string initial_position;
    protocol::Moves moves;
    std::time_t game_time;  // The time when the game has finished

    template<typename Archive>
    void serialize(Archive& archive, const std::uint32_t) {
        archive(
            initial_time,
            game_type,
            ending,
            initial_position,
            moves,
            game_time
        );
    }
};

inline const char* to_string(SavedGame::GameType game_type) {
    const char* string {};

    switch (game_type) {
        case SavedGame::GameType::Local:
            string = "local"_L;
            break;
        case SavedGame::GameType::LocalVsComputer:
            string = "local vs computer"_L;
            break;
        case SavedGame::GameType::Online:
            string = "online"_L;
            break;
    }

    return string;
}

inline const char* to_string(SavedGame::Ending ending) {
    const char* string {};

    switch (ending) {
        case SavedGame::Ending::WinnerWhite:
            string = "1-0";
            break;
        case SavedGame::Ending::WinnerBlack:
            string = "0-1";
            break;
        case SavedGame::Ending::Draw:
            string = "1/2-1/2";
            break;
    }

    return string;
}

CEREAL_CLASS_VERSION(SavedGame, version_number())

class SavedGames {
public:
    const std::vector<SavedGame>& get() const { return m_saved_games; }

    void add_saved_game(SavedGame&& saved_game);
    void load(const std::filesystem::path& file_path);
    void save(const std::filesystem::path& file_path);
private:
    std::vector<SavedGame> m_saved_games;
};

struct SavedGamesError : std::runtime_error {
    explicit SavedGamesError(const char* message)
        : std::runtime_error(message) {}
    explicit SavedGamesError(const std::string& message)
        : std::runtime_error(message) {}
};
