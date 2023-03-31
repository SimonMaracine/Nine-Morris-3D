#include <engine/engine_other.h>
#include <engine/engine_scene.h>

#include "game/keyboard_controls.h"
#include "game/entities/board.h"
#include "game/entities/node.h"

#define POSITION(index) (glm::vec3(NODE_POSITIONS[index].x, 0.47f, NODE_POSITIONS[index].z))

static constexpr KeyboardControls::Direction NEXT[4][4] = {
    { KeyboardControls::Direction::Up, KeyboardControls::Direction::Left, KeyboardControls::Direction::Down, KeyboardControls::Direction::Right },
    { KeyboardControls::Direction::Left, KeyboardControls::Direction::Down, KeyboardControls::Direction::Right, KeyboardControls::Direction::Up },
    { KeyboardControls::Direction::Down, KeyboardControls::Direction::Right, KeyboardControls::Direction::Up, KeyboardControls::Direction::Left },
    { KeyboardControls::Direction::Right, KeyboardControls::Direction::Up, KeyboardControls::Direction::Left, KeyboardControls::Direction::Down }
};

KeyboardControls::KeyboardControls(Ctx* ctx, Board* board, renderables::Quad* quad)
    : board(board), quad(quad) {

    for (size_t i = 0; i < 24; i++) {
        nodes[i] = KNode(i);
    }

    quad->position = POSITION(0);
    quad->scale = 0.14f;
    quad->texture = (
        board->must_take_piece
            ?
            ctx->res.texture["keyboard_controls_cross"_H]
            :
            ctx->res.texture["keyboard_controls_default"_H]
    );
}

void KeyboardControls::post_initialize() {
    nodes[0].neighbors(nullptr, &nodes[9], nullptr, &nodes[1]);
    nodes[1].neighbors(nullptr, &nodes[4], &nodes[0], &nodes[2]);
    nodes[2].neighbors(nullptr, &nodes[14], &nodes[1], nullptr);
    nodes[3].neighbors(nullptr, &nodes[10], nullptr, &nodes[4]);
    nodes[4].neighbors(&nodes[1], &nodes[7], &nodes[3], &nodes[5]);
    nodes[5].neighbors(nullptr, &nodes[13], &nodes[4], nullptr);
    nodes[6].neighbors(nullptr, &nodes[11], nullptr, &nodes[7]);
    nodes[7].neighbors(&nodes[4], nullptr, &nodes[6], &nodes[8]);
    nodes[8].neighbors(nullptr, &nodes[12], &nodes[7], nullptr);
    nodes[9].neighbors(&nodes[0], &nodes[21], nullptr, &nodes[10]);
    nodes[10].neighbors(&nodes[3], &nodes[18], &nodes[9], &nodes[11]);
    nodes[11].neighbors(&nodes[6], &nodes[15], &nodes[10], nullptr);
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

    quad->position = POSITION(current_node->index);
}

Board::Flags KeyboardControls::click_and_release() {
    const Node& node = board->nodes[current_node->index];
    const identifier::Id hovered_id = (
        node.piece_index == NULL_INDEX
            ?
            node.model->bounding_box->id
            :
            board->pieces.at(node.piece_index).model->bounding_box->id
    );

    board->click(hovered_id);

    return board->release(hovered_id);
}

KeyboardControls::Direction KeyboardControls::calculate(Direction original_direction, float camera_angle) {
    int angle = static_cast<int>(camera_angle) % 360;
    if (angle < 0) {
        angle = 360 + angle;
    }

    if (angle > 315 || angle <= 45) {
        return NEXT[static_cast<int>(original_direction)][0];
    } else if (angle > 45 && angle <= 135) {
        return NEXT[static_cast<int>(original_direction)][1];
    } else if (angle > 135 && angle <= 225) {
        return NEXT[static_cast<int>(original_direction)][2];
    } else if (angle > 225 && angle <= 315) {
        return NEXT[static_cast<int>(original_direction)][3];
    }

    ASSERT(false, "This line shouldn't be reached");
    return {};
}
