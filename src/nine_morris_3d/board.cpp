#include <glm/glm.hpp>

#include "nine_morris_3d/board.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/keyboard_controls.h"
#include "other/logging.h"
#include "other/assert.h"

#define GET_ACTIVE_PIECES(result) \
    std::array<Piece*, 18> pointer_pieces; \
    for (size_t i = 0; i < 18; i++) { \
        pointer_pieces[i] = &pieces[i]; \
    } \
    std::vector<Piece*> result; \
    std::copy_if(pointer_pieces.begin(), pointer_pieces.end(), std::back_inserter(result), [](const Piece* piece) { \
        return piece->active; \
    });

// format is the first argument to __VA_ARGS__
#define FORMATTED_MESSAGE(result, size, ...) \
    char result[size]; \
    sprintf(result, __VA_ARGS__);

#define TURN_IS_WHITE_SO(_true, _false) (turn == Player::White ? _true : _false)
#define WAIT_FOR_NEXT_MOVE() next_move = false
#define CAN_MAKE_MOVE() next_move = true

constexpr unsigned int WINDMILLS[16][3] = {
    { 0, 1, 2 }, { 2, 14, 23 }, { 21, 22, 23 }, { 0, 9, 21 },
    { 3, 4, 5 }, { 5, 13, 20 }, { 18, 19, 20 }, { 3, 10, 18 },
    { 6, 7, 8 }, { 8, 12, 17 }, { 15, 16, 17 }, { 6, 11, 15 },
    { 1, 4, 7 }, { 12, 13, 14 }, { 16, 19, 22 }, { 9, 10, 11 }
};

Board::Board(StateHistory& state_history) {
    undo_state_history = &state_history.undo_state_history;
    redo_state_history = &state_history.redo_state_history;
}

void Board::copy_smart(Board& to, const Board& from, bool state_history_inclusive) {
    to.model.index_count = from.model.index_count;
    to.model.position = from.model.position;
    to.model.rotation = from.model.rotation;
    to.model.scale = from.model.scale;
    to.model.outline_color = from.model.outline_color;

    to.paint_model.index_count = from.paint_model.index_count;
    to.paint_model.position = from.paint_model.position;
    to.paint_model.rotation = from.paint_model.rotation;
    to.paint_model.scale = from.paint_model.scale;
    to.paint_model.outline_color = from.paint_model.outline_color;

    for (size_t i = 0; i < 24; i++) {
        Node& node = to.nodes[i];
        node.id = from.nodes[i].id;
        node.model.index_count = from.nodes[i].model.index_count;
        node.model.position = from.nodes[i].model.position;
        node.model.rotation = from.nodes[i].model.rotation;
        node.model.scale = from.nodes[i].model.scale;
        node.model.outline_color = from.nodes[i].model.outline_color;
        node.piece_id = from.nodes[i].piece_id;
        node.piece = nullptr;
        node.index = from.nodes[i].index;
    }

    for (size_t i = 0; i < 18; i++) {
        Piece& piece = to.pieces[i];
        piece.id = from.pieces[i].id;
        piece.model.index_count = from.pieces[i].model.index_count;
        piece.model.position = from.pieces[i].model.position;
        piece.model.rotation = from.pieces[i].model.rotation;
        piece.model.scale = from.pieces[i].model.scale;
        piece.model.outline_color = from.pieces[i].model.outline_color;
        piece.movement = from.pieces[i].movement;
        piece.should_move = from.pieces[i].should_move;
        piece.type = from.pieces[i].type;
        piece.in_use = from.pieces[i].in_use;
        piece.node_id = from.pieces[i].node_id;
        piece.node = nullptr;
        piece.show_outline = from.pieces[i].show_outline;
        piece.to_take = from.pieces[i].to_take;
        piece.pending_remove = false;
        piece.selected = false;
        piece.active = from.pieces[i].active;
        piece.renderer_with_outline =from.pieces[i].renderer_with_outline;
    }

    to.phase = from.phase;
    to.turn = from.turn;
    to.ending = from.ending;
    to.ending_message = from.ending_message;
    to.white_pieces_count = from.white_pieces_count;
    to.black_pieces_count = from.black_pieces_count;
    to.not_placed_white_pieces_count = from.not_placed_white_pieces_count;
    to.not_placed_black_pieces_count = from.not_placed_black_pieces_count;
    to.should_take_piece = from.should_take_piece;
    to.hovered_node = nullptr;
    to.hovered_piece = nullptr;
    to.selected_piece = nullptr;
    to.can_jump = from.can_jump;
    to.turns_without_mills = from.turns_without_mills;
    to.repetition_history = from.repetition_history;
    if (state_history_inclusive) {
        to.undo_state_history = from.undo_state_history;
        to.redo_state_history = from.redo_state_history;
    }
    to.next_move = true;

    // Assign correct addresses
    for (Node& node : to.nodes) {
        for (Piece& piece : to.pieces) {
            if (node.piece_id == piece.id) {
                node.piece = &piece;
                break;
            }
        }
    }
    for (Piece& piece : to.pieces) {
        for (Node& node : to.nodes) {
            if (piece.node_id == node.id) {
                piece.node = &node;
                break;
            }
        }
    }
}

bool Board::place_piece(hoverable::Id hovered_id) {
    bool placed = false;

    for (Node& node : nodes) {
        if (node.id == hovered_id && (&node) == hovered_node && node.piece == nullptr) {
            remember_state();
            WAIT_FOR_NEXT_MOVE();

            const glm::vec3& position = node.model.position;

            if (turn == Player::White) {
                node.piece = new_piece_to_place(Piece::White, position.x, position.z, &node);
                node.piece_id = node.piece->id;
                white_pieces_count++;
                not_placed_white_pieces_count--;
            } else {
                node.piece = new_piece_to_place(Piece::Black, position.x, position.z, &node);
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
            } else {
                check_player_number_of_pieces(turn);
                switch_turn();

                if (not_placed_pieces_count() == 0) {
                    phase = Phase::MovePieces;
                    update_outlines();

                    DEB_INFO("Phase 2");

                    if (is_player_blocked(turn)) {
                        DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

                        FORMATTED_MESSAGE(
                            message, 64, "%s player has blocked %s player.",
                            TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("white", "black")
                        )

                        game_over(
                            TURN_IS_WHITE_SO(Ending::WinnerBlack, Ending::WinnerWhite),
                            TURN_IS_WHITE_SO(Piece::White, Piece::Black),
                            message
                        );
                    }
                }
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
        GET_ACTIVE_PIECES(active_pieces)

        for (Piece* piece : active_pieces) {
            if (turn == Player::White) {
                if (piece->id == hovered_id && hovered_piece->id == hovered_id &&
                        piece->type == Piece::Black) {
                    if (!is_windmill_made(piece->node, Piece::Black) ||
                            number_of_pieces_in_windmills(Piece::Black) == black_pieces_count) {
                        ASSERT(piece->in_use, "Piece must be in use");

                        remember_state();
                        WAIT_FOR_NEXT_MOVE();

                        piece->node->piece = nullptr;
                        piece->node->piece_id = hoverable::null;
                        take_and_raise_piece(piece);
                        should_take_piece = false;
                        update_cursor();
                        set_pieces_to_take(Piece::Black, false);
                        black_pieces_count--;
                        check_player_number_of_pieces(Player::Black);
                        check_player_number_of_pieces(Player::White);
                        switch_turn();
                        update_outlines();

                        DEB_DEBUG("Black piece {} taken", hovered_id);

                        if (is_player_blocked(turn)) {
                            DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

                            FORMATTED_MESSAGE(
                                message, 64, "%s player has blocked %s player.",
                                TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("White", "Black")
                            )

                            game_over(
                                TURN_IS_WHITE_SO(Ending::WinnerBlack, Ending::WinnerWhite),
                                TURN_IS_WHITE_SO(Piece::White, Piece::Black),
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
                if (piece->id == hovered_id && hovered_piece->id == hovered_id &&
                        piece->type == Piece::White) {
                    if (!is_windmill_made(piece->node, Piece::White) ||
                            number_of_pieces_in_windmills(Piece::White) == white_pieces_count) {
                        ASSERT(piece->in_use, "Piece must be in use");

                        remember_state();
                        WAIT_FOR_NEXT_MOVE();

                        piece->node->piece = nullptr;
                        piece->node->piece_id = hoverable::null;
                        take_and_raise_piece(piece);
                        should_take_piece = false;
                        update_cursor();
                        set_pieces_to_take(Piece::White, false);
                        white_pieces_count--;
                        check_player_number_of_pieces(Player::White);
                        check_player_number_of_pieces(Player::Black);
                        switch_turn();
                        update_outlines();

                        DEB_DEBUG("White piece {} taken", hovered_id);

                        if (is_player_blocked(turn)) {
                            DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

                            FORMATTED_MESSAGE(
                                message, 64, "%s player has blocked %s player.",
                                TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("white", "black")
                            )

                            game_over(
                                TURN_IS_WHITE_SO(Ending::WinnerBlack, Ending::WinnerWhite),
                                TURN_IS_WHITE_SO(Piece::White, Piece::Black),
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
        if (not_placed_pieces_count() == 0 && !should_take_piece && phase != Phase::GameOver) {
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

bool Board::put_down_piece(hoverable::Id hovered_id) {
    bool put = false;

    // Save the selected piece and the node from where it was
    Piece* piece_put = nullptr;
    Node* node_from = nullptr;

    if (selected_piece != nullptr) {  // Do anything only if there is a selected piece
        for (Node& node : nodes) {
            if (node.id == hovered_id && can_go(selected_piece->node, &node)) {
                ASSERT(node.piece == nullptr, "Piece must not be null");
                remember_state();
                WAIT_FOR_NEXT_MOVE();

                if (selected_piece->type == Piece::White && can_jump[static_cast<int>(Piece::White)] ||
                        selected_piece->type == Piece::Black && can_jump[static_cast<int>(Piece::Black)]) {
                    const glm::vec3 target = glm::vec3(node.model.position.x, PIECE_Y_POSITION, node.model.position.z);
                    const glm::vec3 target0 = selected_piece->model.position + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
                    const glm::vec3 target1 = target + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
                    const glm::vec3 velocity = glm::normalize(target0 - selected_piece->model.position) * PIECE_BASE_VELOCITY;

                    prepare_piece_for_three_step_move(selected_piece, target, velocity, target0, target1);
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

                node_from = selected_piece->node;
                selected_piece->node = &node;
                selected_piece->node_id = node.id;
                selected_piece->selected = false;
                node.piece = selected_piece;
                node.piece_id = selected_piece->id;

                piece_put = selected_piece;
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
                    turns_without_mills++;
                    switch_turn();
                    update_outlines();

                    if (is_player_blocked(turn)) {
                        DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

                        FORMATTED_MESSAGE(
                            message, 64, "%s player has blocked %s player.",
                            TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("white", "black")
                        )

                        game_over(
                            TURN_IS_WHITE_SO(Ending::WinnerBlack, Ending::WinnerWhite),
                            TURN_IS_WHITE_SO(Piece::White, Piece::Black),
                            message
                        );
                    }

                    remember_position_and_check_repetition(piece_put, node_from);
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

bool Board::undo() {
    ASSERT(!undo_state_history->empty(), "Undo history must not be empty");

    if (!next_move) {
        DEB_WARN("Cannot do anything when pieces are in air");
        return false;
    }

    const bool undo_game_over = phase == Phase::None;

    Board previous_state = *this;
    Board& state_board = undo_state_history->back();

    copy_smart(*this, state_board, false);

    // Reset pieces' models
    for (Piece& piece : pieces) {
        app->renderer->remove_model(piece.model.handle);

        if (piece.active) {
            app->renderer->add_model(piece.model, Renderer::CastShadow | Renderer::HasShadow);
        }
    }

    undo_state_history->pop_back();
    redo_state_history->push_back(previous_state);

    DEB_DEBUG("Popped state off of undo stack and undid move");

    update_cursor();

    return undo_game_over;
}

bool Board::redo() {
    ASSERT(!redo_state_history->empty(), "Redo history must not be empty");

    if (!next_move) {
        DEB_WARN("Cannot do anything when pieces are in air");
        return false;
    }

    Board previous_state = *this;
    Board& state_board = redo_state_history->back();

    copy_smart(*this, state_board, false);

    // Reset pieces' models
    for (Piece& piece : pieces) {
        app->renderer->remove_model(piece.model.handle);

        if (piece.active) {
            app->renderer->add_model(piece.model, Renderer::CastShadow | Renderer::HasShadow);
        }
    }

    redo_state_history->pop_back();
    undo_state_history->push_back(previous_state);

    DEB_DEBUG("Popped state off of redo stack and redid move");

    update_cursor();

    return phase == Phase::None;
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

            if (keyboard != nullptr) {
                keyboard->quad.texture = app->data.keyboard_controls_texture_cross;
            }
        } else {
            app->window->set_cursor(app->arrow_cursor);

            if (keyboard != nullptr) {
                keyboard->quad.texture = app->data.keyboard_controls_texture;
            }
        }
    }
}

void Board::update_nodes(hoverable::Id hovered_id) {
    for (Node& node : nodes) {
        if (node.id == hovered_id && phase != Board::Phase::None && phase != Board::Phase::GameOver
                && !should_take_piece) {
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

std::string_view Board::get_ending_message() {
    ASSERT(ending_message != "", "Ending message cannot be empty");

    return ending_message;
}

Piece* Board::new_piece_to_place(Piece::Type type, float x_pos, float z_pos, Node* node) {
    GET_ACTIVE_PIECES(active_pieces)

    for (Piece* piece : active_pieces) {
        if (!piece->in_use && piece->type == type) {
            const glm::vec3 target = glm::vec3(x_pos, PIECE_Y_POSITION, z_pos);
            const glm::vec3 target0 = piece->model.position + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
            const glm::vec3 target1 = target + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
            const glm::vec3 velocity = glm::normalize(target0 - piece->model.position)
                    * PIECE_BASE_VELOCITY;

            prepare_piece_for_three_step_move(piece, target, velocity, target0, target1);

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

void Board::game_over(Ending ending, Piece::Type type_to_hide, std::string_view ending_message) {
    phase = Phase::GameOver;
    this->ending = ending;
    this->ending_message = ending_message;
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
        if (turns_without_mills == MAX_TURNS_WITHOUT_MILLS) {
            DEB_INFO("The max amount of turns without mills has been hit");

            FORMATTED_MESSAGE(
                message, 64, "%u turns have passed without a windmill made.",
                MAX_TURNS_WITHOUT_MILLS
            )

            game_over(
                Ending::TieBetweenBothPlayers,
                TURN_IS_WHITE_SO(Piece::White, Piece::Black),
                message
            );
        }
    }

    turn = TURN_IS_WHITE_SO(Player::Black, Player::White);
}

bool Board::is_windmill_made(Node* node, Piece::Type type) {
    for (size_t i = 0; i < 16; i++) {
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

    for (size_t i = 0; i < 16; i++) {
        const unsigned int* mill = WINDMILLS[i];

        const Node& node1 = nodes[mill[0]];
        const Node& node2 = nodes[mill[1]];
        const Node& node3 = nodes[mill[2]];

        if (node1.piece != nullptr && node2.piece != nullptr && node3.piece != nullptr) {
            Piece* piece1 = node1.piece;
            Piece* piece2 = node2.piece;
            Piece* piece3 = node3.piece;

            if (piece1->type == type && piece2->type == type && piece3->type == type) {
                std::vector<Piece*>::iterator iter;

                iter = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), node1.piece);
                if (iter == pieces_inside_mills.end()) {
                    pieces_inside_mills.push_back(node1.piece);
                }
                iter = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), node2.piece);
                if (iter == pieces_inside_mills.end()) {
                    pieces_inside_mills.push_back(node2.piece);
                }
                iter = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), node3.piece);
                if (iter == pieces_inside_mills.end()) {
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

            FORMATTED_MESSAGE(
                message, 64, "White player cannot make any more windmills."
            )

            game_over(Ending::WinnerBlack, Piece::White, message);
        }
    } else {
        DEB_DEBUG("Checking black player number of pieces");

        if (black_pieces_count + not_placed_black_pieces_count == 3) {
            can_jump[static_cast<int>(player)] = true;

            DEB_INFO("Black player can jump");
        } else if (black_pieces_count + not_placed_black_pieces_count == 2) {
            DEB_INFO("Black player has only 2 pieces");

            FORMATTED_MESSAGE(
                message, 64, "Black player cannot make any more windmills."
            )

            game_over(Ending::WinnerWhite, Piece::Black, message);
        }
    }
}

bool Board::is_player_blocked(Player player) {
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

Board::GamePosition Board::get_position() {
    GamePosition position;

    for (size_t i = 0; i < 24; i++) {
        Node& node = nodes[i];

        if (node.piece != nullptr) {
            position[i] = node.piece->type;
        } else {
            position[i] = Piece::None;
        }
    }

    return position;
}

void Board::remember_position_and_check_repetition(Piece* piece, Node* node) {
    using Position = ThreefoldRepetitionHistory::PositionPlusInfo;

    ASSERT(piece != nullptr, "Piece must not be null");
    ASSERT(node != nullptr, "Node must not be null");

    const Position current_position = { get_position(), piece->id, node->id };

    for (const Position& position : repetition_history.twos) {
        if (position == current_position) {
            DEB_INFO("Threefold repetition");

            FORMATTED_MESSAGE(
                message, 64, "%s player has made threefold repetition.",
                TURN_IS_WHITE_SO("Black", "White")
            )

            game_over(Ending::TieBetweenBothPlayers, TURN_IS_WHITE_SO(Piece::White, Piece::Black), message);
            return;
        }
    }

    for (const Position& position : repetition_history.ones) {
        if (position == current_position) {
            std::vector<Position>& ones = repetition_history.ones;

            auto iter = std::find(ones.begin(), ones.end(), position);
            ASSERT(iter != ones.end(), "That should be impossible");

            // This invalidates repetition_history.ones, but it's okay, because we return
            ones.erase(iter);

            // Insert current_position, because position is invalidated
            repetition_history.twos.push_back(current_position);
            return;
        }
    }

    repetition_history.ones.push_back(current_position);
}

void Board::remember_state() {
    undo_state_history->push_back(*this);
    redo_state_history->clear();

    DEB_DEBUG("Pushed new state");
}

void Board::arrive_at_node(Piece* piece) {
    piece->model.position = piece->movement.target;

    // Reset all these movement variables
    piece->should_move = false;
    memset(&piece->movement, 0, sizeof(Piece::Movement));

    // Remove piece if set to remove
    if (piece->pending_remove) {
        piece->pending_remove = false;
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

void Board::prepare_piece_for_three_step_move(Piece* piece, const glm::vec3& target,
        const glm::vec3& velocity, const glm::vec3& target0, const glm::vec3& target1) {
    piece->movement.target = target;

    piece->movement.velocity = velocity;
    piece->movement.target0 = target0;
    piece->movement.target1 = target1;

    piece->movement.type = Piece::MovementType::ThreeStep;
    piece->should_move = true;
}
