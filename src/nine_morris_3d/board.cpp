#include <memory>
#include <array>
#include <vector>
#include <algorithm>
#include <cassert>
#include <iterator>

#include <glm/glm.hpp>

#include "nine_morris_3d/board.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "other/logging.h"
#include "other/assert.h"

#define GET_ACTIVE_PIECES(result) \
    std::array<Piece*, 18> pointer_pieces; \
    for (unsigned int i = 0; i < 18; i++) { \
        pointer_pieces[i] = &pieces[i]; \
    } \
    std::vector<Piece*> result; \
    std::copy_if(pointer_pieces.begin(), pointer_pieces.end(), std::back_inserter(result), [](const Piece* piece) { \
        return piece->active; \
    });

#define TURN_IS_WHITE_SO(_true, _false) (turn == Player::White ? _true : _false)
#define WAIT_FOR_NEXT_MOVE() next_move = false
#define CAN_MAKE_MOVE() next_move = true

constexpr unsigned int WINDMILLS[16][3] = {
    { 0, 1, 2 }, { 2, 14, 23 }, { 21, 22, 23 }, { 0, 9, 21 },
    { 3, 4, 5 }, { 5, 13, 20 }, { 18, 19, 20 }, { 3, 10, 18 },
    { 6, 7, 8 }, { 8, 12, 17 }, { 15, 16, 17 }, { 6, 11, 15 },
    { 1, 4, 7 }, { 12, 13, 14 }, { 16, 19, 22 }, { 9, 10, 11 }
};

Board::Board(std::shared_ptr<std::vector<Board>> board_state_history)
    : state_history(board_state_history) {}

bool Board::place_piece(hoverable::Id hovered_id) {
    bool placed = false;

    for (Node& node : nodes) {
        if (node.id == hovered_id && (&node) == hovered_node && node.piece == nullptr) {
            remember_state();
            WAIT_FOR_NEXT_MOVE();

            const glm::vec3& position = node.model.position;

            if (turn == Player::White) {
                node.piece = place_new_piece(Piece::White, position.x, position.z, &node);
                node.piece_id = node.piece->id;
                white_pieces_count++;
                not_placed_white_pieces_count--;
            } else {
                node.piece = place_new_piece(Piece::Black, position.x, position.z, &node);
                node.piece_id = node.piece->id;
                black_pieces_count++;
                not_placed_black_pieces_count--;
            }

            if (is_windmill_made(&node, TURN_IS_WHITE_SO(Piece::White, Piece::Black))) {
                DEB_DEBUG("{} windmill is made", TURN_IS_WHITE_SO("White", "Black"));

                should_take_piece = true;
                update_cursor();

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
                    DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));
                    game_over(TURN_IS_WHITE_SO(Ending::WinnerBlack, Ending::WinnerWhite),
                            TURN_IS_WHITE_SO(Piece::White, Piece::Black));
                }

                DEB_INFO("Phase 2");
            }

            placed = true;
            break;
        }
    }

    return placed;
}

void Board::move_pieces(float dt) {
    GET_ACTIVE_PIECES(active_pieces)

    for (Piece* piece : active_pieces) {
        if (piece->should_move) {
            switch (piece->movement.type) {
                case Piece::MovementType::None:
                    ASSERT(false, "Movement type 'None' is invalid");
                    break;
                case Piece::MovementType::Linear: {
                    piece->model.position += piece->movement.velocity * dt + (piece->movement.target - piece->model.position)
                            * PIECE_VARIABLE_VELOCITY * dt;

                    if (glm::length(piece->movement.target - piece->model.position) < 0.03f) {
                        arrive_at_node(piece);
                    }

                    break;
                }
                case Piece::MovementType::ThreeStep: {
                    if (!piece->movement.reached_target0) {
                        piece->model.position += piece->movement.velocity * dt + (piece->movement.target0 - piece->model.position)
                                * PIECE_VARIABLE_VELOCITY * dt;
                    } else if (!piece->movement.reached_target1) {
                        piece->model.position += piece->movement.velocity * dt + (piece->movement.target1 - piece->model.position)
                                * PIECE_VARIABLE_VELOCITY * dt;
                    } else {
                        piece->model.position += piece->movement.velocity * dt + (piece->movement.target - piece->model.position)
                                * PIECE_VARIABLE_VELOCITY * dt;
                    }

                    if (!piece->movement.reached_target0 &&
                            glm::length(piece->movement.target0 - piece->model.position) < 0.03f) {
                        piece->movement.reached_target0 = true;
                        piece->model.position = piece->movement.target0;
                        piece->movement.velocity = glm::normalize(piece->movement.target1 - piece->model.position)
                                * PIECE_BASE_VELOCITY;
                    } else if (!piece->movement.reached_target1 &&
                            glm::length(piece->movement.target1 - piece->model.position) < 0.03f) {
                        piece->movement.reached_target1 = true;
                        piece->model.position = piece->movement.target1;
                        piece->movement.velocity = glm::normalize(piece->movement.target - piece->model.position)
                                * PIECE_BASE_VELOCITY;
                    }

                    if (glm::length(piece->movement.target - piece->model.position) < 0.03f) {
                        arrive_at_node(piece);
                    }

                    break;
                }
            }
        }
    }
}

bool Board::take_piece(hoverable::Id hovered_id) {
    bool took = false;

    if (hovered_piece != nullptr) {  // Do anything only if there is a hovered piece
        for (Node& node : nodes) {
            if (node.piece != nullptr) {
                if (turn == Player::White) {
                    if (node.piece->id == hovered_id && hovered_piece->id == hovered_id &&
                            node.piece->type == Piece::Black) {
                        if (!is_windmill_made(&node, Piece::Black) ||
                                number_of_pieces_in_windmills(Piece::Black) == black_pieces_count) {
                            ASSERT(node.piece->active, "Piece must be active in the scene");
                            ASSERT(node.piece->in_use, "Piece must be in use");

                            remember_state();
                            WAIT_FOR_NEXT_MOVE();

                            take_and_raise_piece(node.piece);
                            node.piece = nullptr;
                            node.piece_id = hoverable::null;
                            should_take_piece = false;
                            update_cursor();
                            set_pieces_to_take(Piece::Black, false);
                            black_pieces_count--;
                            check_player_number_of_pieces(Player::White);
                            check_player_number_of_pieces(Player::Black);
                            switch_turn();
                            update_outlines();

                            DEB_DEBUG("Black piece {} taken", hovered_id);

                            if (check_player_blocked(turn)) {
                                DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));
                                game_over(TURN_IS_WHITE_SO(Ending::WinnerBlack, Ending::WinnerWhite),
                                        TURN_IS_WHITE_SO(Piece::White, Piece::Black));
                            }
                        } else {
                            DEB_DEBUG("Cannot take piece from windmill");
                        }

                        took = true;
                        break;
                    }
                } else {
                    if (node.piece->id == hovered_id && hovered_piece->id == hovered_id &&
                            node.piece->type == Piece::White) {
                        if (!is_windmill_made(&node, Piece::White) ||
                                number_of_pieces_in_windmills(Piece::White) == white_pieces_count) {
                            ASSERT(node.piece->active, "Piece must be active in the scene");
                            ASSERT(node.piece->in_use, "Piece must be in use");

                            remember_state();
                            WAIT_FOR_NEXT_MOVE();

                            take_and_raise_piece(node.piece);
                            node.piece = nullptr;
                            node.piece_id = hoverable::null;
                            should_take_piece = false;
                            update_cursor();
                            set_pieces_to_take(Piece::White, false);
                            white_pieces_count--;
                            check_player_number_of_pieces(Player::White);
                            check_player_number_of_pieces(Player::Black);
                            switch_turn();
                            update_outlines();

                            DEB_DEBUG("White piece {} taken", hovered_id);

                            if (check_player_blocked(turn)) {
                                DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));
                                game_over(TURN_IS_WHITE_SO(Ending::WinnerBlack, Ending::WinnerWhite),
                                        TURN_IS_WHITE_SO(Piece::White, Piece::Black));
                            }
                        } else {
                            DEB_DEBUG("Cannot take piece from windmill");
                        }

                        took = true;
                        break;
                    }
                }
            }
        }

        // Do this even if it may not be needed
        if (phase == Phase::PlacePieces && not_placed_pieces_count() == 0 && !should_take_piece) {
            phase = Phase::MovePieces;
            update_outlines();

            DEB_INFO("Phase 2");
        }
    }

    return took;
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

bool Board::put_piece(hoverable::Id hovered_id) {
    bool put = false;

    if (selected_piece != nullptr) {  // Do anything only if there is a selected piece
        for (Node& node : nodes) {
            if (node.id == hovered_id && can_go(selected_piece->node, &node)) {
                ASSERT(node.piece == nullptr, "Piece must be not null");
                remember_state();
                WAIT_FOR_NEXT_MOVE();

                if (selected_piece->type == Piece::White && can_jump[static_cast<int>(Piece::White)] ||
                        selected_piece->type == Piece::Black && can_jump[static_cast<int>(Piece::Black)]) {
                    const glm::vec3 target = glm::vec3(node.model.position.x, PIECE_Y_POSITION, node.model.position.z);
                    const glm::vec3 target0 = selected_piece->model.position + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
                    const glm::vec3 target1 = target + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
                    const glm::vec3 velocity = glm::normalize(target0 - selected_piece->model.position) * PIECE_BASE_VELOCITY;

                    prepare_piece_for_threestep_move(selected_piece, target, velocity, target0, target1);
                } else {
                    const glm::vec3 target = glm::vec3(node.model.position.x, PIECE_Y_POSITION, node.model.position.z);

                    prepare_piece_for_linear_move(
                        selected_piece,
                        target,
                        glm::normalize(target - selected_piece->model.position) * PIECE_BASE_VELOCITY
                    );
                }

                // Reset all of these
                Node* previous_node = selected_piece->node;
                previous_node->piece = nullptr;
                previous_node->piece_id = hoverable::null;
                selected_piece->node = &node;
                selected_piece->node_id = node.id;
                selected_piece->selected = false;
                node.piece = selected_piece;
                node.piece_id = selected_piece->id;
                selected_piece = nullptr;

                if (is_windmill_made(&node, TURN_IS_WHITE_SO(Piece::White, Piece::Black))) {
                    DEB_DEBUG("{} windmill is made", TURN_IS_WHITE_SO("White", "Black"));

                    should_take_piece = true;
                    update_cursor();

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
                        DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));
                        game_over(TURN_IS_WHITE_SO(Ending::WinnerBlack, Ending::WinnerWhite),
                                TURN_IS_WHITE_SO(Piece::White, Piece::Black));
                    }

                    remember_position_and_check_repetition();
                }

                put = true;
                break;
            }
        }
    }

    return put;
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

void Board::release() {
    hovered_node = nullptr;
    hovered_piece = nullptr;
}

void Board::undo() {
    ASSERT(state_history->size() > 0, "History is empty");

    Board& state = state_history->back();

    model.index_count = state.model.index_count;
    model.position = state.model.position;
    model.rotation = state.model.rotation;
    model.scale = state.model.scale;
    model.outline_color = state.model.outline_color;

    paint_model.index_count = state.paint_model.index_count;
    paint_model.position = state.paint_model.position;
    paint_model.rotation = state.paint_model.rotation;
    paint_model.scale = state.paint_model.scale;
    paint_model.outline_color = state.paint_model.outline_color;

    for (unsigned int i = 0; i < 24; i++) {
        Node& node = nodes[i];
        node.id = state.nodes[i].id;
        node.model.index_count = state.nodes[i].model.index_count;
        node.model.position = state.nodes[i].model.position;
        node.model.rotation = state.nodes[i].model.rotation;
        node.model.scale = state.nodes[i].model.scale;
        node.model.outline_color = state.nodes[i].model.outline_color;
        node.piece_id = state.nodes[i].piece_id;
        node.piece = nullptr;  // It must be NULL, if the ids don't match
        // Assign correct addresses
        for (Piece& piece : pieces) {
            if (piece.id == node.piece_id) {
                node.piece = &piece;
                break;
            }
        }
        node.index = state.nodes[i].index;
    }

    for (unsigned int i = 0; i < 18; i++) {
        Piece& piece = pieces[i];
        piece.id = state.pieces[i].id;
        piece.model.index_count = state.pieces[i].model.index_count;
        piece.model.position = state.pieces[i].model.position;
        piece.model.rotation = state.pieces[i].model.rotation;
        piece.model.scale = state.pieces[i].model.scale;
        piece.model.outline_color = state.pieces[i].model.outline_color;
        piece.movement.type = state.pieces[i].movement.type;
        piece.movement.velocity = state.pieces[i].movement.velocity;
        piece.movement.target = state.pieces[i].movement.target;
        piece.movement.target0 = state.pieces[i].movement.target0;
        piece.movement.target1 = state.pieces[i].movement.target1;
        piece.movement.reached_target0 = state.pieces[i].movement.reached_target0;
        piece.movement.reached_target1 = state.pieces[i].movement.reached_target1;
        piece.should_move = state.pieces[i].should_move;
        piece.type = state.pieces[i].type;
        piece.in_use = state.pieces[i].in_use;
        piece.node_id = state.pieces[i].node_id;
        piece.node = nullptr;  // It must be NULL, if the ids don't match
        // Assign correct addresses
        for (Node& node : nodes) {
            if (node.id == piece.node_id) {
                piece.node = &node;
                break;
            }
        }
        piece.show_outline = state.pieces[i].show_outline;
        piece.to_take = state.pieces[i].to_take;
        piece.pending_remove = false;
        piece.selected = false;
        piece.active = state.pieces[i].active;
    }

    phase = state.phase;
    turn = state.turn;
    ending = state.ending;
    white_pieces_count = state.white_pieces_count;
    black_pieces_count = state.black_pieces_count;
    not_placed_white_pieces_count = state.not_placed_white_pieces_count;
    not_placed_black_pieces_count = state.not_placed_black_pieces_count;
    should_take_piece = state.should_take_piece;
    hovered_node = nullptr;
    hovered_piece = nullptr;
    selected_piece = nullptr;
    can_jump = state.can_jump;
    turns_without_mills = state.turns_without_mills;
    repetition_history = state.repetition_history;
    state_history = state.state_history;
    next_move = state.next_move;

    for (Piece& piece : pieces) {
        app->renderer->remove_model(piece.model.handle);

        if (piece.active) {
            app->renderer->add_model(piece.model, Renderer::CastShadow | Renderer::HasShadow);
        }
    }

    state_history->pop_back();

    DEB_DEBUG("Popped state and undid move");

    update_cursor();
}

unsigned int Board::not_placed_pieces_count() {
    return not_placed_white_pieces_count + not_placed_black_pieces_count;
}

void Board::finalize_pieces_state() {
    GET_ACTIVE_PIECES(active_pieces)

    for (Piece* piece : active_pieces) {
        if (piece->should_move) {
            arrive_at_node(piece);
        }
    }
}

void Board::update_cursor() {
    if (app->options.custom_cursor) {
        if (should_take_piece) {
            app->window->set_cursor(app->cross_cursor);
        } else {
            app->window->set_cursor(app->arrow_cursor);
        }
    }
}

void Board::update_nodes(hoverable::Id hovered_id) {
    for (Node& node : nodes) {
        if (node.id == hovered_id && phase != Board::Phase::None && phase != Board::Phase::GameOver) {
            node.model.material->set_vec4("u_color", glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
        } else {
            node.model.material->set_vec4("u_color", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        }
    }
}

void Board::update_pieces(hoverable::Id hovered_id) {
    GET_ACTIVE_PIECES(active_pieces)

    for (Piece* piece : active_pieces) {
        if (piece->selected) {
            piece->model.outline_color = glm::vec3(1.0f, 0.0f, 0.0f);
        } else if (piece->show_outline && piece->id == hovered_id && piece->in_use && !piece->pending_remove) {
            piece->model.outline_color = glm::vec3(1.0f, 0.5f, 0.0f);
        } else if (piece->to_take && piece->id == hovered_id && piece->in_use) {
            piece->model.material->set_vec3("u_material.tint", glm::vec3(1.0f, 0.2f, 0.2f));
        } else {
            piece->model.material->set_vec3("u_material.tint", glm::vec3(1.0f, 1.0f, 1.0f));
        }

        if (piece->selected || piece->show_outline && piece->id == hovered_id && piece->in_use && !piece->pending_remove) {
            if (!piece->renderer_with_outline) {
                app->renderer->update_model(piece->model, Renderer::WithOutline | Renderer::CastShadow | Renderer::HasShadow);
                piece->renderer_with_outline = true;
            }
        } else {
            if (piece->renderer_with_outline) {
                app->renderer->update_model(piece->model, Renderer::CastShadow | Renderer::HasShadow);
                piece->renderer_with_outline = false;
            }
        }
    }
}

Piece* Board::place_new_piece(Piece::Type type, float x_pos, float z_pos, Node* node) {
    GET_ACTIVE_PIECES(active_pieces)

    for (Piece* piece : active_pieces) {
        if (!piece->in_use && piece->type == type) {
            const glm::vec3 target = glm::vec3(x_pos, PIECE_Y_POSITION, z_pos);
            const glm::vec3 target0 = piece->model.position + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
            const glm::vec3 target1 = target + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
            const glm::vec3 velocity = glm::normalize(target0 - piece->model.position)
                    * PIECE_BASE_VELOCITY;

            prepare_piece_for_threestep_move(piece, target, velocity, target0, target1);

            piece->in_use = true;
            piece->node = node;
            piece->node_id = node->id;

            return piece;
        }
    }

    ASSERT(false, "Couldn't find a piece");
    return nullptr;
}

void Board::take_and_raise_piece(Piece* piece) {
    prepare_piece_for_linear_move(
        piece,
        glm::vec3(piece->model.position.x, PIECE_Y_POSITION + PIECE_RAISE_HEIGHT, piece->model.position.z),
        glm::vec3(0.0f)
    );

    piece->node = nullptr;
    piece->node_id = hoverable::null;
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
            DEB_INFO("Game over, white wins");
            break;
        case Ending::WinnerBlack:
            DEB_INFO("Game over, black wins");
            break;
        case Ending::TieBetweenBothPlayers:
            DEB_INFO("Game over, tie between both players");
            break;
        default:
            ASSERT(false, "Invalid ending");
    }
}

void Board::switch_turn() {
    if (phase == Phase::MovePieces) {
        turns_without_mills++;

        if (turns_without_mills == MAX_TURNS_WITHOUT_MILLS) {
            DEB_INFO("The max amount of turns without mills has been hit");
            game_over(Ending::TieBetweenBothPlayers,
                    TURN_IS_WHITE_SO(Piece::White, Piece::Black));
        }
    }

    turn = TURN_IS_WHITE_SO(Player::Black, Player::White);
}

bool Board::is_windmill_made(Node* node, Piece::Type type) {
    for (unsigned int i = 0; i < 16; i++) {
        const unsigned int* mill = WINDMILLS[i];

        const Node& node1 = nodes[mill[0]];
        const Node& node2 = nodes[mill[1]];
        const Node& node3 = nodes[mill[2]];

        if (node1.piece != nullptr && node2.piece != nullptr && node3.piece != nullptr) {
            const Piece* piece1 = node1.piece;
            const Piece* piece2 = node2.piece;
            const Piece* piece3 = node3.piece;

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

        const Node& node1 = nodes[mill[0]];
        const Node& node2 = nodes[mill[1]];
        const Node& node3 = nodes[mill[2]];

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

    DEB_DEBUG("Number of {} pieces in mills: {}", type == Piece::White ? "white" : "black",
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
    ASSERT(source_node != destination_node, "Source must be different than destination");

    if (can_jump[static_cast<int>(turn)]) {
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
        DEB_DEBUG("Checking white player number of pieces");

        if (white_pieces_count + not_placed_white_pieces_count == 3) {
            can_jump[static_cast<int>(player)] = true;
            DEB_INFO("White player can jump");
        } else if (white_pieces_count + not_placed_white_pieces_count == 2) {
            DEB_INFO("White player has only 2 pieces");
            game_over(Ending::WinnerBlack, Piece::White);
        }
    } else {
        DEB_DEBUG("Checking black player number of pieces");

        if (black_pieces_count + not_placed_black_pieces_count == 3) {
            can_jump[static_cast<int>(player)] = true;
            DEB_INFO("Black player can jump");
        } else if (black_pieces_count + not_placed_black_pieces_count == 2) {
            DEB_INFO("Black player has only 2 pieces");
            game_over(Ending::WinnerWhite, Piece::Black);
        }
    }
}

bool Board::check_player_blocked(Player player) {
    DEB_DEBUG("{} player is checked if is blocked",
            player == Player::White ? "White" : "Black");

    bool at_least_one_piece = false;
    Piece::Type type = player == Player::White ? Piece::White : Piece::Black;

    if (can_jump[static_cast<int>(player)]) {
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
            DEB_INFO("Threefold repetition");
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

void Board::remember_state() {
    state_history->push_back(*this);

    DEB_DEBUG("Pushed new state");
}

void Board::arrive_at_node(Piece* piece) {
    piece->model.position = piece->movement.target;

    // Reset all these movement variables
    piece->should_move = false;
    memset(&piece->movement, 0, sizeof(Piece::Movement));

    // Remove piece if set to remove
    if (piece->pending_remove) {
        piece->active = false;
        app->renderer->remove_model(piece->model.handle);
    }

    CAN_MAKE_MOVE();
}

void Board::prepare_piece_for_linear_move(Piece* piece, const glm::vec3& target, const glm::vec3& velocity) {
    piece->movement.target = target;

    piece->movement.velocity = velocity;

    piece->movement.type = Piece::MovementType::Linear;
    piece->should_move = true;
}

void Board::prepare_piece_for_threestep_move(Piece* piece, const glm::vec3& target,
        const glm::vec3& velocity, const glm::vec3& target0, const glm::vec3& target1) {
    piece->movement.target = target;

    piece->movement.velocity = velocity;
    piece->movement.target0 = target0;
    piece->movement.target1 = target1;

    piece->movement.type = Piece::MovementType::ThreeStep;
    piece->should_move = true;
}
