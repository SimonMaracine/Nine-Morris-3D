#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

// #include "ecs/internal_components.h"
#include "nine_morris_3d.h"
#include "game/components/board.h"
#include "game/components/piece.h"
#include "game/components/node.h"
#include "game/components/active.h"
#include "game/components/piece_move.h"
#include "game/components/undo_redo_state.h"
#include "game/components/game_context.h"
#include "game/components/options.h"
#include "game/systems/board.h"
#include "game/board_common.h"
#include "game/constants.h"
// #include "other/logging.h"
// #include "other/assert.h"

// bool place_piece_system(entt::registry& registry, hover::Id hovered_id) {
//     bool placed = false;

//     auto board_view = registry.view<BoardComponent>();
//     auto& board_c = board_view.get<BoardComponent>(board_view.back());

//     auto view = registry.view<NodeComponent, HoverComponent, TransformComponent>();

//     for (auto entity : view) {
//         auto [node_c, hover_c, transform_c] = view.get<NodeComponent, HoverComponent, TransformComponent>(entity);

//         if (hover_c.id == hovered_id && entity == board_c.hovered_node && node_c.piece == entt::null) {
//             remember_state(registry);
//             WAIT_FOR_NEXT_MOVE();

//             const glm::vec3& position = transform_c.position;

//             if (board_c.turn == BoardPlayer::White) {
//                 node_c.piece = new_piece_to_place(registry, PieceType::White, position.x, position.z, entity);
//                 node_c.piece_index = registry.get<PieceComponent>(node_c.piece).index;
//                 board_c.white_pieces_count++;
//                 board_c.not_placed_pieces_count--;
//             } else {
//                 node_c.piece = new_piece_to_place(registry, PieceType::Black, position.x, position.z, entity);
//                 node_c.piece_index = registry.get<PieceComponent>(node_c.piece).index;
//                 board_c.black_pieces_count++;
//                 board_c.not_placed_pieces_count--;
//             }

//             if (is_windmill_made(registry, entity, TURN_IS_WHITE_SO(PieceType::White, PieceType::Black))) {
//                 DEB_DEBUG("{} windmill is made", TURN_IS_WHITE_SO("White", "Black"));

//                 board_c.should_take_piece = true;
//                 update_cursor_system(registry);

//                 if (board_c.turn == BoardPlayer::White) {
//                     set_pieces_to_take(registry, PieceType::Black, true);
//                 } else {
//                     set_pieces_to_take(registry, PieceType::White, true);
//                 }
//             } else {
//                 check_player_number_of_pieces(registry, board_c.turn);
//                 switch_turn(registry);

//                 if (board_c.not_placed_pieces_count == 0) {
//                     board_c.phase = BoardPhase::MovePieces;
//                     update_outlines(registry);

//                     DEB_INFO("Phase 2");

//                     if (is_player_blocked(registry, board_c.turn)) {
//                         DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

//                         FORMATTED_MESSAGE(
//                             message, 64, "%s player has blocked %s player.",
//                             TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("white", "black")
//                         )

//                         game_over(
//                             registry,
//                             TURN_IS_WHITE_SO(BoardEnding::WinnerBlack, BoardEnding::WinnerWhite),
//                             TURN_IS_WHITE_SO(PieceType::White, PieceType::Black),
//                             message
//                         );
//                     }
//                 }
//             }

//             placed = true;
//             break;
//         }
//     }

//     return placed;
// }

bool take_piece_system(entt::registry& registry, hover::Id hovered_id) {
    bool took = false;

    auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    auto view = registry.view<PieceComponent, HoverComponent, TransformComponent, ActiveComponent>();

    if (board_c.hovered_piece != entt::null) {  // Do anything only if there is a hovered piece
        for (auto entity : view) {
            auto [piece_c, hover_c, transform_c] = view.get<PieceComponent, HoverComponent, TransformComponent>(entity);

            if (board_c.turn == BoardPlayer::White) {
                if (hover_c.id == hovered_id && piece_c.type == PieceType::Black) {
                    ASSERT(registry.get<HoverComponent>(board_c.hovered_piece).id == hovered_id, "They must be equal");

                    if (!is_windmill_made(registry, piece_c.node, PieceType::Black) ||
                            number_of_pieces_in_windmills(registry, PieceType::Black) == board_c.black_pieces_count) {
                        ASSERT(piece_c.in_use, "Piece must be in use");

                        remember_state(registry);
                        WAIT_FOR_NEXT_MOVE();

                        auto& node_c = registry.get<NodeComponent>(piece_c.node);

                        node_c.piece = entt::null;
                        node_c.piece_index = INVALID_PIECE_INDEX;
                        take_and_raise_piece(registry, entity);
                        board_c.should_take_piece = false;
                        // update_cursor_system(registry);  // TODO don't call other systems
                        // TODO use app to call event
                        set_pieces_to_take(registry, PieceType::Black, false);
                        board_c.black_pieces_count--;
                        check_player_number_of_pieces(registry, BoardPlayer::Black);
                        check_player_number_of_pieces(registry, BoardPlayer::White);
                        switch_turn(registry);
                        update_outlines(registry);

                        DEB_DEBUG("Black piece {} taken", hovered_id);

                        if (is_player_blocked(registry, board_c.turn)) {
                            DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

                            FORMATTED_MESSAGE(
                                message, 64, "%s player has blocked %s player.",
                                TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("White", "Black")
                            )

                            game_over(
                                registry,
                                TURN_IS_WHITE_SO(BoardEnding::WinnerBlack, BoardEnding::WinnerWhite),
                                TURN_IS_WHITE_SO(PieceType::White, PieceType::Black),
                                message
                            );
                        }
                    } else {
                        DEB_DEBUG("Cannot take piece from windmill");
                    }

                    took = true;
                    break;
                }
            } else {
                if (hover_c.id == hovered_id && piece_c.type == PieceType::White) {
                    ASSERT(registry.get<HoverComponent>(board_c.hovered_piece).id == hovered_id, "They must be equal");

                    if (!is_windmill_made(registry, piece_c.node, PieceType::White) ||
                            number_of_pieces_in_windmills(registry, PieceType::White) == board_c.white_pieces_count) {
                        ASSERT(piece_c.in_use, "Piece must be in use");

                        remember_state(registry);
                        WAIT_FOR_NEXT_MOVE();

                        auto& node_c = registry.get<NodeComponent>(piece_c.node);

                        node_c.piece = entt::null;
                        node_c.piece_index = INVALID_PIECE_INDEX;
                        take_and_raise_piece(registry, entity);
                        board_c.should_take_piece = false;
                        // update_cursor_system(registry);  // TODO use app to call event
                        set_pieces_to_take(registry, PieceType::White, false);
                        board_c.white_pieces_count--;
                        check_player_number_of_pieces(registry, BoardPlayer::Black);
                        check_player_number_of_pieces(registry, BoardPlayer::White);
                        switch_turn(registry);
                        update_outlines(registry);

                        DEB_DEBUG("White piece {} taken", hovered_id);

                        if (is_player_blocked(registry, board_c.turn)) {
                            DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

                            FORMATTED_MESSAGE(
                                message, 64, "%s player has blocked %s player.",
                                TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("white", "black")
                            )

                            game_over(
                                registry,
                                TURN_IS_WHITE_SO(BoardEnding::WinnerBlack, BoardEnding::WinnerWhite),
                                TURN_IS_WHITE_SO(PieceType::White, PieceType::Black),
                                message
                            );
                        }
                    } else {
                        DEB_DEBUG("Cannot take piece from windmill");
                    }

                    took = true;
                    break;
                }
            }
        }

        // Do this even if it may not be needed
        if (board_c.not_placed_pieces_count == 0 && !board_c.should_take_piece && board_c.phase != BoardPhase::GameOver) {
            board_c.phase = BoardPhase::MovePieces;
            update_outlines(registry);

            DEB_INFO("Phase 2");
        }
    }

    return took;
}

bool put_down_piece_system(entt::registry& registry, hover::Id hovered_id) {
    bool put = false;

    auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    // Save the selected piece and the node from where it was
    entt::entity piece_put = entt::null;
    entt::entity node_from = entt::null;

    auto view = registry.view<NodeComponent, HoverComponent, TransformComponent>();

    if (board_c.selected_piece != entt::null) {  // Do anything only if there is a selected piece
        for (auto entity : view) {
            auto [node_c, hover_c, transform_c] = view.get<NodeComponent, HoverComponent, TransformComponent>(entity);

            auto [selected_piece_c, selected_transform_c] = registry.get<PieceComponent, TransformComponent>(board_c.selected_piece);

            if (hover_c.id == hovered_id && can_go(registry, selected_piece_c.node, entity)) {
                ASSERT(node_c.piece == entt::null, "Piece must not be null");

                remember_state(registry);
                WAIT_FOR_NEXT_MOVE();

                const bool three_step = (
                    selected_piece_c.type == PieceType::White && board_c.can_jump[static_cast<int>(PieceType::White)]
                    || selected_piece_c.type == PieceType::Black && board_c.can_jump[static_cast<int>(PieceType::Black)]
                );

                if (three_step) {
                    const glm::vec3 target = glm::vec3(transform_c.position.x, PIECE_Y_POSITION, transform_c.position.z);
                    const glm::vec3 target0 = selected_transform_c.position + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
                    const glm::vec3 target1 = target + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
                    const glm::vec3 velocity = glm::normalize(target0 - selected_transform_c.position) * PIECE_BASE_VELOCITY;

                    prepare_piece_for_three_step_move(registry, board_c.selected_piece, target, velocity, target0, target1);
                } else {
                    const glm::vec3 target = glm::vec3(transform_c.position.x, PIECE_Y_POSITION, transform_c.position.z);

                    prepare_piece_for_linear_move(
                        registry,
                        board_c.selected_piece,
                        target,
                        glm::normalize(target - selected_transform_c.position) * PIECE_BASE_VELOCITY
                    );
                }

                // Reset all of these
                auto& previous_node_c = registry.get<NodeComponent>(selected_piece_c.node);
                previous_node_c.piece = entt::null;
                previous_node_c.piece_index = INVALID_PIECE_INDEX;

                node_from = selected_piece_c.node;
                selected_piece_c.node = entity;
                selected_piece_c.node_index = node_c.index;
                selected_piece_c.selected = false;
                node_c.piece = board_c.selected_piece;
                node_c.piece_index = selected_piece_c.index;

                piece_put = board_c.selected_piece;
                board_c.selected_piece = entt::null;

                if (is_windmill_made(registry, entity, TURN_IS_WHITE_SO(PieceType::White, PieceType::Black))) {
                    DEB_DEBUG("{} windmill is made", TURN_IS_WHITE_SO("White", "Black"));

                    board_c.should_take_piece = true;
                    // update_cursor(registry);  // TODO use app to call event

                    if (board_c.turn == BoardPlayer::White) {
                        set_pieces_to_take(registry, PieceType::Black, true);
                        set_pieces_show_outline(registry, PieceType::White, false);
                    } else {
                        set_pieces_to_take(registry, PieceType::White, true);
                        set_pieces_show_outline(registry, PieceType::Black, false);
                    }

                    board_c.turns_without_mills = 0;
                } else {
                    check_player_number_of_pieces(registry, BoardPlayer::White);
                    check_player_number_of_pieces(registry, BoardPlayer::Black);
                    board_c.turns_without_mills++;
                    switch_turn(registry);
                    update_outlines(registry);

                    if (is_player_blocked(registry, board_c.turn)) {
                        DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

                        FORMATTED_MESSAGE(
                            message, 64, "%s player has blocked %s player.",
                            TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("white", "black")
                        )

                        game_over(
                            registry,
                            TURN_IS_WHITE_SO(BoardEnding::WinnerBlack, BoardEnding::WinnerWhite),
                            TURN_IS_WHITE_SO(PieceType::White, PieceType::Black),
                            message
                        );
                    }

                    remember_position_and_check_repetition(registry, piece_put, node_from);
                }

                put = true;
                break;
            }
        }
    }

    return put;
}

void computer_place_piece_system(entt::registry& registry, size_t node_index) {
    auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    auto view = registry.view<NodeComponent, TransformComponent>();

    for (auto entity : view) {
        auto [node_c, transform_c] = view.get<NodeComponent, TransformComponent>(entity);

        if (node_c.index == node_index) {
            ASSERT(node_c.piece == entt::null, "Node must not already have a piece");

            remember_state(registry);
            WAIT_FOR_NEXT_MOVE();

            const glm::vec3& position = transform_c.position;

            if (board_c.turn == BoardPlayer::White) {
                node_c.piece = new_piece_to_place(registry, PieceType::White, position.x, position.z, entity);
                node_c.piece_index = registry.get<PieceComponent>(node_c.piece).index;
                board_c.white_pieces_count++;
                board_c.not_placed_pieces_count--;
            } else {
                node_c.piece = new_piece_to_place(registry, PieceType::Black, position.x, position.z, entity);
                node_c.piece_index = registry.get<PieceComponent>(node_c.piece).index;
                board_c.black_pieces_count++;
                board_c.not_placed_pieces_count--;
            }

            if (is_windmill_made(registry, entity, TURN_IS_WHITE_SO(PieceType::White, PieceType::Black))) {
                DEB_DEBUG("{} windmill is made", TURN_IS_WHITE_SO("White", "Black"));

                board_c.should_take_piece = true;
                // update_cursor(registry);  // TODO use app to call event

                if (board_c.turn == BoardPlayer::White) {
                    set_pieces_to_take(registry, PieceType::Black, true);
                } else {
                    set_pieces_to_take(registry, PieceType::White, true);
                }
            } else {
                check_player_number_of_pieces(registry, board_c.turn);
                switch_turn(registry);

                if (board_c.not_placed_pieces_count == 0) {
                    board_c.phase = BoardPhase::MovePieces;
                    update_outlines(registry);

                    DEB_INFO("Phase 2");

                    if (is_player_blocked(registry, board_c.turn)) {
                        DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

                        FORMATTED_MESSAGE(
                            message, 64, "%s player has blocked %s player.",
                            TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("white", "black")
                        )

                        game_over(
                            registry,
                            TURN_IS_WHITE_SO(BoardEnding::WinnerBlack, BoardEnding::WinnerWhite),
                            TURN_IS_WHITE_SO(PieceType::White, PieceType::Black),
                            message
                        );
                    }
                }
            }

            break;
        }
    }
}

void computer_take_piece_system(entt::registry& registry, size_t node_index) {
    auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    auto view = registry.view<TransformComponent, PieceComponent, ActiveComponent>();

    size_t piece_index = INVALID_PIECE_INDEX;
    registry.view<NodeComponent>().each([node_index, &piece_index](entt::entity, NodeComponent& node_c) {
        if (node_c.index == node_index) {
            piece_index = node_c.piece_index;
        }
    });
    ASSERT(piece_index != INVALID_PIECE_INDEX, "Piece index must be valid");

    for (auto entity : view) {
        auto [transform_c, piece_c] = view.get<TransformComponent, PieceComponent>(entity);

        if (board_c.turn == BoardPlayer::White) {
            if (piece_c.index == piece_index && piece_c.type == PieceType::Black) {
                const size_t pieces_in_windmills = number_of_pieces_in_windmills(registry, PieceType::Black);

                if (!is_windmill_made(registry, piece_c.node, PieceType::Black) || pieces_in_windmills == board_c.black_pieces_count) {
                    ASSERT(piece_c.in_use, "Piece must be in use");

                    remember_state(registry);
                    WAIT_FOR_NEXT_MOVE();

                    auto& node_c = registry.get<NodeComponent>(piece_c.node);

                    node_c.piece = entt::null;
                    node_c.piece_index = INVALID_PIECE_INDEX;
                    take_and_raise_piece(registry, entity);
                    board_c.should_take_piece = false;
                    // update_cursor(registry);  // TODO use app to call event
                    set_pieces_to_take(registry, PieceType::Black, false);
                    board_c.black_pieces_count--;
                    check_player_number_of_pieces(registry, BoardPlayer::Black);
                    check_player_number_of_pieces(registry, BoardPlayer::White);
                    switch_turn(registry);
                    update_outlines(registry);

                    DEB_DEBUG("Black piece {} taken", piece_c.index);

                    if (is_player_blocked(registry, board_c.turn)) {
                        DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

                        FORMATTED_MESSAGE(
                            message, 64, "%s player has blocked %s player.",
                            TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("White", "Black")
                        )

                        game_over(
                            registry,
                            TURN_IS_WHITE_SO(BoardEnding::WinnerBlack, BoardEnding::WinnerWhite),
                            TURN_IS_WHITE_SO(PieceType::White, PieceType::Black),
                            message
                        );
                    }
                } else {
                    DEB_DEBUG("Cannot take piece from windmill");
                }

                break;
            }
        } else {
            if (piece_c.index == piece_index && piece_c.type == PieceType::White) {
                const size_t pieces_in_windmills = number_of_pieces_in_windmills(registry, PieceType::White);

                if (!is_windmill_made(registry, piece_c.node, PieceType::White) || pieces_in_windmills == board_c.white_pieces_count) {
                    ASSERT(piece_c.in_use, "Piece must be in use");

                    remember_state(registry);
                    WAIT_FOR_NEXT_MOVE();

                    auto& node_c = registry.get<NodeComponent>(piece_c.node);

                    node_c.piece = entt::null;
                    node_c.piece_index = INVALID_PIECE_INDEX;
                    take_and_raise_piece(registry, entity);
                    board_c.should_take_piece = false;
                    // update_cursor(registry);  // TODO use app to call event
                    set_pieces_to_take(registry, PieceType::White, false);
                    board_c.white_pieces_count--;
                    check_player_number_of_pieces(registry, BoardPlayer::White);
                    check_player_number_of_pieces(registry, BoardPlayer::Black);
                    switch_turn(registry);
                    update_outlines(registry);

                    DEB_DEBUG("White piece {} taken", piece_c.index);

                    if (is_player_blocked(registry, board_c.turn)) {
                        DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

                        FORMATTED_MESSAGE(
                            message, 64, "%s player has blocked %s player.",
                            TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("white", "black")
                        )

                        game_over(
                            registry,
                            TURN_IS_WHITE_SO(BoardEnding::WinnerBlack, BoardEnding::WinnerWhite),
                            TURN_IS_WHITE_SO(PieceType::White, PieceType::Black),
                            message
                        );
                    }
                } else {
                    DEB_DEBUG("Cannot take piece from windmill");
                }

                break;
            }
        }
    }

    // Do this even if it may not be needed
    if (board_c.not_placed_pieces_count == 0 && !board_c.should_take_piece && board_c.phase != BoardPhase::GameOver) {
        board_c.phase = BoardPhase::MovePieces;
        update_outlines(registry);

        DEB_INFO("Phase 2");
    }
}

void computer_put_down_piece_system(entt::registry& registry, size_t source_node_index, size_t destination_node_index) {
    ASSERT(source_node_index != destination_node_index, "Source must not equal the destination");

    auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    // Save the selected piece and the node from where it was
    entt::entity piece_put = entt::null;
    entt::entity node_from = entt::null;

    entt::entity chosen_piece = entt::null;
    registry.view<NodeComponent>().each([source_node_index, &chosen_piece](entt::entity entity, NodeComponent& node_c) {
        if (node_c.index == source_node_index) {
            ASSERT(node_c.piece != entt::null, "Piece must not be null");
            chosen_piece = node_c.piece;
        }
    });

    auto [chosen_piece_c, chosen_transform_c] = registry.get<PieceComponent, TransformComponent>(chosen_piece);

    auto view = registry.view<NodeComponent, TransformComponent>();

    for (auto entity : view) {
        auto [node_c, transform_c] = view.get<NodeComponent, TransformComponent>(entity);

        if (node_c.index == destination_node_index && can_go(registry, chosen_piece_c.node, entity)) {
            ASSERT(node_c.piece == entt::null, "Piece must not be null");

            remember_state(registry);
            WAIT_FOR_NEXT_MOVE();

            const bool three_step = (
                chosen_piece_c.type == PieceType::White && board_c.can_jump[static_cast<int>(PieceType::White)]
                || chosen_piece_c.type == PieceType::Black && board_c.can_jump[static_cast<int>(PieceType::Black)]
            );

            if (three_step) {
                const glm::vec3 target = glm::vec3(transform_c.position.x, PIECE_Y_POSITION, transform_c.position.z);
                const glm::vec3 target0 = chosen_transform_c.position + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
                const glm::vec3 target1 = target + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
                const glm::vec3 velocity = glm::normalize(target0 - chosen_transform_c.position) * PIECE_BASE_VELOCITY;

                prepare_piece_for_three_step_move(registry, chosen_piece, target, velocity, target0, target1);
            } else {
                const glm::vec3 target = glm::vec3(transform_c.position.x, PIECE_Y_POSITION, transform_c.position.z);

                prepare_piece_for_linear_move(
                    registry,
                    chosen_piece,
                    target,
                    glm::normalize(target - chosen_transform_c.position) * PIECE_BASE_VELOCITY
                );
            }

            // Reset all of these
            auto& previous_node_c = registry.get<NodeComponent>(chosen_piece_c.node);
            previous_node_c.piece = entt::null;
            previous_node_c.piece_index = INVALID_PIECE_INDEX;

            node_from = chosen_piece_c.node;
            chosen_piece_c.node = entity;
            chosen_piece_c.node_index = node_c.index;
            chosen_piece_c.selected = false;
            node_c.piece = chosen_piece;
            node_c.piece_index = chosen_piece_c.index;

            piece_put = chosen_piece;

            if (is_windmill_made(registry, entity, TURN_IS_WHITE_SO(PieceType::White, PieceType::Black))) {
                DEB_DEBUG("{} windmill is made", TURN_IS_WHITE_SO("White", "Black"));

                board_c.should_take_piece = true;
                // update_cursor(registry);  // TODO use app to call event

                if (board_c.turn == BoardPlayer::White) {
                    set_pieces_to_take(registry, PieceType::Black, true);
                    set_pieces_show_outline(registry, PieceType::White, false);
                } else {
                    set_pieces_to_take(registry, PieceType::White, true);
                    set_pieces_show_outline(registry, PieceType::Black, false);
                }

                board_c.turns_without_mills = 0;
            } else {
                check_player_number_of_pieces(registry, BoardPlayer::White);
                check_player_number_of_pieces(registry, BoardPlayer::Black);
                board_c.turns_without_mills++;
                switch_turn(registry);
                update_outlines(registry);

                if (is_player_blocked(registry, board_c.turn)) {
                    DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

                    FORMATTED_MESSAGE(
                        message, 64, "%s player has blocked %s player.",
                        TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("white", "black")
                    )

                    game_over(
                        registry,
                        TURN_IS_WHITE_SO(BoardEnding::WinnerBlack, BoardEnding::WinnerWhite),
                        TURN_IS_WHITE_SO(PieceType::White, PieceType::Black),
                        message
                    );
                }

                remember_position_and_check_repetition(registry, piece_put, node_from);
            }

            break;
        }
    }
}

void select_piece_system(entt::registry& registry, hover::Id hovered_id) {
    auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    auto view = registry.view<PieceComponent, HoverComponent, ActiveComponent>();

    for (auto entity : view) {
        auto [piece_c, hover_c] = view.get<PieceComponent, HoverComponent>(entity);

        if (hover_c.id == hovered_id) {
            if (board_c.turn == BoardPlayer::White && piece_c.type == PieceType::White
                    || board_c.turn == BoardPlayer::Black && piece_c.type == PieceType::Black) {
                if (!piece_c.selected && !piece_c.pending_remove) {
                    board_c.selected_piece = entity;
                    piece_c.selected = true;
                    unselect_other_pieces(registry, entity);
                } else {
                    board_c.selected_piece = entt::null;
                    piece_c.selected = false;
                }
            }

            break;
        }
    }
}

void press_system(entt::registry& registry, hover::Id hovered_id) {
    auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    auto node_view = registry.view<NodeComponent, HoverComponent>();

    for (auto entity : node_view) {
        auto& hover_c = node_view.get<HoverComponent>(entity);

        if (hover_c.id == hovered_id) {
            board_c.hovered_node = entity;
            break;
        }
    }

    auto piece_view = registry.view<PieceComponent, HoverComponent, ActiveComponent>();

    for (auto entity : piece_view) {
        auto& hover_c = piece_view.get<HoverComponent>(entity);

        if (hover_c.id == hovered_id) {
            board_c.hovered_piece = entity;
            break;
        }
    }
}

void release_system(entt::registry& registry) {
    auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    board_c.hovered_node = entt::null;
    board_c.hovered_piece = entt::null;
}

void finalize_pieces_state_system(entt::registry& registry) {
    auto view = registry.view<PieceComponent, PieceMoveComponent, ActiveComponent>();

    for (auto entity : view) {
        auto& piece_move_c = view.get<PieceMoveComponent>(entity);

        if (piece_move_c.moving) {
            arrive_at_node(registry, entity);
        }
    }
}

// void update_cursor_system(entt::registry& registry) {
//     auto board_view = registry.view<BoardComponent>();
//     auto& board_c = board_view.get<BoardComponent>(board_view.back());

//     auto options_view = registry.view<OptionsComponent>();
//     auto& options_c = options_view.get<OptionsComponent>(options_view.back());

//     if (options_c.custom_cursor) {
//         if (board_c.should_take_piece) {
//             app->window->set_cursor(app->cross_cursor);  // TODO use a specific parameter

//             // if (board_c.keyboard != entt::null) {  // TODO use events
//                 // keyboard->quad.texture = app->data.keyboard_controls_cross_texture;
//             // }
//         } else {
//             app->window->set_cursor(app->arrow_cursor);

//             // if (keyboard != nullptr) {
//                 // keyboard->quad.texture = app->data.keyboard_controls_texture;
//             // }
//         }
//     }
// }

// void copy_smart(BoardComponent& destination, const BoardComponent& source, StateHistory* history) {

// }
