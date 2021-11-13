#include <memory>
#include <array>
#include <vector>
#include <algorithm>
#include <cassert>
#include <iterator>

#include "nine_morris_3d/board.h"
#include "nine_morris_3d/hoverable.h"
#include "other/logging.h"

#define GET_ACTIVE_PIECES(result) \
    std::array<Piece*, 18> pointer_pieces; \
    for (unsigned int i = 0; i < 18; i++) { \
        pointer_pieces[i] = &pieces[i]; \
    } \
    std::vector<Piece*> result; \
    std::copy_if(pointer_pieces.begin(), pointer_pieces.end(), std::back_inserter(result), [](Piece* piece) { \
        return piece->active; \
    });

#define TURN_IS_WHITE_SO(_true, _false) turn == Player::White ? _true : _false

constexpr unsigned int WINDMILLS[16][3] = {
    { 0, 1, 2 }, { 2, 14, 23 }, { 21, 22, 23 }, { 0, 9, 21 },
    { 3, 4, 5 }, { 5, 13, 20 }, { 18, 19, 20 }, { 3, 10, 18 },
    { 6, 7, 8 }, { 8, 12, 17 }, { 15, 16, 17 }, { 6, 11, 15 },
    { 1, 4, 7 }, { 12, 13, 14 }, { 16, 19, 22 }, { 9, 10, 11 }
};

Board::Board(hoverable::Id id) : id(id) {

}

void Board::place_piece(hoverable::Id hovered_id) {
    for (Node& node : nodes) {
        if (node.id == hovered_id && (&node) == hovered_node && node.piece == nullptr) {
            // TODO remember place

            const glm::vec3& position = node.position;

            if (turn == Player::White) {
                node.piece = place_new_piece(Piece::White, position.x, position.z, &node);
                white_pieces_count++;
                not_placed_white_pieces_count--;
            } else {
                node.piece = place_new_piece(Piece::Black, position.x, position.z, &node);
                black_pieces_count++;
                not_placed_black_pieces_count--;
            }

            if (is_windmill_made(&node, TURN_IS_WHITE_SO(Piece::White, Piece::Black))) {
                SPDLOG_DEBUG("{} windmill is made", TURN_IS_WHITE_SO("White", "Black"));

                should_take_piece = true;

                if (turn == Player::White) {
                    set_pieces_to_take(Piece::Black, true);
                } else {
                    set_pieces_to_take(Piece::White, true);
                }

                turns_without_mills = 0;
            } else {
                check_player_number_of_pieces(turn);
                switch_turn();
            }

            if (not_placed_pieces_count() == 0 && !should_take_piece) {
                phase = Phase::MovePieces;
                update_outlines();

                if (check_player_blocked(turn)) {
                    SPDLOG_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));
                    game_over(TURN_IS_WHITE_SO(Ending::WinnerBlack, Ending::WinnerWhite),
                            TURN_IS_WHITE_SO(Piece::White, Piece::Black));
                }

                SPDLOG_INFO("Phase 2");
            }

            break;
        }
    }
}

void Board::move_pieces(float dt) {
    GET_ACTIVE_PIECES(active_pieces)

    for (Piece* piece : active_pieces) {
        if (piece->should_move) {
            if (piece->distance_travelled < glm::length(piece->distance_to_travel)) {
                glm::vec3 velocity = piece->velocity * dt;
                piece->position += velocity;
                piece->distance_travelled += glm::length(velocity);
            } else {
                piece->position = piece->target;

                // Reset all these variables
                piece->should_move = false;
                piece->velocity = glm::vec3(0.0f);
                piece->target = glm::vec3(0.0f);
                piece->distance_travelled = 0.0f;
                piece->distance_to_travel = glm::vec3(0.0f);

                // Remove piece if set to remove
                if (piece->pending_remove) {  // TODO something dodgy here
                    piece->active = false;
                }
            }
        }
    }
}

void Board::take_piece(hoverable::Id hovered_id) {
    for (Node& node : nodes) {
        if (node.piece != nullptr) {
            if (turn == Player::White) {
                if (node.piece->id == hovered_id && hovered_piece->id == hovered_id &&
                        node.piece->type == Piece::Black) {
                    if (!is_windmill_made(&node, Piece::Black) ||
                            number_of_pieces_in_windmills(Piece::Black) == black_pieces_count) {
                        // TODO remember take

                        take_and_raise_piece(node.piece);
                        node.piece = nullptr;
                        should_take_piece = false;
                        set_pieces_to_take(Piece::Black, false);
                        black_pieces_count--;
                        check_player_number_of_pieces(Player::White);
                        check_player_number_of_pieces(Player::Black);
                        switch_turn();
                        update_outlines();

                        if (check_player_blocked(turn)) {
                            SPDLOG_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));
                            game_over(TURN_IS_WHITE_SO(Ending::WinnerBlack, Ending::WinnerWhite),
                                    TURN_IS_WHITE_SO(Piece::White, Piece::Black));
                        }
                    } else {
                        SPDLOG_DEBUG("Cannot take piece from windmill");
                    }

                    break;
                }
            } else {
                if (node.piece->id == hovered_id && hovered_piece->id == hovered_id &&
                        node.piece->type == Piece::White) {
                    if (!is_windmill_made(&node, Piece::White) ||
                            number_of_pieces_in_windmills(Piece::White) == white_pieces_count) {
                        // TODO remember take

                        take_and_raise_piece(node.piece);
                        node.piece = nullptr;
                        should_take_piece = false;
                        set_pieces_to_take(Piece::White, false);
                        white_pieces_count--;
                        check_player_number_of_pieces(Player::White);
                        check_player_number_of_pieces(Player::Black);
                        switch_turn();
                        update_outlines();

                        if (check_player_blocked(turn)) {
                            SPDLOG_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));
                            game_over(TURN_IS_WHITE_SO(Ending::WinnerBlack, Ending::WinnerWhite),
                                    TURN_IS_WHITE_SO(Piece::White, Piece::Black));
                        }
                    } else {
                        SPDLOG_DEBUG("Cannot take piece from windmill");
                    }

                    break;
                }
            }
        }
    }

    // Do this even if it may not be needed
    if (phase == Phase::PlacePieces && not_placed_pieces_count() == 0 && !should_take_piece) {
        phase = Phase::MovePieces;
        update_outlines();
        SPDLOG_INFO("Phase 2");
    }
}

void Board::select_piece(hoverable::Id hovered_id) {
    GET_ACTIVE_PIECES(active_pieces)

    for (Piece* piece : active_pieces) {
        if (piece->id == hovered_id) {
            if (turn == Player::White && piece->type == Piece::White ||
                    turn == Player::Black && piece->type == Piece::Black) {
                if (!piece->selected && !piece->pending_remove) {
                    selected_piece = piece;
                    piece->selected = true;
                    unselect_other_pieces(piece);
                } else {
                    selected_piece = nullptr;
                    piece->selected = false;
                }
            }

            break;
        }
    }
}

void Board::put_piece(hoverable::Id hovered_id) {
    if (selected_piece != nullptr) {  // Do anything only if there is a selected piece
        for (Node& node : nodes) {
            if (node.id == hovered_id && can_go(selected_piece->node, &node)) {
                assert(node.piece == nullptr);
                // TODO remember move

                selected_piece->target.x = node.position.x;
                selected_piece->target.y = PIECE_Y_POSITION;
                selected_piece->target.z = node.position.z;

                selected_piece->velocity = (selected_piece->target - selected_piece->position) * PIECE_MOVE_SPEED;
                selected_piece->distance_to_travel = selected_piece->target - selected_piece->position;
                selected_piece->should_move = true;

                // Reset all of these
                Node* previous_node = selected_piece->node;
                previous_node->piece = nullptr;
                selected_piece->node = &node;
                selected_piece->selected = false;
                node.piece = selected_piece;
                selected_piece = nullptr;

                if (is_windmill_made(&node, TURN_IS_WHITE_SO(Piece::White, Piece::Black))) {
                    SPDLOG_DEBUG("{} windmill is made", TURN_IS_WHITE_SO("White", "Black"));

                    should_take_piece = true;

                    if (turn == Player::White) {
                        set_pieces_to_take(Piece::Black, true);
                        set_pieces_show_outline(Piece::White, false);
                    } else {
                        set_pieces_to_take(Piece::White, true);
                        set_pieces_show_outline(Piece::Black, false);
                    }

                    turns_without_mills = 0;
                } else {
                    check_player_number_of_pieces(Player::White);
                    check_player_number_of_pieces(Player::Black);
                    switch_turn();
                    update_outlines();

                    if (check_player_blocked(turn)) {
                        SPDLOG_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));
                        game_over(TURN_IS_WHITE_SO(Ending::WinnerBlack, Ending::WinnerWhite),
                                TURN_IS_WHITE_SO(Piece::White, Piece::Black));
                    }

                    remember_position_and_check_repetition();
                }

                break;
            }
        }
    }
}

void Board::press(hoverable::Id hovered_id) {
    for (Node& node : nodes) {
        if (node.id == hovered_id) {
            hovered_node = &node;
        }
    }

    GET_ACTIVE_PIECES(active_pieces)

    for (Piece* piece : active_pieces) {
        if (piece->id == hovered_id) {
            hovered_piece = piece;
        }
    }
}

void Board::release(hoverable::Id hovered_id) {
    hovered_node = nullptr;
    hovered_piece = nullptr;
}

void Board::undo() {
    // TODO this
}

Piece* Board::place_new_piece(Piece::Type type, float x_pos, float z_pos, Node* node) {
    GET_ACTIVE_PIECES(active_pieces)

    for (Piece* piece : active_pieces) {
        if (!piece->in_use && piece->type == type) {
            piece->target.x = x_pos;
            piece->target.y = PIECE_Y_POSITION;
            piece->target.z = z_pos;

            piece->velocity = (piece->target - piece->position) * PIECE_MOVE_SPEED;
            piece->distance_to_travel = piece->target - piece->position;
            piece->should_move = true;

            piece->in_use = true;
            piece->node = node;

            return piece;
        }
    }

    assert(false);
    return nullptr;
}

void Board::take_and_raise_piece(Piece* piece) {
    piece->target.x = piece->position.x;
    piece->target.y = PIECE_Y_POSITION + 1.5f;
    piece->target.z = piece->position.z;

    piece->velocity = (piece->target - piece->position) * PIECE_MOVE_SPEED;
    piece->distance_to_travel = piece->target - piece->position;
    piece->should_move = true;

    piece->node = nullptr;
    piece->pending_remove = true;
}

void Board::set_pieces_show_outline(Piece::Type type, bool show) {
    GET_ACTIVE_PIECES(active_pieces)

    for (Piece* piece : active_pieces) {
        if (piece->type == type) {
            piece->show_outline = show;
        }
    }
}

void Board::game_over(Ending ending, Piece::Type type_to_hide) {
    phase = Phase::GameOver;
    this->ending = ending;
    set_pieces_show_outline(type_to_hide, false);

    switch (ending) {
        case Ending::WinnerWhite:
            SPDLOG_INFO("Game over, white wins");
            break;
        case Ending::WinnerBlack:
            SPDLOG_INFO("Game over, black wins");
            break;
        case Ending::TieBetweenBothPlayers:
            SPDLOG_INFO("Game over, tie between both players");
            break;
        default:
            assert(false);
    }
}

void Board::switch_turn() {
    if (phase == Phase::MovePieces) {
        turns_without_mills++;

        if (turns_without_mills == MAX_TURNS_WITHOUT_MILLS) {
            SPDLOG_INFO("The max amount of turns without mills has been hit");
            game_over(Ending::TieBetweenBothPlayers,
                    TURN_IS_WHITE_SO(Piece::White, Piece::Black));
        }
    }

    turn = TURN_IS_WHITE_SO(Player::Black, Player::White);
}

bool Board::is_windmill_made(Node* node, Piece::Type type) {
    for (unsigned int i = 0; i < 16; i++) {
        const unsigned int* mill = WINDMILLS[i];

        Node& node1 = nodes[mill[0]];
        Node& node2 = nodes[mill[1]];
        Node& node3 = nodes[mill[2]];

        if (node1.piece != nullptr && node2.piece != nullptr && node3.piece != nullptr) {
            Piece* piece1 = node1.piece;
            Piece* piece2 = node2.piece;
            Piece* piece3 = node3.piece;

            if (piece1->type == type && piece2->type == type && piece3->type == type) {
                if (piece1->node == node || piece2->node == node || piece3->node == node) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Board::set_pieces_to_take(Piece::Type type, bool take) {
    GET_ACTIVE_PIECES(active_pieces)

    for (Piece* piece : active_pieces) {
        if (piece->type == type) {
            piece->to_take = take;
        }
    }
}

unsigned int Board::number_of_pieces_in_windmills(Piece::Type type) {
    std::vector<Piece*> pieces_inside_mills;

    for (unsigned int i = 0; i < 16; i++) {
        const unsigned int* mill = WINDMILLS[i];

        Node& node1 = nodes[mill[0]];
        Node& node2 = nodes[mill[1]];
        Node& node3 = nodes[mill[2]];

        if (node1.piece != nullptr && node2.piece != nullptr && node3.piece != nullptr) {
            Piece* piece1 = node1.piece;
            Piece* piece2 = node2.piece;
            Piece* piece3 = node3.piece;

            if (piece1->type == type && piece2->type == type && piece3->type == type) {
                std::vector<Piece*>::iterator it;

                it = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), node1.piece);
                if (it == pieces_inside_mills.end()) {
                    pieces_inside_mills.push_back(node1.piece);
                }
                it = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), node2.piece);
                if (it == pieces_inside_mills.end()) {
                    pieces_inside_mills.push_back(node2.piece);
                }
                it = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), node3.piece);
                if (it == pieces_inside_mills.end()) {
                    pieces_inside_mills.push_back(node3.piece);
                }
            }
        }
    }

    SPDLOG_DEBUG("Number of {} pieces in mills: {}", type == Piece::White ? "white" : "black",
            pieces_inside_mills.size());

    return pieces_inside_mills.size();
}

void Board::unselect_other_pieces(Piece* currently_selected_piece) {
    GET_ACTIVE_PIECES(active_pieces)

    for (Piece* piece : active_pieces) {
        if (piece != currently_selected_piece) {
            piece->selected = false;
        }
    }
}

void Board::update_outlines() {
    if (phase == Phase::MovePieces) {
        if (turn == Player::White) {
            set_pieces_show_outline(Piece::White, true);
            set_pieces_show_outline(Piece::Black, false);
        } else {
            set_pieces_show_outline(Piece::Black, true);
            set_pieces_show_outline(Piece::White, false);
        }
    }
}

bool Board::can_go(Node* source_node, Node* destination_node) {
    assert(source_node != destination_node);

    if (can_jump[(int) turn]) {
        return true;
    }

    switch (source_node->index) {
        case 0:
            if (destination_node->index == 1 || destination_node->index == 9)
                return true;
            break;
        case 1:
            if (destination_node->index == 0 || destination_node->index == 2 || destination_node->index == 4)
                return true;
            break;
        case 2:
            if (destination_node->index == 1 || destination_node->index == 14)
                return true;
            break;
        case 3:
            if (destination_node->index == 4 || destination_node->index == 10)
                return true;
            break;
        case 4:
            if (destination_node->index == 1 || destination_node->index == 3 || destination_node->index == 5 ||
                    destination_node->index == 7)
                return true;
            break;
        case 5:
            if (destination_node->index == 4 || destination_node->index == 13)
                return true;
            break;
        case 6:
            if (destination_node->index == 7 || destination_node->index == 11)
                return true;
            break;
        case 7:
            if (destination_node->index == 4 || destination_node->index == 6 || destination_node->index == 8)
                return true;
            break;
        case 8:
            if (destination_node->index == 7 || destination_node->index == 12)
                return true;
            break;
        case 9:
            if (destination_node->index == 0 || destination_node->index == 10 || destination_node->index == 21)
                return true;
            break;
        case 10:
            if (destination_node->index == 3 || destination_node->index == 9 || destination_node->index == 11 ||
                    destination_node->index == 18)
                return true;
            break;
        case 11:
            if (destination_node->index == 6 || destination_node->index == 10 || destination_node->index == 15)
                return true;
            break;
        case 12:
            if (destination_node->index == 8 || destination_node->index == 13 || destination_node->index == 17)
                return true;
            break;
        case 13:
            if (destination_node->index == 5 || destination_node->index == 12 || destination_node->index == 14 ||
                    destination_node->index == 20)
                return true;
            break;
        case 14:
            if (destination_node->index == 2 || destination_node->index == 13 || destination_node->index == 23)
                return true;
            break;
        case 15:
            if (destination_node->index == 11 || destination_node->index == 16)
                return true;
            break;
        case 16:
            if (destination_node->index == 15 || destination_node->index == 17 || destination_node->index == 19)
                return true;
            break;
        case 17:
            if (destination_node->index == 12 || destination_node->index == 16)
                return true;
            break;
        case 18:
            if (destination_node->index == 10 || destination_node->index == 19)
                return true;
            break;
        case 19:
            if (destination_node->index == 16 || destination_node->index == 18 || destination_node->index == 20 ||
                    destination_node->index == 22)
                return true;
            break;
        case 20:
            if (destination_node->index == 13 || destination_node->index == 19)
                return true;
            break;
        case 21:
            if (destination_node->index == 9 || destination_node->index == 22)
                return true;
            break;
        case 22:
            if (destination_node->index == 19 || destination_node->index == 21 || destination_node->index == 23)
                return true;
            break;
        case 23:
            if (destination_node->index == 14 || destination_node->index == 22)
                return true;
            break;
    }

    return false;
}

void Board::check_player_number_of_pieces(Player player) {
    if (player == Player::White) {
        SPDLOG_DEBUG("Checking white player number of pieces");

        if (white_pieces_count + not_placed_white_pieces_count == 3) {
            can_jump[(int) player] = true;
            SPDLOG_INFO("White player can jump");
        } else if (white_pieces_count + not_placed_white_pieces_count == 2) {
            SPDLOG_INFO("White player has only 2 pieces");
            game_over(Ending::WinnerBlack, Piece::White);
        }
    } else {
        SPDLOG_DEBUG("Checking black player number of pieces");

        if (black_pieces_count + not_placed_black_pieces_count == 3) {
            can_jump[(int) player] = true;
            SPDLOG_INFO("Black player can jump");
        } else if (black_pieces_count + not_placed_black_pieces_count == 2) {
            SPDLOG_INFO("Black player has only 2 pieces");
            game_over(Ending::WinnerWhite, Piece::Black);
        }
    }
}

bool Board::check_player_blocked(Player player) {
    SPDLOG_DEBUG("{} player is checked if is blocked",
            player == Player::White ? "White" : "Black");

    bool at_least_one_piece = false;
    Piece::Type type = player == Player::White ? Piece::White : Piece::Black;

    if (can_jump[(int) player]) {
        return false;
    }

    GET_ACTIVE_PIECES(active_pieces)

    for (Piece* piece : active_pieces) {
        if (piece->type == type && !piece->pending_remove && piece->in_use) {
            at_least_one_piece = true;

            switch (piece->node->index) {
                case 0: {
                    Node& node1 = nodes[1];
                    Node& node2 = nodes[9];
                    if (node1.piece == nullptr || node2.piece == nullptr)
                        return false;
                    break;
                }
                case 1: {
                    Node& node1 = nodes[0];
                    Node& node2 = nodes[2];
                    Node& node3 = nodes[4];
                    if (node1.piece == nullptr || node2.piece == nullptr ||
                            node3.piece == nullptr)
                        return false;
                    break;
                }
                case 2: {
                    Node& node1 = nodes[1];
                    Node& node2 = nodes[14];
                    if (node1.piece == nullptr || node2.piece == nullptr)
                        return false;
                    break;
                }
                case 3: {
                    Node& node1 = nodes[4];
                    Node& node2 = nodes[10];
                    if (node1.piece == nullptr || node2.piece == nullptr)
                        return false;
                    break;
                }
                case 4: {
                    Node& node1 = nodes[1];
                    Node& node2 = nodes[3];
                    Node& node3 = nodes[5];
                    Node& node4 = nodes[7];
                    if (node1.piece == nullptr || node2.piece == nullptr ||
                            node3.piece == nullptr || node4.piece == nullptr)
                        return false;
                    break;
                }
                case 5: {
                    Node& node1 = nodes[4];
                    Node& node2 = nodes[13];
                    if (node1.piece == nullptr || node2.piece == nullptr)
                        return false;
                    break;
                }
                case 6: {
                    Node& node1 = nodes[7];
                    Node& node2 = nodes[11];
                    if (node1.piece == nullptr || node2.piece == nullptr)
                        return false;
                    break;
                }
                case 7: {
                    Node& node1 = nodes[4];
                    Node& node2 = nodes[6];
                    Node& node3 = nodes[8];
                    if (node1.piece == nullptr || node2.piece == nullptr ||
                            node3.piece == nullptr)
                        return false;
                    break;
                }
                case 8: {
                    Node& node1 = nodes[7];
                    Node& node2 = nodes[12];
                    if (node1.piece == nullptr || node2.piece == nullptr)
                        return false;
                    break;
                }
                case 9: {
                    Node& node1 = nodes[0];
                    Node& node2 = nodes[10];
                    Node& node3 = nodes[21];
                    if (node1.piece == nullptr || node2.piece == nullptr ||
                            node3.piece == nullptr)
                        return false;
                    break;
                }
                case 10: {
                    Node& node1 = nodes[3];
                    Node& node2 = nodes[9];
                    Node& node3 = nodes[11];
                    Node& node4 = nodes[18];
                    if (node1.piece == nullptr || node2.piece == nullptr ||
                            node3.piece == nullptr || node4.piece == nullptr)
                        return false;
                    break;
                }
                case 11: {
                    Node& node1 = nodes[6];
                    Node& node2 = nodes[10];
                    Node& node3 = nodes[15];
                    if (node1.piece == nullptr || node2.piece == nullptr ||
                            node3.piece == nullptr)
                        return false;
                    break;
                }
                case 12: {
                    Node& node1 = nodes[8];
                    Node& node2 = nodes[13];
                    Node& node3 = nodes[17];
                    if (node1.piece == nullptr || node2.piece == nullptr ||
                            node3.piece == nullptr)
                        return false;
                    break;
                }
                case 13: {
                    Node& node1 = nodes[5];
                    Node& node2 = nodes[12];
                    Node& node3 = nodes[14];
                    Node& node4 = nodes[20];
                    if (node1.piece == nullptr || node2.piece == nullptr ||
                            node3.piece == nullptr || node4.piece == nullptr)
                        return false;
                    break;
                }
                case 14: {
                    Node& node1 = nodes[2];
                    Node& node2 = nodes[13];
                    Node& node3 = nodes[23];
                    if (node1.piece == nullptr || node2.piece == nullptr ||
                            node3.piece == nullptr)
                        return false;
                    break;
                }
                case 15: {
                    Node& node1 = nodes[11];
                    Node& node2 = nodes[16];
                    if (node1.piece == nullptr || node2.piece == nullptr)
                        return false;
                    break;
                }
                case 16: {
                    Node& node1 = nodes[15];
                    Node& node2 = nodes[17];
                    Node& node3 = nodes[19];
                    if (node1.piece == nullptr || node2.piece == nullptr ||
                            node3.piece == nullptr)
                        return false;
                    break;
                }
                case 17: {
                    Node& node1 = nodes[12];
                    Node& node2 = nodes[16];
                    if (node1.piece == nullptr || node2.piece == nullptr)
                        return false;
                    break;
                }
                case 18: {
                    Node& node1 = nodes[10];
                    Node& node2 = nodes[19];
                    if (node1.piece == nullptr || node2.piece == nullptr)
                        return false;
                    break;
                }
                case 19: {
                    Node& node1 = nodes[16];
                    Node& node2 = nodes[18];
                    Node& node3 = nodes[20];
                    Node& node4 = nodes[22];
                    if (node1.piece == nullptr || node2.piece == nullptr ||
                            node3.piece == nullptr || node4.piece == nullptr)
                        return false;
                    break;
                }
                case 20: {
                    Node& node1 = nodes[13];
                    Node& node2 = nodes[19];
                    if (node1.piece == nullptr || node2.piece == nullptr)
                        return false;
                    break;
                }
                case 21: {
                    Node& node1 = nodes[9];
                    Node& node2 = nodes[22];
                    if (node1.piece == nullptr || node2.piece == nullptr)
                        return false;
                    break;
                }
                case 22: {
                    Node& node1 = nodes[19];
                    Node& node2 = nodes[21];
                    Node& node3 = nodes[23];
                    if (node1.piece == nullptr || node2.piece == nullptr ||
                            node3.piece == nullptr)
                        return false;
                    break;
                }
                case 23: {
                    Node& node1 = nodes[14];
                    Node& node2 = nodes[22];
                    if (node1.piece == nullptr || node2.piece == nullptr)
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

std::array<Piece::Type, 24> Board::get_position() {
    std::array<Piece::Type, 24> position;

    for (unsigned int i = 0; i < 24; i++) {
        Node& node = nodes[i];

        if (node.piece != nullptr) {
            Piece* piece = node.piece;
            position[i] = piece->type;
        } else {
            position[i] = Piece::None;
        }
    }

    return position;
}

void Board::remember_position_and_check_repetition() {
    std::array<Piece::Type, 24> current_position = get_position();

    for (const std::array<Piece::Type, 24>& position : repetition_history.twos) {
        if (position == current_position) {
            SPDLOG_INFO("Threefold repetition");
            game_over(Ending::TieBetweenBothPlayers, TURN_IS_WHITE_SO(Piece::White, Piece::Black));
            return;
        }
    }

    for (const std::array<Piece::Type, 24>& position : repetition_history.ones) {
        if (position == current_position) {
            std::vector<std::array<Piece::Type, 24>>& vec = repetition_history.ones;
            vec.erase(std::remove(vec.begin(), vec.end(), position), vec.end());
            repetition_history.twos.push_back(position);
            return;
        }
    }

    repetition_history.ones.push_back(current_position);
}

unsigned int Board::not_placed_pieces_count() {
    return not_placed_white_pieces_count + not_placed_black_pieces_count;
}
