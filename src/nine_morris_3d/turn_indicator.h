#pragma once

#include <glm/glm.hpp>

class TurnIndicator {
public:
    TurnIndicator() = default;
    ~TurnIndicator() = default;

    void update(int width, int height);

    glm::vec3 position = glm::vec3(0.0f);
    float scale = 0.0f;
};
