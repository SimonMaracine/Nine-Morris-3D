#include <glm/glm.hpp>

#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/keyboard_controls.h"
#include "nine_morris_3d/board.h"
#include "nine_morris_3d/node.h"
#include "other/logging.h"

KeyboardControls::KeyboardControls(Board& board)
    : board(board) {
    for (size_t i = 0; i < 24; i++) {
        nodes[i] = KeyboardControls::Node(i);
    }

    nodes[0].neighbors(nullptr, &nodes[9], nullptr, &nodes[1]);
    nodes[1].neighbors(nullptr, &nodes[4], &nodes[0], &nodes[2]);
    nodes[2].neighbors(nullptr, &nodes[14], &nodes[1], nullptr);
    nodes[3].neighbors(nullptr, &nodes[10], nullptr, &nodes[4]);
    nodes[4].neighbors(&nodes[1], &nodes[7], &nodes[3], &nodes[5]);
    nodes[5].neighbors(nullptr, &nodes[13], &nodes[4], nullptr);
    nodes[6].neighbors(nullptr, &nodes[11], nullptr, &nodes[7]);
    nodes[7].neighbors(&nodes[4], &nodes[6], nullptr, &nodes[8]);
    nodes[8].neighbors(nullptr, &nodes[12], &nodes[7], nullptr);
    nodes[9].neighbors(&nodes[0], &nodes[21], nullptr, &nodes[10]);
    nodes[10].neighbors(&nodes[3], &nodes[18], &nodes[9], &nodes[11]);
    nodes[11].neighbors(&nodes[11], &nodes[15], &nodes[10], nullptr);
    nodes[12].neighbors(&nodes[8], &nodes[17], nullptr, &nodes[13]);
    nodes[13].neighbors(&nodes[5], &nodes[20], &nodes[12], &nodes[14]);
    nodes[14].neighbors(&nodes[2], &nodes[23], &nodes[13], nullptr);
    nodes[15].neighbors(&nodes[11], nullptr, nullptr, &nodes[16]);
    nodes[16].neighbors(nullptr, &nodes[19], &nodes[15], &nodes[17]);
    nodes[17].neighbors(&nodes[12], nullptr, &nodes[16], nullptr);
    nodes[18].neighbors(&nodes[10], nullptr, nullptr, &nodes[19]);
    nodes[19].neighbors(&nodes[16], &nodes[22], &nodes[18], &nodes[20]);
    nodes[20].neighbors(&nodes[13], nullptr, &nodes[19], nullptr);
    nodes[21].neighbors(&nodes[9], nullptr, nullptr, &nodes[22]);
    nodes[22].neighbors(&nodes[19], nullptr, &nodes[21], &nodes[23]);
    nodes[23].neighbors(&nodes[14], nullptr, &nodes[22], nullptr);

    current_node = &nodes[0];

    glm::vec3 position = NODE_POSITIONS[current_node->index];
    position.y = 0.8f;
    quad.position = position;
    quad.scale = 0.3f;
    quad.texture = app->data.keyboard_controls_texture;
}

void KeyboardControls::move(Direction direction) {
    switch (direction) {
        case Direction::Up:
            if (nodes[current_node->index].neighbor_up != nullptr) {
                current_node = nodes[current_node->index].neighbor_up;
            }
            break;
        case Direction::Down:
            if (nodes[current_node->index].neighbor_down != nullptr) {
                current_node = nodes[current_node->index].neighbor_down;
            }
            break;
        case Direction::Left:
            if (nodes[current_node->index].neighbor_left != nullptr) {
                current_node = nodes[current_node->index].neighbor_left;
            }
            break;
        case Direction::Right:
            if (nodes[current_node->index].neighbor_right != nullptr) {
                current_node = nodes[current_node->index].neighbor_right;
            }
            break;
    }

    glm::vec3 position = NODE_POSITIONS[current_node->index];
    position.y = 0.8f;
    quad.position = position;
}

void KeyboardControls::press() {
    DEB_INFO("Pressed");
}

void KeyboardControls::release() {
    DEB_INFO("Released");
}
