#include "engine/engine_application.h"

#include "game/game_position.h"
#include "other/constants.h"

GamePosition::GamePosition() {
    std::fill(data.begin(), data.end(), PieceType::None);
}

PieceType GamePosition::at(size_t index) const {
#ifdef NM3D_PLATFORM_DEBUG
    return data.at(index);
#else
    return data[index];
#endif
}

PieceType& GamePosition::at(size_t index) {
#ifdef NM3D_PLATFORM_DEBUG
    return data.at(index);
#else
    return data[index];
#endif
}

bool GamePosition::operator==(const GamePosition& other) const {
    return data == other.data;  // Don't take into account turns and pieces
}
