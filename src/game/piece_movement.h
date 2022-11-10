#pragma once

#include "other/constants.h"

struct PieceMovement {
    PieceMovementType type = PieceMovementType::None;
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 target = glm::vec3(0.0f);
    glm::vec3 target0 = glm::vec3(0.0f);
    glm::vec3 target1 = glm::vec3(0.0f);
    bool reached_target0 = false;
    bool reached_target1 = false;
    bool moving = false;
};
