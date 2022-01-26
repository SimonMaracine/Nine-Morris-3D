#include <glm/glm.hpp>

#include "nine_morris_3d/turn_indicator.h"

void TurnIndicator::update(int width, int height) {
    position = glm::vec3(static_cast<float>(width - 90), static_cast<float>(height - 115), 0.0f);
}
 