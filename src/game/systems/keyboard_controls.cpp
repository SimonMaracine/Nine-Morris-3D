// #include <entt/entt.hpp>
// #include <nine_morris_3d_engine/nine_morris_3d_engine.h>

// // #include "ecs/internal_components.h"
// #include "game/components/keyboard_controls.h"
// #include "game/components/board.h"
// #include "game/components/node.h"
// #include "game/systems/keyboard_controls.h"
// #include "game/constants.h"
// // #include "other/assert.h"

// #define POSITION(index) glm::vec3(NODE_POSITIONS[index].x, KEYBOARD_CONTROLS_Y_POSITION, NODE_POSITIONS[index].z)

// static constexpr KeyboardControlsDirection NEXT[4][4] = {
//     { KeyboardControlsDirection::Up, KeyboardControlsDirection::Left, KeyboardControlsDirection::Down, KeyboardControlsDirection::Right },
//     { KeyboardControlsDirection::Left, KeyboardControlsDirection::Down, KeyboardControlsDirection::Right, KeyboardControlsDirection::Up },
//     { KeyboardControlsDirection::Down, KeyboardControlsDirection::Right, KeyboardControlsDirection::Up, KeyboardControlsDirection::Left },
//     { KeyboardControlsDirection::Right, KeyboardControlsDirection::Up, KeyboardControlsDirection::Left, KeyboardControlsDirection::Down }
// };

// void keyboard_controls_system(entt::registry& registry) {
//     const auto view = registry.view<BoardComponent>();
//     const auto& board_c = view.get<BoardComponent>(view.back());

//     constexpr auto construct = [](entt::registry& registry, entt::entity entity) {
//         auto [keyboard_controls_c, transform_c, quad_c] = registry.get<KeyboardControlsComponent, TransformComponent, QuadComponent>(entity);
//         auto& kb = keyboard_controls_c;

//         for (size_t i = 0; i < 24; i++) {
//             keyboard_controls_c.nodes[i] = KeyboardControlsComponent::KNode(i);
//         }

//         quad_c.quad.texture = board_c.should_take_piece ? app->data.keyboard_controls_cross_texture : app->data.keyboard_controls_texture;  // FIXME
//         transform_c.position = POSITION(0);
//         transform_c.scale = glm::vec3(0.14f);

//         kb.nodes[0].neighbors(nullptr, &kb.nodes[9], nullptr, &kb.nodes[1]);
//         kb.nodes[1].neighbors(nullptr, &kb.nodes[4], &kb.nodes[0], &kb.nodes[2]);
//         kb.nodes[2].neighbors(nullptr, &kb.nodes[14], &kb.nodes[1], nullptr);
//         kb.nodes[3].neighbors(nullptr, &kb.nodes[10], nullptr, &kb.nodes[4]);
//         kb.nodes[4].neighbors(&kb.nodes[1], &kb.nodes[7], &kb.nodes[3], &kb.nodes[5]);
//         kb.nodes[5].neighbors(nullptr, &kb.nodes[13], &kb.nodes[4], nullptr);
//         kb.nodes[6].neighbors(nullptr, &kb.nodes[11], nullptr, &kb.nodes[7]);
//         kb.nodes[7].neighbors(&kb.nodes[4], nullptr, &kb.nodes[6], &kb.nodes[8]);
//         kb.nodes[8].neighbors(nullptr, &kb.nodes[12], &kb.nodes[7], nullptr);
//         kb.nodes[9].neighbors(&kb.nodes[0], &kb.nodes[21], nullptr, &kb.nodes[10]);
//         kb.nodes[10].neighbors(&kb.nodes[3], &kb.nodes[18], &kb.nodes[9], &kb.nodes[11]);
//         kb.nodes[11].neighbors(&kb.nodes[6], &kb.nodes[15], &kb.nodes[10], nullptr);
//         kb.nodes[12].neighbors(&kb.nodes[8], &kb.nodes[17], nullptr, &kb.nodes[13]);
//         kb.nodes[13].neighbors(&kb.nodes[5], &kb.nodes[20], &kb.nodes[12], &kb.nodes[14]);
//         kb.nodes[14].neighbors(&kb.nodes[2], &kb.nodes[23], &kb.nodes[13], nullptr);
//         kb.nodes[15].neighbors(&kb.nodes[11], nullptr, nullptr, &kb.nodes[16]);
//         kb.nodes[16].neighbors(nullptr, &kb.nodes[19], &kb.nodes[15], &kb.nodes[17]);
//         kb.nodes[17].neighbors(&kb.nodes[12], nullptr, &kb.nodes[16], nullptr);
//         kb.nodes[18].neighbors(&kb.nodes[10], nullptr, nullptr, &kb.nodes[19]);
//         kb.nodes[19].neighbors(&kb.nodes[16], &kb.nodes[22], &kb.nodes[18], &kb.nodes[20]);
//         kb.nodes[20].neighbors(&kb.nodes[13], nullptr, &kb.nodes[19], nullptr);
//         kb.nodes[21].neighbors(&kb.nodes[9], nullptr, nullptr, &kb.nodes[22]);
//         kb.nodes[22].neighbors(&kb.nodes[19], nullptr, &kb.nodes[21], &kb.nodes[23]);
//         kb.nodes[23].neighbors(&kb.nodes[14], nullptr, &kb.nodes[22], nullptr);

//         kb.current_node = &kb.nodes[0];
//     };

//     registry.on_construct<KeyboardControlsComponent>().connect<&construct>();
// }

// void keyboard_controls_move_system(entt::registry& registry, KeyboardControlsDirection direction) {
//     auto view = registry.view<KeyboardControlsComponent, TransformComponent>();
//     auto [keyboard_controls_c, transform_c] = view.get<KeyboardControlsComponent, TransformComponent>(view.back());
//     auto& kb = keyboard_controls_c;

//     switch (direction) {
//         case KeyboardControlsDirection::Up:
//             if (kb.nodes[kb.current_node->index].neighbor_up != nullptr) {
//                 kb.current_node = kb.nodes[kb.current_node->index].neighbor_up;
//             }
//             break;
//         case KeyboardControlsDirection::Down:
//             if (kb.nodes[kb.current_node->index].neighbor_down != nullptr) {
//                 kb.current_node = kb.nodes[kb.current_node->index].neighbor_down;
//             }
//             break;
//         case KeyboardControlsDirection::Left:
//             if (kb.nodes[kb.current_node->index].neighbor_left != nullptr) {
//                 kb.current_node = kb.nodes[kb.current_node->index].neighbor_left;
//             }
//             break;
//         case KeyboardControlsDirection::Right:
//             if (kb.nodes[kb.current_node->index].neighbor_right != nullptr) {
//                 kb.current_node = kb.nodes[kb.current_node->index].neighbor_right;
//             }
//             break;
//     }

//     transform_c.position = POSITION(kb.current_node->index);
// }

// bool keyboard_controls_press_system(entt::registry& registry, bool& first_move) {
//     const auto board_view = registry.view<BoardComponent>();
//     const auto& board_c = board_view.get<BoardComponent>(board_view.back());

//     auto view = registry.view<KeyboardControlsComponent>();
//     auto& keyboard_controls_c = view.get<KeyboardControlsComponent>(view.back());
//     auto& kb = keyboard_controls_c;

//     auto [node_c, hover_c] = registry.get<NodeComponent, HoverComponent>(board_c.nodes[kb.current_node->index]);
//     const hover::Id hovered_id = node_c.piece == entt::null ? hover_c.id : registry.get<HoverComponent>(node_c.piece).id;

//     bool did = false;

//     // board->press(hovered_id);

//     if (board_c.phase == BoardPhase::PlacePieces) {
//         if (board_c.should_take_piece) {
//             // did = board->take_piece(hovered_id);
//         } else {
//             // did = board->place_piece(hovered_id);
//         }
//     } else if (board_c.phase == BoardPhase::MovePieces) {
//         if (board_c.should_take_piece) {
//             // did = board->take_piece(hovered_id);
//         } else {
//             // board->select_piece(hovered_id);
//             // did = board->put_down_piece(hovered_id);
//         }
//     }

//     return did;
// }

// KeyboardControlsDirection calculate(KeyboardControlsDirection original_direction, float camera_angle) {
//     int angle = static_cast<int>(camera_angle) % 360;
//     if (angle < 0) {
//         angle = 360 + angle;
//     }

//     if (angle > 315 || angle <= 45) {
//         return NEXT[static_cast<int>(original_direction)][0];
//     } else if (angle > 45 && angle <= 135) {
//         return NEXT[static_cast<int>(original_direction)][1];
//     } else if (angle > 135 && angle <= 225) {
//         return NEXT[static_cast<int>(original_direction)][2];
//     } else if (angle > 225 && angle <= 315) {
//         return NEXT[static_cast<int>(original_direction)][3];
//     }

//     ASSERT(false, "This shouldn't be reached");
//     return KeyboardControlsDirection::Up;
// }
