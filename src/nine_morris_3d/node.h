#pragma once

#include <glm/glm.hpp>

#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/hoverable.h"
#include "graphics/renderer/new_renderer.h"
#include "nine_morris_3d/piece.h"

constexpr float NODE_Y_POSITION = 0.063f;

constexpr glm::vec3 NODE_POSITIONS[24] = {
    glm::vec3(2.046f, NODE_Y_POSITION, 2.062f),    // 0
    glm::vec3(-0.008f, NODE_Y_POSITION, 2.089f),   // 1
    glm::vec3(-2.101f, NODE_Y_POSITION, 2.076f),   // 2
    glm::vec3(1.480f, NODE_Y_POSITION, 1.512f),    // 3
    glm::vec3(0.001f, NODE_Y_POSITION, 1.513f),    // 4
    glm::vec3(-1.509f, NODE_Y_POSITION, 1.502f),   // 5
    glm::vec3(0.889f, NODE_Y_POSITION, 0.898f),    // 6
    glm::vec3(0.001f, NODE_Y_POSITION, 0.906f),    // 7
    glm::vec3(-0.930f, NODE_Y_POSITION, 0.892f),   // 8
    glm::vec3(2.058f, NODE_Y_POSITION, 0.041f),    // 9
    glm::vec3(1.481f, NODE_Y_POSITION, 0.035f),    // 10
    glm::vec3(0.894f, NODE_Y_POSITION, 0.036f),    // 11
    glm::vec3(-0.934f, NODE_Y_POSITION, 0.060f),   // 12
    glm::vec3(-1.508f, NODE_Y_POSITION, 0.060f),   // 13
    glm::vec3(-2.083f, NODE_Y_POSITION, 0.057f),   // 14
    glm::vec3(0.882f, NODE_Y_POSITION, -0.894f),   // 15
    glm::vec3(0.011f, NODE_Y_POSITION, -0.900f),   // 16
    glm::vec3(-0.930f, NODE_Y_POSITION, -0.890f),  // 17
    glm::vec3(1.477f, NODE_Y_POSITION, -1.455f),   // 18
    glm::vec3(0.006f, NODE_Y_POSITION, -1.463f),   // 19
    glm::vec3(-1.493f, NODE_Y_POSITION, -1.458f),  // 20
    glm::vec3(2.063f, NODE_Y_POSITION, -2.046f),   // 21
    glm::vec3(0.001f, NODE_Y_POSITION, -2.061f),   // 22
    glm::vec3(-2.081f, NODE_Y_POSITION, -2.045f)   // 23
};

class Node {
public:
    Node() = default;
    Node(hoverable::Id id, unsigned int index);
    ~Node() = default;

    hoverable::Id id = hoverable::null;

    // glm::vec3 position = glm::vec3(0.0f);
    // float scale = 0.0f;

    // std::shared_ptr<VertexArray> vertex_array;
    // int index_count = 0;

    Renderer::Model model;

    hoverable::Id piece_id = hoverable::null;
    Piece* piece = nullptr;

    unsigned int index = 0;  // From 0 to 23, needed for easier coping with these
};
