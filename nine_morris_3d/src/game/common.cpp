#include "game/common.hpp"

Player opponent(Player player) {
    if (player == Player::White) {
        return Player::Black;
    } else {
        return Player::White;
    }
}
