#pragma once

enum class Player {
    White = 1,
    Black = 2
};

enum class Piece {
    None,
    White,
    Black
};

enum class GameOver {
    None,
    WinnerWhite,
    WinnerBlack,
    TieBetweenBothPlayers
};

template<typename Board>
struct Position {
    Board board {};
    Player turn {};

    bool operator==(const Position& other) const {
        return board == other.board && turn == other.turn;
    }
};

Player opponent(Player player);

template<typename Board>
unsigned int count_pieces(const Board& board, Player player) {
    unsigned int result {0};

    for (const Piece piece : board) {
        result += static_cast<unsigned int>(piece == static_cast<Piece>(player));
    }

    return result;
}
