#include "saved_games.hpp"

#include <fstream>

using namespace std::string_literals;

void SavedGames::add_saved_game(SavedGame&& saved_game) {
    m_saved_games.push_back(std::move(saved_game));
}

void SavedGames::load(const std::filesystem::path& file_path) {
    std::ifstream stream {file_path, std::ios::binary};

    if (!stream.is_open()) {
        throw SavedGamesError("Could not open file for reading: `" + file_path.string() + "`");
    }

    try {
        cereal::BinaryInputArchive archive {stream};
        archive(m_saved_games);
    } catch (const cereal::Exception& e) {
        m_saved_games.clear();
        throw SavedGamesError("Error reading from file: "s + e.what());
    } catch (...) {
        m_saved_games.clear();
        throw SavedGamesError("Unexpected error reading from file");
    }

    // Must clear the data on error, as it is incomplete
    // No validation for now
}

void SavedGames::save(const std::filesystem::path& file_path) {
    std::ofstream stream {file_path, std::ios::binary};

    if (!stream.is_open()) {
        throw SavedGamesError("Could not open file for writing: `" + file_path.string() + "`");
    }

    try {
        cereal::BinaryOutputArchive archive {stream};
        archive(m_saved_games);
    } catch (const cereal::Exception& e) {
        throw SavedGamesError("Error writing to file: "s + e.what());
    }
}
