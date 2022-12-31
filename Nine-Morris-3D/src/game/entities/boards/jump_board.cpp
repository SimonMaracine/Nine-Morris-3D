#include <engine/engine_graphics.h>
#include <engine/engine_other.h>

#include "game/entities/boards/jump_board.h"
#include "game/entities/serialization/jump_board_serialized.h"
#include "game/undo_redo_state.h"
#include "game/entities/piece.h"
#include "other/constants.h"
#include "other/data.h"

void JumpBoard::click(identifier::Id hovered_id) {
    // Check for clicked nodes
    for (const Node& node : nodes) {
        if (node.model->bounding_box->id == hovered_id) {
            clicked_node_index = node.index;
            break;
        }
    }

    // Check for clicked pieces
    for (const auto& [_, piece] : pieces) {
        if (piece.model->bounding_box->id == hovered_id) {
            clicked_piece_index = piece.index;
            break;
        }
    }
}

Board::Flags JumpBoard::release(identifier::Id hovered_id) {
    switch (phase) {
        case BoardPhase::PlacePieces:
            if (must_take_piece) {
                check_take_piece(hovered_id);
            } else {
                check_place_piece(hovered_id);
            }
            break;
        case BoardPhase::MovePieces:
            if (must_take_piece) {
                check_take_piece(hovered_id);
            } else {
                check_select_piece(hovered_id);
                check_move_piece(hovered_id);
            }
            break;
        default:
            break;
    }

    clicked_node_index = NULL_INDEX;
    clicked_piece_index = NULL_INDEX;

    const auto result = Flags {
        flags.did_action,
        flags.switched_turn,
        flags.must_take_or_took_piece
    };

    flags = Flags {};

    return result;
}

void JumpBoard::computer_place_piece(size_t node_index) {
    remember_state();
    place_piece(node_index);
}

void JumpBoard::computer_move_piece(size_t source_node_index, size_t destination_node_index) {
    remember_state();
    move_piece(nodes.at(source_node_index).piece_index, destination_node_index);
}

void JumpBoard::computer_take_piece(size_t node_index) {
    remember_state();
    take_piece(nodes.at(node_index).piece_index);
}

void JumpBoard::check_select_piece(identifier::Id hovered_id) {
    for (const auto& [index, piece] : pieces) {
        if (index == clicked_piece_index && piece.model->bounding_box->id == hovered_id) {
            const bool can_select = (
                turn == BoardPlayer::White && piece.type == PieceType::White
                || turn == BoardPlayer::Black && piece.type == PieceType::Black
            );

            if (can_select) {
                select_piece(index);
                break;
            }
        }
    }
}

void JumpBoard::check_place_piece(identifier::Id hovered_id) {
    for (const Node& node : nodes) {
        const bool can_place = (
            node.index == clicked_node_index && node.model->bounding_box->id == hovered_id
            && node.piece_index == NULL_INDEX
        );

        if (can_place) {
            remember_state();
            place_piece(node.index);

            flags.did_action = true;
            break;
        }
    }
}

void JumpBoard::check_move_piece(identifier::Id hovered_id) {
    if (selected_piece_index == NULL_INDEX) {
        return;
    }

    for (const Node& node : nodes) {
        const bool can_move = (
            node.index == clicked_node_index && node.model->bounding_box->id == hovered_id
            && node.piece_index == NULL_INDEX && can_go(selected_piece_index, node.index)
        );

        if (can_move) {
            remember_state();
            move_piece(selected_piece_index, node.index);

            selected_piece_index = NULL_INDEX;
            flags.did_action = true;
            break;
        }
    }
}

void JumpBoard::check_take_piece(identifier::Id hovered_id) {
    if (clicked_piece_index == NULL_INDEX) {
        return;
    }

    constexpr auto windmills = WINDMILLS_NINE_MENS_MORRIS;
    constexpr auto count = NINE_MENS_MORRIS_MILLS;

    for (auto& [index, piece] : pieces) {
        if (turn == BoardPlayer::White) {
            const bool valid_piece = (
                index == clicked_piece_index && piece.model->bounding_box->id == hovered_id
                && piece.type == PieceType::Black && piece.in_use
            );

            if (valid_piece) {
                const bool can_take = (
                    !is_windmill_made(piece.node_index, PieceType::Black, windmills, count)
                    || number_of_pieces_in_windmills(PieceType::Black, windmills, count) == black_pieces_count
                );

                if (can_take) {
                    remember_state();
                    take_piece(index);

                    flags.did_action = true;
                } else {
                    DEB_DEBUG("Cannot take black piece from windmill");
                }

                break;
            }
        } else {
            const bool valid_piece = (
                index == clicked_piece_index && piece.model->bounding_box->id == hovered_id
                && piece.type == PieceType::White && piece.in_use
            );

            if (valid_piece) {
                const bool can_take = (
                    !is_windmill_made(piece.node_index, PieceType::White, windmills, count)
                    || number_of_pieces_in_windmills(PieceType::White, windmills, count) == white_pieces_count
                );

                if (can_take) {
                    remember_state();
                    take_piece(index);

                    flags.did_action = true;
                } else {
                    DEB_DEBUG("Cannot take white piece from windmill");
                }

                break;
            }
        }
    }

    // Do this even if it may not be needed
    if (not_placed_white_pieces_count + not_placed_black_pieces_count == 0 && !must_take_piece
            && phase != BoardPhase::GameOver) {
        phase = BoardPhase::MovePieces;
        update_piece_outlines();

        DEB_INFO("Started phase 2");
    }
}

void JumpBoard::place_piece(size_t node_index) {
    ASSERT(node_index != NULL_INDEX, "Invalid index");

    Node& node = nodes.at(node_index);

    WAIT_FOR_NEXT_MOVE();

    const glm::vec3& position = node.model->position;

    if (turn == BoardPlayer::White) {
        node.piece_index = new_piece_to_place(PieceType::White, position.x, position.z, node_index);
        white_pieces_count++;
        not_placed_white_pieces_count--;
    } else {
        node.piece_index = new_piece_to_place(PieceType::Black, position.x, position.z, node_index);
        black_pieces_count++;
        not_placed_black_pieces_count--;
    }

    constexpr auto windmills = WINDMILLS_NINE_MENS_MORRIS;
    constexpr auto count = NINE_MENS_MORRIS_MILLS;

    if (is_windmill_made(node_index, TURN_IS_WHITE_SO(PieceType::White, PieceType::Black), windmills, count)) {
        DEB_DEBUG("{} windmill is made", TURN_IS_WHITE_SO("White", "Black"));

        must_take_piece = true;
        flags.must_take_or_took_piece = true;

        set_pieces_to_take(TURN_IS_WHITE_SO(PieceType::Black, PieceType::White), true);
    } else {
        check_player_number_of_pieces(turn);  // TODO maybe check both?
        switch_turn_and_check_turns_without_mills();

        if (not_placed_white_pieces_count + not_placed_black_pieces_count == 0) {
            phase = BoardPhase::MovePieces;
            update_piece_outlines();

            DEB_INFO("Started phase 2");

            if (is_player_blocked(turn)) {
                DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

                FORMATTED_MESSAGE(
                    message, 64, "%s player has blocked %s player.",
                    TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("white", "black")
                )

                game_over(
                    BoardEnding {TURN_IS_WHITE_SO(BoardEnding::WinnerBlack, BoardEnding::WinnerWhite), message},
                    TURN_IS_WHITE_SO(PieceType::White, PieceType::Black)
                );
            }
        }
    }
}

void JumpBoard::move_piece(size_t piece_index, size_t node_index) {
    ASSERT(piece_index != NULL_INDEX, "Invalid index");
    ASSERT(node_index != NULL_INDEX, "Invalid index");

    Piece& piece = pieces.at(piece_index);
    Node& node = nodes.at(node_index);

    ASSERT(node.piece_index == NULL_INDEX, "Piece must be null");

    WAIT_FOR_NEXT_MOVE();

    if (piece.type == PieceType::White && can_jump[static_cast<int>(PieceType::White)]
            || piece.type == PieceType::Black && can_jump[static_cast<int>(PieceType::Black)]) {
        const auto target = glm::vec3(node.model->position.x, PIECE_Y_POSITION, node.model->position.z);
        const auto target0 = piece.model->position + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
        const auto target1 = target + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
        const auto velocity = glm::normalize(target0 - piece.model->position) * PIECE_BASE_VELOCITY;

        prepare_piece_for_three_step_move(piece_index, target, velocity, target0, target1);
    } else {
        const auto target = glm::vec3(node.model->position.x, PIECE_Y_POSITION, node.model->position.z);

        prepare_piece_for_linear_move(
            piece_index,
            target,
            glm::normalize(target - piece.model->position) * PIECE_BASE_VELOCITY
        );
    }

    // Reset all of these
    Node& previous_node = nodes.at(piece.node_index);
    previous_node.piece_index = NULL_INDEX;

    piece.node_index = node_index;
    piece.selected = false;
    node.piece_index = piece_index;

    constexpr auto windmills = WINDMILLS_NINE_MENS_MORRIS;
    constexpr auto count = NINE_MENS_MORRIS_MILLS;

    if (is_windmill_made(node_index, TURN_IS_WHITE_SO(PieceType::White, PieceType::Black), windmills, count)) {
        DEB_DEBUG("{} windmill is made", TURN_IS_WHITE_SO("White", "Black"));

        must_take_piece = true;
        flags.must_take_or_took_piece = true;

        if (turn == BoardPlayer::White) {
            set_pieces_to_take(PieceType::Black, true);
            set_pieces_show_outline(PieceType::White, false);
        } else {
            set_pieces_to_take(PieceType::White, true);
            set_pieces_show_outline(PieceType::Black, false);
        }

        turns_without_mills = 0;
    } else {
        check_player_number_of_pieces(BoardPlayer::White);
        check_player_number_of_pieces(BoardPlayer::Black);

        turns_without_mills++;
        switch_turn_and_check_turns_without_mills();
        update_piece_outlines();

        if (is_player_blocked(turn)) {
            DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

            FORMATTED_MESSAGE(
                message, 64, "%s player has blocked %s player.",
                TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("white", "black")
            )

            game_over(
                BoardEnding {TURN_IS_WHITE_SO(BoardEnding::WinnerBlack, BoardEnding::WinnerWhite), message},
                TURN_IS_WHITE_SO(PieceType::White, PieceType::Black)
            );
        }

        remember_position_and_check_repetition(piece_index, node_index);
    }
}

void JumpBoard::take_piece(size_t piece_index) {
    ASSERT(piece_index != NULL_INDEX, "Invalid index");

    Piece& piece = pieces.at(piece_index);

    ASSERT(piece.in_use, "Piece must be in use");

    WAIT_FOR_NEXT_MOVE();

    nodes.at(piece.node_index).piece_index = NULL_INDEX;
    take_and_raise_piece(piece_index);
    must_take_piece = false;
    flags.must_take_or_took_piece = true;
    set_pieces_to_take(piece.type, false);

    if (piece.type == PieceType::White) {
        white_pieces_count--;
    } else {
        black_pieces_count--;
    }

    DEB_DEBUG("{} piece {} taken", piece.type == PieceType::White ? "White" : "Black", piece_index);

    check_player_number_of_pieces(BoardPlayer::Black);
    check_player_number_of_pieces(BoardPlayer::White);

    switch_turn_and_check_turns_without_mills();
    update_piece_outlines();

    if (is_player_blocked(turn)) {
        DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

        FORMATTED_MESSAGE(
            message, 64, "%s player has blocked %s player.",
            TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("white", "black")
        )

        game_over(
            BoardEnding {TURN_IS_WHITE_SO(BoardEnding::WinnerBlack, BoardEnding::WinnerWhite), message},
            TURN_IS_WHITE_SO(PieceType::White, PieceType::Black)
        );
    }
}

void JumpBoard::switch_turn_and_check_turns_without_mills() {
    if (phase == BoardPhase::MovePieces) {
        if (turns_without_mills == MAX_TURNS_WITHOUT_MILLS) {
            DEB_INFO("The max amount of turns without mills has been hit");

            FORMATTED_MESSAGE(
                message, 64, "%u turns have passed without a windmill.",
                MAX_TURNS_WITHOUT_MILLS
            )

            game_over(
                BoardEnding {BoardEnding::TieBetweenBothPlayers, message},
                TURN_IS_WHITE_SO(PieceType::White, PieceType::Black)
            );
        }
    }

    turn = TURN_IS_WHITE_SO(BoardPlayer::Black, BoardPlayer::White);
    flags.switched_turn = true;
}

bool JumpBoard::can_go(size_t piece_index, size_t destination_node_index) {
    ASSERT(piece_index != NULL_INDEX, "Invalid index");
    ASSERT(destination_node_index != NULL_INDEX, "Invalid index");

    const size_t source_node_index = pieces.at(piece_index).node_index;

    ASSERT(source_node_index != NULL_INDEX, "Source must not be null");
    ASSERT(source_node_index != destination_node_index, "Source must be different than destination");

    if (can_jump[static_cast<int>(turn)]) {
        return true;
    }

    const Node& source_node = nodes.at(source_node_index);
    const Node& destination_node = nodes.at(destination_node_index);

    switch (source_node.index) {
        case 0:
            if (destination_node.index == 1 || destination_node.index == 9)
                return true;
            break;
        case 1:
            if (destination_node.index == 0 || destination_node.index == 2 || destination_node.index == 4)
                return true;
            break;
        case 2:
            if (destination_node.index == 1 || destination_node.index == 14)
                return true;
            break;
        case 3:
            if (destination_node.index == 4 || destination_node.index == 10)
                return true;
            break;
        case 4:
            if (destination_node.index == 1 || destination_node.index == 3 || destination_node.index == 5
                    || destination_node.index == 7)
                return true;
            break;
        case 5:
            if (destination_node.index == 4 || destination_node.index == 13)
                return true;
            break;
        case 6:
            if (destination_node.index == 7 || destination_node.index == 11)
                return true;
            break;
        case 7:
            if (destination_node.index == 4 || destination_node.index == 6 || destination_node.index == 8)
                return true;
            break;
        case 8:
            if (destination_node.index == 7 || destination_node.index == 12)
                return true;
            break;
        case 9:
            if (destination_node.index == 0 || destination_node.index == 10 || destination_node.index == 21)
                return true;
            break;
        case 10:
            if (destination_node.index == 3 || destination_node.index == 9 || destination_node.index == 11
                    || destination_node.index == 18)
                return true;
            break;
        case 11:
            if (destination_node.index == 6 || destination_node.index == 10 || destination_node.index == 15)
                return true;
            break;
        case 12:
            if (destination_node.index == 8 || destination_node.index == 13 || destination_node.index == 17)
                return true;
            break;
        case 13:
            if (destination_node.index == 5 || destination_node.index == 12 || destination_node.index == 14
                    || destination_node.index == 20)
                return true;
            break;
        case 14:
            if (destination_node.index == 2 || destination_node.index == 13 || destination_node.index == 23)
                return true;
            break;
        case 15:
            if (destination_node.index == 11 || destination_node.index == 16)
                return true;
            break;
        case 16:
            if (destination_node.index == 15 || destination_node.index == 17 || destination_node.index == 19)
                return true;
            break;
        case 17:
            if (destination_node.index == 12 || destination_node.index == 16)
                return true;
            break;
        case 18:
            if (destination_node.index == 10 || destination_node.index == 19)
                return true;
            break;
        case 19:
            if (destination_node.index == 16 || destination_node.index == 18 || destination_node.index == 20
                    || destination_node.index == 22)
                return true;
            break;
        case 20:
            if (destination_node.index == 13 || destination_node.index == 19)
                return true;
            break;
        case 21:
            if (destination_node.index == 9 || destination_node.index == 22)
                return true;
            break;
        case 22:
            if (destination_node.index == 19 || destination_node.index == 21 || destination_node.index == 23)
                return true;
            break;
        case 23:
            if (destination_node.index == 14 || destination_node.index == 22)
                return true;
            break;
    }

    return false;
}

void JumpBoard::check_player_number_of_pieces(BoardPlayer player) {
    if (player == BoardPlayer::White) {
        DEB_DEBUG("Checking white player number of pieces...");

        if (white_pieces_count + not_placed_white_pieces_count == 3) {
            DEB_INFO("White player can jump now");

            can_jump[static_cast<int>(player)] = true;
        } else if (white_pieces_count + not_placed_white_pieces_count == 2) {
            DEB_INFO("White player has only 2 pieces now");

            FORMATTED_MESSAGE(
                message, 64, "White player cannot make any more windmills."
            )

            game_over(BoardEnding {BoardEnding::WinnerBlack, message}, PieceType::White);
        }
    } else {
        DEB_DEBUG("Checking black player number of pieces...");

        if (black_pieces_count + not_placed_black_pieces_count == 3) {
            DEB_INFO("Black player can jump now");

            can_jump[static_cast<int>(player)] = true;
        } else if (black_pieces_count + not_placed_black_pieces_count == 2) {
            DEB_INFO("Black player has only 2 pieces now");

            FORMATTED_MESSAGE(
                message, 64, "Black player cannot make any more windmills."
            )

            game_over(BoardEnding {BoardEnding::WinnerWhite, message}, PieceType::Black);
        }
    }
}

bool JumpBoard::is_player_blocked(BoardPlayer player) {
    DEB_DEBUG("Checking {} player if it's blocked...", player == BoardPlayer::White ? "white" : "black");

    const PieceType type = player == BoardPlayer::White ? PieceType::White : PieceType::Black;
    bool at_least_one_piece = false;

    if (can_jump[static_cast<int>(player)]) {
        return false;
    }

    for (auto& [_, piece] : pieces) {
        if (piece.type == type && !piece.pending_remove && piece.in_use) {
            at_least_one_piece = true;

            const Node& node = nodes.at(piece.node_index);

            switch (node.index) {
                case 0: {
                    const Node& node1 = nodes.at(1);
                    const Node& node2 = nodes.at(9);
                    if (node1.piece_index == NULL_INDEX|| node2.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 1: {
                    const Node& node1 = nodes.at(0);
                    const Node& node2 = nodes.at(2);
                    const Node& node3 = nodes.at(4);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX
                            || node3.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 2: {
                    const Node& node1 = nodes.at(1);
                    const Node& node2 = nodes.at(14);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 3: {
                    const Node& node1 = nodes.at(4);
                    const Node& node2 = nodes.at(10);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 4: {
                    const Node& node1 = nodes.at(1);
                    const Node& node2 = nodes.at(3);
                    const Node& node3 = nodes.at(5);
                    const Node& node4 = nodes.at(7);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX
                            || node3.piece_index == NULL_INDEX || node4.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 5: {
                    const Node& node1 = nodes.at(4);
                    const Node& node2 = nodes.at(13);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 6: {
                    const Node& node1 = nodes.at(7);
                    const Node& node2 = nodes.at(11);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 7: {
                    const Node& node1 = nodes.at(4);
                    const Node& node2 = nodes.at(6);
                    const Node& node3 = nodes.at(8);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX
                            || node3.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 8: {
                    const Node& node1 = nodes.at(7);
                    const Node& node2 = nodes.at(12);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 9: {
                    const Node& node1 = nodes.at(0);
                    const Node& node2 = nodes.at(10);
                    const Node& node3 = nodes.at(21);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX
                            || node3.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 10: {
                    const Node& node1 = nodes.at(3);
                    const Node& node2 = nodes.at(9);
                    const Node& node3 = nodes.at(11);
                    const Node& node4 = nodes.at(18);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX
                            || node3.piece_index == NULL_INDEX || node4.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 11: {
                    const Node& node1 = nodes.at(6);
                    const Node& node2 = nodes.at(10);
                    const Node& node3 = nodes.at(15);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX
                            || node3.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 12: {
                    const Node& node1 = nodes.at(8);
                    const Node& node2 = nodes.at(13);
                    const Node& node3 = nodes.at(17);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX
                            || node3.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 13: {
                    const Node& node1 = nodes.at(5);
                    const Node& node2 = nodes.at(12);
                    const Node& node3 = nodes.at(14);
                    const Node& node4 = nodes.at(20);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX
                            || node3.piece_index == NULL_INDEX || node4.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 14: {
                    const Node& node1 = nodes.at(2);
                    const Node& node2 = nodes.at(13);
                    const Node& node3 = nodes.at(23);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX
                            || node3.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 15: {
                    const Node& node1 = nodes.at(11);
                    const Node& node2 = nodes.at(16);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 16: {
                    const Node& node1 = nodes.at(15);
                    const Node& node2 = nodes.at(17);
                    const Node& node3 = nodes.at(19);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX
                            || node3.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 17: {
                    const Node& node1 = nodes.at(12);
                    const Node& node2 = nodes.at(16);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 18: {
                    const Node& node1 = nodes.at(10);
                    const Node& node2 = nodes.at(19);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 19: {
                    const Node& node1 = nodes.at(16);
                    const Node& node2 = nodes.at(18);
                    const Node& node3 = nodes.at(20);
                    const Node& node4 = nodes.at(22);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX
                            || node3.piece_index == NULL_INDEX || node4.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 20: {
                    const Node& node1 = nodes.at(13);
                    const Node& node2 = nodes.at(19);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 21: {
                    const Node& node1 = nodes.at(9);
                    const Node& node2 = nodes.at(22);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 22: {
                    const Node& node1 = nodes.at(19);
                    const Node& node2 = nodes.at(21);
                    const Node& node3 = nodes.at(23);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX
                            || node3.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
                case 23: {
                    const Node& node1 = nodes.at(14);
                    const Node& node2 = nodes.at(22);
                    if (node1.piece_index == NULL_INDEX || node2.piece_index == NULL_INDEX)
                        return false;
                    break;
                }
            }
        }
    }

    if (at_least_one_piece) {
        return true;
    } else {
        return false;
    }
}

void JumpBoard::remember_state() {
    JumpBoardSerialized serialized;
    to_serialized(serialized);

    const UndoRedoState<JumpBoardSerialized>::State current_state = {
        serialized, *camera_controller
    };

    undo_redo_state->undo.push_back(current_state);
    undo_redo_state->redo.clear();

    DEB_DEBUG("Pushed new state onto undo stack and cleared redo stack");
}

void JumpBoard::to_serialized(JumpBoardSerialized& serialized) {

    for (size_t i = 0; i < 24; i++) {
        serialized.nodes.at(i) = NodeSerialized {};
        serialized.nodes.at(i).index = nodes.at(i).index;
        serialized.nodes.at(i).piece_index = nodes.at(i).piece_index;
    }

    for (auto& [index, piece] : pieces) {
        serialized.pieces[index] = PieceSerialized {};
        serialized.pieces.at(index).index = piece.index;
        serialized.pieces.at(index).type = piece.type;
        serialized.pieces.at(index).in_use = piece.in_use;
        serialized.pieces.at(index).position = piece.model->position;
        serialized.pieces.at(index).rotation = piece.model->rotation;
        serialized.pieces.at(index).node_index = piece.node_index;
        serialized.pieces.at(index).show_outline = piece.show_outline;
        serialized.pieces.at(index).to_take = piece.to_take;
        serialized.pieces.at(index).pending_remove = piece.pending_remove;
    }

    serialized.phase = phase;
    serialized.turn = turn;
    serialized.ending = ending;
    serialized.must_take_piece = must_take_piece;
    serialized.can_jump = can_jump;
    serialized.repetition_history = repetition_history;
    serialized.is_players_turn = is_players_turn;
    serialized.white_pieces_count = white_pieces_count;
    serialized.black_pieces_count = black_pieces_count;
    serialized.not_placed_white_pieces_count = not_placed_white_pieces_count;
    serialized.not_placed_black_pieces_count = not_placed_black_pieces_count;
    serialized.turns_without_mills = turns_without_mills;
}

void JumpBoard::from_serialized(const JumpBoardSerialized& serialized) {
    auto& data = app->user_data<Data>();

    for (size_t i = 0; i < 24; i++) {
        nodes.at(i).index = serialized.nodes.at(i).index;
        nodes.at(i).piece_index = serialized.nodes.at(i).piece_index;
    }

    for (auto& [ser_index, ser_piece] : serialized.pieces) {
        if (pieces.find(ser_index) != pieces.end()) {
            pieces.at(ser_index).type = ser_piece.type;
            pieces.at(ser_index).in_use = ser_piece.in_use;
            pieces.at(ser_index).model->position = ser_piece.position;
            pieces.at(ser_index).model->rotation = ser_piece.rotation;
            pieces.at(ser_index).node_index = ser_piece.node_index;
            pieces.at(ser_index).show_outline = ser_piece.show_outline;
            pieces.at(ser_index).to_take = ser_piece.to_take;
            pieces.at(ser_index).pending_remove = ser_piece.pending_remove;

            // Reset these values
            pieces.at(ser_index).selected = false;
            pieces.at(ser_index).movement = PieceMovement {};
        } else {
            Piece piece = Piece {
                ser_index,
                ser_piece.type,
                app->res.model[hs {"piece" + std::to_string(ser_index)}],
                app->res.al_source[hs {"piece" + std::to_string(ser_index)}]
            };

            piece.model->position = ser_piece.position;
            piece.model->rotation = ser_piece.rotation;
            piece.model->vertex_array = app->res.vertex_array[hs {"piece" + std::to_string(ser_index)}];
            piece.model->index_buffer = (
                ser_piece.type == PieceType::White
                    ?
                    app->res.index_buffer["white_piece"_h]
                    :
                    app->res.index_buffer["black_piece"_h]
            );
            piece.model->scale = WORLD_SCALE;
            piece.model->material = app->res.material_instance[hs {"piece" + std::to_string(ser_index)}];
            piece.model->outline_color = std::make_optional<glm::vec3>(1.0f);
            piece.model->bounding_box = std::make_optional<Renderer::BoundingBox>();
            piece.model->bounding_box->id = data.piece_ids[ser_index];
            piece.model->bounding_box->size = PIECE_BOUNDING_BOX;
            piece.model->cast_shadow = true;

            piece.in_use = ser_piece.in_use;
            piece.node_index = ser_piece.node_index;
            piece.show_outline = ser_piece.show_outline;
            piece.to_take = ser_piece.to_take;
            piece.pending_remove = ser_piece.pending_remove;

            app->renderer->add_model(piece.model);
            pieces[ser_index] = piece;
        }
    }

    std::vector<size_t> to_remove;

    for (auto& [index, piece] : pieces) {
        if (serialized.pieces.find(index) == serialized.pieces.end()) {
            app->renderer->remove_model(piece.model);
            to_remove.push_back(index);
        }
    }

    for (size_t index : to_remove) {
        pieces.erase(index);
    }

    phase = serialized.phase;
    turn = serialized.turn;
    ending = serialized.ending;
    must_take_piece = serialized.must_take_piece;
    can_jump = serialized.can_jump;
    repetition_history = serialized.repetition_history;
    is_players_turn = serialized.is_players_turn;
    white_pieces_count = serialized.white_pieces_count;
    black_pieces_count = serialized.black_pieces_count;
    not_placed_white_pieces_count = serialized.not_placed_white_pieces_count;
    not_placed_black_pieces_count = serialized.not_placed_black_pieces_count;
    turns_without_mills = serialized.turns_without_mills;

    // Reset these values
    clicked_node_index = NULL_INDEX;
    clicked_piece_index = NULL_INDEX;
    selected_piece_index = NULL_INDEX;
}
