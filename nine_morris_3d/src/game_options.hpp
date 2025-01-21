#pragma once

enum GameType : int {
    GameTypeLocalHumanVsHuman,
    GameTypeLocalHumanVsComputer,
    GameTypeOnline
};

// Data set before any game
// Represents what type of game it is and which are the players
struct GameOptions {
    int game_type {GameTypeLocalHumanVsHuman};

    struct LocalHumanVsHuman {};

    struct LocalHumanVsComputer {
        int computer_color {PlayerColorBlack};
    } local_human_vs_computer;

    struct Online {
        int remote_color {PlayerColorBlack};
    } online;

    int time {};  // This needs to be initialized with the corresponding enum
};
