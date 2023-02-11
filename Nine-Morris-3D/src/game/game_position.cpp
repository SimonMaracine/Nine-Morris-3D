#include "game/game_position.h"
#include "other/constants.h"

GamePosition::GamePosition() {
    std::fill(data.begin(), data.end(), PieceType::None);
}

PieceType GamePosition::at(size_t index) const {
    return data.at(index);
}

PieceType& GamePosition::at(size_t index) {
    return data.at(index);
}

bool GamePosition::operator==(const GamePosition& other) const {
    return data == other.data;  // Don't take into account turns
}
