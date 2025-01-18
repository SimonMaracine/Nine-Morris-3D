#pragma once

enum class GameType {
    LocalHumanVsHuman,
    LocalHumanVsComputer,
    Online
};

// Data set before any game
// Represents what type of game it is and which are the players
struct GameOptions {
    template<typename T>
    explicit GameOptions(T time)
        : time(static_cast<int>(time)) {}

    int game_type {static_cast<int>(GameType::LocalHumanVsHuman)};

    struct LocalHumanVsHuman {};

    struct LocalHumanVsComputer {
        int computer_color {static_cast<int>(PlayerColor::Black)};
    } local_human_vs_computer;

    struct Online {
        int remote_color {static_cast<int>(PlayerColor::Black)};
    } online;

    int time {};
};
