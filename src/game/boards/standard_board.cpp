#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/boards/standard_board.h"
#include "game/piece.h"

void StandardBoard::switch_turn_and_check_turns_without_mills() {
    if (phase == BoardPhase::MovePieces) {
        if (turns_without_mills == MAX_TURNS_WITHOUT_MILLS) {
            DEB_INFO("The max amount of turns without mills has been hit");

            FORMATTED_MESSAGE(
                message, 64, "%u turns have passed without a windmill made.",
                MAX_TURNS_WITHOUT_MILLS
            )

            game_over(
                BoardEnding {BoardEnding::TieBetweenBothPlayers, message},
                TURN_IS_WHITE_SO(PieceType::White, PieceType::Black)
            );
        }
    }

    turn = TURN_IS_WHITE_SO(BoardPlayer::Black, BoardPlayer::White);
    switched_turn = true;
}

bool StandardBoard::can_go(size_t source_node_index, size_t destination_node_index) {
    ASSERT(source_node_index != destination_node_index, "Source must be different than destination");

    if (can_jump[static_cast<int>(turn)]) {
        return true;
    }

    const Node& source_node = nodes[source_node_index];
    const Node& destination_node = nodes[destination_node_index];

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

void StandardBoard::check_player_number_of_pieces(BoardPlayer player) {
    if (player == BoardPlayer::White) {
        DEB_DEBUG("Checking white player number of pieces");

        if (white_pieces_count + not_placed_pieces_count == 3) {  // TODO really think this through
            DEB_INFO("White player can jump");

            can_jump[static_cast<int>(player)] = true;
        } else if (white_pieces_count + not_placed_pieces_count == 2) {
            DEB_INFO("White player has only 2 pieces");

            FORMATTED_MESSAGE(
                message, 64, "White player cannot make any more windmills."
            )

            game_over(BoardEnding {BoardEnding::WinnerBlack, message}, PieceType::White);
        }
    } else {
        DEB_DEBUG("Checking black player number of pieces");

        if (black_pieces_count + not_placed_pieces_count == 3) {
            DEB_INFO("Black player can jump");

            can_jump[static_cast<int>(player)] = true;
        } else if (black_pieces_count + not_placed_pieces_count == 2) {
            DEB_INFO("Black player has only 2 pieces");

            FORMATTED_MESSAGE(
                message, 64, "Black player cannot make any more windmills."
            )

            game_over(BoardEnding {BoardEnding::WinnerWhite, message}, PieceType::Black);
        }
    }
}

bool StandardBoard::is_player_blocked(BoardPlayer player) {
    DEB_DEBUG("{} player is checked if it's blocked", player == BoardPlayer::White ? "White" : "Black");

    const PieceType type = player == BoardPlayer::White ? PieceType::White : PieceType::Black;
    bool at_least_one_piece = false;

    if (can_jump[static_cast<int>(player)]) {
        return false;
    }

    for (auto& pair : pieces) {
        Piece& piece = pair.second;

        if (piece.type == type && !piece.pending_remove && piece.in_use) {
            at_least_one_piece = true;

            const Node& node = nodes[piece.node];

            switch (node.index) {
                case 0: {
                    const Node& node1 = nodes[1];
                    const Node& node2 = nodes[9];
                    if (node1.piece == NULL_INDEX|| node2.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 1: {
                    const Node& node1 = nodes[0];
                    const Node& node2 = nodes[2];
                    const Node& node3 = nodes[4];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX
                            || node3.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 2: {
                    const Node& node1 = nodes[1];
                    const Node& node2 = nodes[14];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 3: {
                    const Node& node1 = nodes[4];
                    const Node& node2 = nodes[10];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 4: {
                    const Node& node1 = nodes[1];
                    const Node& node2 = nodes[3];
                    const Node& node3 = nodes[5];
                    const Node& node4 = nodes[7];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX
                            || node3.piece == NULL_INDEX || node4.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 5: {
                    const Node& node1 = nodes[4];
                    const Node& node2 = nodes[13];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 6: {
                    const Node& node1 = nodes[7];
                    const Node& node2 = nodes[11];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 7: {
                    const Node& node1 = nodes[4];
                    const Node& node2 = nodes[6];
                    const Node& node3 = nodes[8];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX
                            || node3.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 8: {
                    const Node& node1 = nodes[7];
                    const Node& node2 = nodes[12];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 9: {
                    const Node& node1 = nodes[0];
                    const Node& node2 = nodes[10];
                    const Node& node3 = nodes[21];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX
                            || node3.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 10: {
                    const Node& node1 = nodes[3];
                    const Node& node2 = nodes[9];
                    const Node& node3 = nodes[11];
                    const Node& node4 = nodes[18];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX
                            || node3.piece == NULL_INDEX || node4.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 11: {
                    const Node& node1 = nodes[6];
                    const Node& node2 = nodes[10];
                    const Node& node3 = nodes[15];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX
                            || node3.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 12: {
                    const Node& node1 = nodes[8];
                    const Node& node2 = nodes[13];
                    const Node& node3 = nodes[17];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX
                            || node3.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 13: {
                    const Node& node1 = nodes[5];
                    const Node& node2 = nodes[12];
                    const Node& node3 = nodes[14];
                    const Node& node4 = nodes[20];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX
                            || node3.piece == NULL_INDEX || node4.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 14: {
                    const Node& node1 = nodes[2];
                    const Node& node2 = nodes[13];
                    const Node& node3 = nodes[23];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX
                            || node3.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 15: {
                    const Node& node1 = nodes[11];
                    const Node& node2 = nodes[16];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 16: {
                    const Node& node1 = nodes[15];
                    const Node& node2 = nodes[17];
                    const Node& node3 = nodes[19];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX
                            || node3.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 17: {
                    const Node& node1 = nodes[12];
                    const Node& node2 = nodes[16];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 18: {
                    const Node& node1 = nodes[10];
                    const Node& node2 = nodes[19];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 19: {
                    const Node& node1 = nodes[16];
                    const Node& node2 = nodes[18];
                    const Node& node3 = nodes[20];
                    const Node& node4 = nodes[22];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX
                            || node3.piece == NULL_INDEX || node4.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 20: {
                    const Node& node1 = nodes[13];
                    const Node& node2 = nodes[19];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 21: {
                    const Node& node1 = nodes[9];
                    const Node& node2 = nodes[22];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 22: {
                    const Node& node1 = nodes[19];
                    const Node& node2 = nodes[21];
                    const Node& node3 = nodes[23];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX
                            || node3.piece == NULL_INDEX)
                        return false;
                    break;
                }
                case 23: {
                    const Node& node1 = nodes[14];
                    const Node& node2 = nodes[22];
                    if (node1.piece == NULL_INDEX || node2.piece == NULL_INDEX)
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
