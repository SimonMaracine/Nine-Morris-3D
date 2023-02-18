#include <engine/engine_graphics.h>
#include <engine/engine_other.h>

#include "game/entities/board.h"
#include "game/entities/node.h"
#include "game/entities/piece.h"
#include "game/piece_movement.h"
#include "game/game_position.h"
#include "other/constants.h"

GamePosition Board::get_position() {
    GamePosition position;

    for (const Node& node : nodes) {
        if (node.piece_index != NULL_INDEX) {
            position.at(node.index) = pieces.at(node.piece_index).type;
        } else {
            position.at(node.index) = PieceType::None;
        }
    }

    position.turns = turn_count;
    position.white_pieces_on_board = white_pieces_count;
    position.black_pieces_on_board = black_pieces_count;
    position.white_pieces_outside = not_placed_white_pieces_count;
    position.black_pieces_outside = not_placed_black_pieces_count;

    return position;
}

void Board::update_nodes(identifier::Id hovered_id) {
    for (Node& node : nodes) {
        const bool hovered = node.model->bounding_box->id == hovered_id;
        const bool highlight = (
            phase == BoardPhase::PlacePieces || (phase == BoardPhase::MovePieces && selected_piece_index != NULL_INDEX)
        );
        const bool permitted = !must_take_piece && is_players_turn;

        if (hovered && highlight && permitted) {
            node.model->material->set_vec4("u_color"_H, NODE_COLOR);
        } else {
            node.model->material->set_vec4("u_color"_H, glm::vec4(0.0f));
        }
    }
}

void Board::update_pieces(identifier::Id hovered_id) {
    for (auto& [_, piece] : pieces) {
        const bool hovered = piece.model->bounding_box->id == hovered_id;

        if (piece.selected) {
            piece.model->outline_color = std::make_optional<glm::vec3>(RED_OUTLINE);
        } else if (piece.show_outline && hovered && piece.in_use && !piece.pending_remove) {
            piece.model->outline_color = std::make_optional<glm::vec3>(ORANGE_OUTLINE);
        } else {
            piece.model->outline_color = std::nullopt;
        }

        if (piece.to_take && hovered && piece.in_use) {
            piece.model->material->set_vec3("u_material.tint"_H, RED_TINT);
        } else {
            piece.model->material->set_vec3("u_material.tint"_H, DEFAULT_TINT);
        }
    }
}

void Board::move_pieces() {
    const float dt = app->get_delta();

    std::vector<size_t> to_erase;

    for (auto& [index, piece] : pieces) {
        if (!piece.movement.moving) {
            continue;
        }

        switch (piece.movement.type) {
            case PieceMovementType::None:
                ASSERT(false, "Movement type None is invalid");
                break;
            case PieceMovementType::Linear: {
                piece.model->position += (
                    piece.movement.velocity * dt + (piece.movement.target - piece.model->position)
                            * PIECE_VARIABLE_VELOCITY * dt
                );
                piece.source->set_position(piece.model->position);

                if (glm::length(piece.movement.target - piece.model->position) < 0.03f) {
                    to_erase.push_back(index);
                }

                break;
            }
            case PieceMovementType::ThreeStep: {
                if (!piece.movement.reached_target0) {
                    piece.model->position += (
                        piece.movement.velocity * dt + (piece.movement.target0 - piece.model->position)
                                * PIECE_VARIABLE_VELOCITY * dt
                    );
                } else if (!piece.movement.reached_target1) {
                    piece.model->position += (
                        piece.movement.velocity * dt + (piece.movement.target1 - piece.model->position)
                                * PIECE_VARIABLE_VELOCITY * dt
                    );
                } else {
                    piece.model->position += (
                        piece.movement.velocity * dt + (piece.movement.target - piece.model->position)
                                * PIECE_VARIABLE_VELOCITY * dt
                    );
                }

                piece.source->set_position(piece.model->position);

                if (!piece.movement.reached_target0 && glm::length(piece.movement.target0 - piece.model->position) < 0.03f) {
                    piece.movement.reached_target0 = true;
                    piece.model->position = piece.movement.target0;
                    piece.movement.velocity = (
                        glm::normalize(piece.movement.target1 - piece.model->position) * PIECE_BASE_VELOCITY
                    );
                } else if (!piece.movement.reached_target1 && glm::length(piece.movement.target1 - piece.model->position) < 0.03f) {
                    piece.movement.reached_target1 = true;
                    piece.model->position = piece.movement.target1;
                    piece.movement.velocity = (
                        glm::normalize(piece.movement.target - piece.model->position) * PIECE_BASE_VELOCITY
                    );
                }

                if (glm::length(piece.movement.target - piece.model->position) < 0.03f) {
                    to_erase.push_back(index);  // Reached target
                }

                break;
            }
        }
    }

    for (size_t index : to_erase) {
        piece_arrive_at_node(index);
    }
}

void Board::finalize_pieces_state() {
    std::vector<size_t> to_erase;

    for (const auto& [index, piece] : pieces) {
        if (piece.movement.moving) {
            to_erase.push_back(index);
        }
    }

    for (size_t index : to_erase) {
        piece_arrive_at_node(index);
    }
}

void Board::update_piece_outlines() {
    if (phase == BoardPhase::MovePieces) {
        if (turn == BoardPlayer::White) {
            set_pieces_show_outline(PieceType::White, true);
            set_pieces_show_outline(PieceType::Black, false);
        } else {
            set_pieces_show_outline(PieceType::Black, true);
            set_pieces_show_outline(PieceType::White, false);
        }
    }
}

size_t Board::new_piece_to_place(PieceType type, float x_pos, float z_pos, size_t node_index) {
    ASSERT(node_index != NULL_INDEX, "Invalid index");

    for (auto& [index, piece] : pieces) {
        if (!piece.in_use && piece.type == type) {
            const auto target = glm::vec3(x_pos, PIECE_Y_POSITION, z_pos);
            const auto target0 = piece.model->position + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
            const auto target1 = target + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
            const auto velocity = glm::normalize(target0 - piece.model->position) * PIECE_BASE_VELOCITY;

            prepare_piece_for_three_step_move(index, target, velocity, target0, target1);

            piece.in_use = true;
            piece.node_index = node_index;

            return index;
        }
    }

    ASSERT(false, "Could not find a piece");
    return NULL_INDEX;
}

void Board::take_and_raise_piece(size_t piece_index) {
    ASSERT(piece_index != NULL_INDEX, "Invalid index");

    Piece& piece = pieces.at(piece_index);

    play_piece_take_sound(piece_index);

    piece.node_index = NULL_INDEX;
    piece.pending_remove = true;

    prepare_piece_for_linear_move(
        piece_index,
        glm::vec3(piece.model->position.x, PIECE_Y_POSITION + PIECE_RAISE_HEIGHT, piece.model->position.z),
        glm::vec3(0.0f)
    );
}

void Board::select_piece(size_t piece_index) {
    ASSERT(piece_index != NULL_INDEX, "Invalid index");

    Piece& piece = pieces.at(piece_index);

    if (!piece.selected && !piece.pending_remove) {
        selected_piece_index = piece_index;
        piece.selected = true;
        unselect_other_pieces(piece_index);
    } else {
        selected_piece_index = NULL_INDEX;
        piece.selected = false;
    }
}

void Board::set_pieces_show_outline(PieceType type, bool show) {
    for (auto& [_, piece] : pieces) {
        if (piece.type == type) {
            piece.show_outline = show;
        }
    }
}

void Board::set_pieces_to_take(PieceType type, bool take) {
    for (auto& [_, piece] : pieces) {
        if (piece.type == type) {
            piece.to_take = take;
        }
    }
}

void Board::game_over(const BoardEnding& ending) {
    phase = BoardPhase::GameOver;
    this->ending = ending;

    set_pieces_show_outline(PieceType::White, false);  // Just hide both
    set_pieces_show_outline(PieceType::Black, false);

    switch (ending.type) {
        case BoardEnding::WinnerWhite:
            DEB_INFO("Game over, white wins");
            break;
        case BoardEnding::WinnerBlack:
            DEB_INFO("Game over, black wins");
            break;
        case BoardEnding::TieBetweenBothPlayers:
            DEB_INFO("Game over, tie between both players");
            break;
        default:
            ASSERT(false, "Invalid ending");
    }
}

bool Board::is_mill_made(size_t node_index, PieceType type, const size_t mills[][3], size_t mills_count) {
    ASSERT(node_index != NULL_INDEX, "Invalid index");

    for (size_t i = 0; i < mills_count; i++) {
        const size_t* mill = mills[i];

        const Node& node1 = nodes.at(mill[0]);
        const Node& node2 = nodes.at(mill[1]);
        const Node& node3 = nodes.at(mill[2]);

        if (node1.piece_index != NULL_INDEX && node2.piece_index != NULL_INDEX && node3.piece_index != NULL_INDEX) {
            const Piece& piece1 = pieces.at(node1.piece_index);
            const Piece& piece2 = pieces.at(node2.piece_index);
            const Piece& piece3 = pieces.at(node3.piece_index);

            if (piece1.type == type && piece2.type == type && piece3.type == type) {
                if (piece1.node_index == node_index || piece2.node_index == node_index || piece3.node_index == node_index) {
                    return true;
                }
            }
        }
    }

    return false;
}

size_t Board::number_of_pieces_in_mills(PieceType type, const size_t mills[][3], size_t mills_count) {
    std::vector<size_t> pieces_inside_mills;

    for (size_t i = 0; i < mills_count; i++) {
        const size_t* mill = mills[i];

        const Node& node1 = nodes.at(mill[0]);
        const Node& node2 = nodes.at(mill[1]);
        const Node& node3 = nodes.at(mill[2]);

        if (node1.piece_index != NULL_INDEX && node2.piece_index != NULL_INDEX && node3.piece_index != NULL_INDEX) {
            const Piece& piece1 = pieces.at(node1.piece_index);
            const Piece& piece2 = pieces.at(node2.piece_index);
            const Piece& piece3 = pieces.at(node3.piece_index);

            if (piece1.type == type && piece2.type == type && piece3.type == type) {
                std::vector<size_t>::iterator iter;

                iter = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), piece1.index);
                if (iter == pieces_inside_mills.end()) {
                    pieces_inside_mills.push_back(piece1.index);
                }

                iter = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), piece2.index);
                if (iter == pieces_inside_mills.end()) {
                    pieces_inside_mills.push_back(piece2.index);
                }

                iter = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), piece3.index);
                if (iter == pieces_inside_mills.end()) {
                    pieces_inside_mills.push_back(piece3.index);
                }
            }
        }
    }

    DEB_DEBUG(
        "Number of {} pieces in mills: {}",
        type == PieceType::White ? "white" : "black",
        pieces_inside_mills.size()
    );

    return pieces_inside_mills.size();
}

void Board::unselect_other_pieces(size_t currently_selected_piece_index) {
    ASSERT(currently_selected_piece_index != NULL_INDEX, "Invalid index");

    for (auto& [_, piece] : pieces) {
        if (piece.index != currently_selected_piece_index) {
            piece.selected = false;
        }
    }
}

void Board::play_piece_place_sound(size_t piece_index) {
    Piece& piece = pieces.at(piece_index);

    const auto choice = random_gen::choice({ "piece_place1"_H, "piece_place2"_H });
    piece.source->play(app->res.al_buffer[choice].get());
}

void Board::play_piece_move_sound(size_t piece_index) {
    Piece& piece = pieces.at(piece_index);

    const auto choice = random_gen::choice({ "piece_move1"_H, "piece_move2"_H });
    piece.source->play(app->res.al_buffer[choice].get());
}

void Board::play_piece_take_sound(size_t piece_index) {
    Piece& piece = pieces.at(piece_index);

    piece.source->play(app->res.al_buffer["piece_take"_H].get());
}

void Board::remember_position_and_check_repetition(size_t piece_index, size_t node_index) {
    ASSERT(piece_index != NULL_INDEX, "Invalid index");
    ASSERT(node_index != NULL_INDEX, "Invalid index");

    using Position = ThreefoldRepetitionHistory::PositionPlusInfo;
    const Position current_position = { get_position(), piece_index, node_index };

    for (const Position& position : repetition_history.twos) {
        if (position == current_position) {
            DEB_INFO("Threefold repetition");

            FORMATTED_MESSAGE(
                message, 64, "%s player has made threefold repetition.",
                TURN_IS_WHITE_SO("Black", "White")
            )

            game_over(BoardEnding {BoardEnding::TieBetweenBothPlayers, message});

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

void Board::piece_arrive_at_node(size_t piece_index) {
    ASSERT(piece_index != NULL_INDEX, "Invalid index");

    Piece& piece = pieces.at(piece_index);

    if (piece.movement.type == PieceMovementType::ThreeStep) {
        // Play sound even if piece is forced to finalize its state
        play_piece_place_sound(piece_index);
    }

    piece.model->position = piece.movement.target;

    // Reset all these movement data
    piece.movement = PieceMovement {};

    // Remove piece forever, if set to remove
    if (piece.pending_remove) {
        scene->scene_list.remove(piece.model);
        // app->renderer->remove_model(piece.model);
        pieces.erase(piece.index);
    }

    CAN_MAKE_MOVE();
}

void Board::prepare_piece_for_linear_move(size_t piece_index, const glm::vec3& target, const glm::vec3& velocity) {
    ASSERT(piece_index != NULL_INDEX, "Invalid index");

    Piece& piece = pieces.at(piece_index);

    if (!piece.pending_remove) {
        play_piece_move_sound(piece_index);
    }

    piece.movement.target = target;
    piece.movement.velocity = velocity;

    piece.movement.type = PieceMovementType::Linear;
    piece.movement.moving = true;
}

void Board::prepare_piece_for_three_step_move(size_t piece_index, const glm::vec3& target,
        const glm::vec3& velocity, const glm::vec3& target0, const glm::vec3& target1) {
    ASSERT(piece_index != NULL_INDEX, "Invalid index");

    Piece& piece = pieces.at(piece_index);

    piece.movement.target = target;
    piece.movement.velocity = velocity;
    piece.movement.target0 = target0;
    piece.movement.target1 = target1;

    piece.movement.type = PieceMovementType::ThreeStep;
    piece.movement.moving = true;
}
