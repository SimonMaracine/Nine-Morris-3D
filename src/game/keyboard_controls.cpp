#include "game/keyboard_controls.h"
#include "game/boards/generic_board.h"
#include "game/node.h"

#define Y_POSITION 0.47f
#define POSITION(index) (glm::vec3(NODE_POSITIONS[index].x, Y_POSITION, NODE_POSITIONS[index].z))

constexpr KeyboardControls::Direction NEXT[4][4] = {
    { KeyboardControls::Direction::Up, KeyboardControls::Direction::Left, KeyboardControls::Direction::Down, KeyboardControls::Direction::Right },
    { KeyboardControls::Direction::Left, KeyboardControls::Direction::Down, KeyboardControls::Direction::Right, KeyboardControls::Direction::Up },
    { KeyboardControls::Direction::Down, KeyboardControls::Direction::Right, KeyboardControls::Direction::Up, KeyboardControls::Direction::Left },
    { KeyboardControls::Direction::Right, KeyboardControls::Direction::Up, KeyboardControls::Direction::Left, KeyboardControls::Direction::Down }
};

KeyboardControls::KeyboardControls(GenericBoard* board)
    : board(board) {

    for (size_t i = 0; i < 24; i++) {
        nodes[i] = KNode(i);
    }

    quad->position = POSITION(0);
    quad->scale = 0.14f;
    quad->texture = (
        board->player_must_take_piece()
            ?
            board->app->res.texture["keyboard_controls_cross_texture"_h]
            :
            board->app->res.texture["keyboard_controls_texture"_h]
    );
}

void KeyboardControls::initialize() {
    board->keyboard = this;

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

bool KeyboardControls::press(bool& first_move) {
    // const Node& node = board->nodes[current_node->index];
    // const hoverable::Id hovered_id = node.piece == nullptr ? node.id : node.piece->id;

    // bool did = false;

    // board->press(hovered_id);

    // if (board->phase == GenericBoard::Phase::PlacePieces) {
    //     if (board->player_must_take_piece()) {
    //         did = board->take_piece(hovered_id);
    //     } else {
    //         did = board->place_piece(hovered_id);
    //     }
    // } else if (board->phase == GenericBoard::Phase::MovePieces) {
    //     if (board->player_must_take_piece()) {
    //         did = board->take_piece(hovered_id);
    //     } else {
    //         board->select_piece(hovered_id);
    //         did = board->put_down_piece(hovered_id);
    //     }
    // }

    // return did;

    // FIXME this
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

    ASSERT(false, "This shouldn't be reached");
    return {};
}
