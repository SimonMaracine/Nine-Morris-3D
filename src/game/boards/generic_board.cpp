#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/boards/generic_board.h"
#include "game/undo_redo_state.h"
#include "game/constants.h"

GamePosition GenericBoard::get_position() {
    GamePosition position;

    for (const Node& node : nodes) {
        if (node.piece != NULL_INDEX) {
            const Piece& piece = pieces[node.piece];

            position[node.index] = piece.type;
        } else {
            position[node.index] = PieceType::None;
        }
    }

    return position;
}

size_t GenericBoard::new_piece_to_place(PieceType type, float x_pos, float z_pos, size_t node_index) {
    for (auto& pair : pieces) {
        Piece& piece = pair.second;

        if (!piece.in_use && piece.type == type) {
            const glm::vec3 target = glm::vec3(x_pos, PIECE_Y_POSITION, z_pos);
            const glm::vec3 target0 = piece.model->position + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
            const glm::vec3 target1 = target + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
            const glm::vec3 velocity = glm::normalize(target0 - piece.model->position) * PIECE_BASE_VELOCITY;

            prepare_piece_for_three_step_move(piece.index, target, velocity, target0, target1);

            piece.in_use = true;
            piece.node = node_index;

            return piece.index;
        }
    }

    ASSERT(false, "Couldn't find a piece");
    return NULL_INDEX;
}

void GenericBoard::take_and_raise_piece(size_t piece_index) {
    Piece& piece = pieces[piece_index];

    prepare_piece_for_linear_move(
        piece_index,
        glm::vec3(piece.model->position.x, PIECE_Y_POSITION + PIECE_RAISE_HEIGHT, piece.model->position.z),
        glm::vec3(0.0f)
    );

    piece.node = NULL_INDEX;
    piece.pending_remove = true;
}

void GenericBoard::set_pieces_show_outline(PieceType type, bool show) {
    for (auto& pair : pieces) {
        Piece& piece = pair.second;

        if (piece.type == type) {
            piece.show_outline = show;
        }
    }
}

void GenericBoard::game_over(const BoardEnding& ending, PieceType type_to_hide) {
    phase = BoardPhase::GameOver;
    this->ending = ending;
    set_pieces_show_outline(type_to_hide, false);

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

bool GenericBoard::is_windmill_made(size_t node_index, PieceType type, size_t** windmills, size_t mills_count) {
    for (size_t i = 0; i < mills_count; i++) {
        const size_t* mill = windmills[i];

        const Node& node1 = nodes[mill[0]];
        const Node& node2 = nodes[mill[1]];
        const Node& node3 = nodes[mill[2]];

        if (node1.piece != NULL_INDEX && node2.piece != NULL_INDEX && node3.piece != NULL_INDEX) {
            const Piece& piece1 = pieces[node1.piece];
            const Piece& piece2 = pieces[node2.piece];
            const Piece& piece3 = pieces[node3.piece];

            if (piece1.type == type && piece2.type == type && piece3.type == type) {
                if (piece1.node == node_index || piece2.node == node_index || piece3.node == node_index) {
                    return true;
                }
            }
        }
    }

    return false;
}

void GenericBoard::set_pieces_to_take(PieceType type, bool take) {
    for (auto& pair : pieces) {
        Piece& piece = pair.second;

        if (piece.type == type) {
            piece.to_take = take;
        }
    }
}

size_t GenericBoard::number_of_pieces_in_windmills(PieceType type, size_t** windmills, size_t mills_count) {
    std::vector<size_t> pieces_inside_mills;

    for (size_t i = 0; i < mills_count; i++) {
        const size_t* mill = windmills[i];

        const Piece& piece1 = pieces[nodes[mill[0]].piece];
        const Piece& piece2 = pieces[nodes[mill[1]].piece];
        const Piece& piece3 = pieces[nodes[mill[2]].piece];

        if (piece1.index != NULL_INDEX && piece2.index != NULL_INDEX && piece3.index != NULL_INDEX) {
            if (piece1.type == type && piece2.type == type && piece3.type == type) {
                std::vector<size_t>::iterator iter;

                // TODO this seems wrong
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

void GenericBoard::unselect_other_pieces(size_t currently_selected_piece_index) {
    for (auto& pair : pieces) {
        Piece& piece = pair.second;

        if (piece.index != currently_selected_piece_index) {
            piece.selected = false;
        }
    }
}

void GenericBoard::update_piece_outlines() {
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

void GenericBoard::remember_position_and_check_repetition(size_t piece_index, Node* node) {
    using Position = ThreefoldRepetitionHistory::PositionPlusInfo;

    ASSERT(piece_index != NULL_INDEX, "Piece must not be null");
    ASSERT(piece_index != NULL_INDEX, "Node must not be null");

    const Piece& piece = pieces[piece_index];

    const Position current_position = { get_position(), piece.index, node->index };

    for (const Position& position : repetition_history.twos) {
        if (position == current_position) {
            DEB_INFO("Threefold repetition");

            FORMATTED_MESSAGE(
                message, 64, "%s player has made threefold repetition.",
                TURN_IS_WHITE_SO("Black", "White")
            )

            game_over(
                BoardEnding {BoardEnding::TieBetweenBothPlayers, message},
                TURN_IS_WHITE_SO(PieceType::White, PieceType::Black)
            );

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

void GenericBoard::remember_state(const Camera& camera) {
    const UndoRedoState::State state = {
        *this,
        camera,
        game_context->state
    };

    undo_redo_state->undo.push_back(state);
    undo_redo_state->redo.clear();

    DEB_DEBUG("Pushed new state");
}

void GenericBoard::piece_arrive_at_node(size_t piece_index) {
    Piece& piece = pieces[piece_index];

    piece.model->position = piece.movement.target;

    // Reset all these movement variables
    piece.movement = Piece::Movement {};

    // Remove piece, if set to remove
    if (piece.pending_remove) {
        app->renderer->remove_model(piece.model);
        pieces.erase(piece.index);
    }

    CAN_MAKE_MOVE();
}

void GenericBoard::prepare_piece_for_linear_move(size_t piece_index, const glm::vec3& target, const glm::vec3& velocity) {
    Piece& piece = pieces[piece_index];

    piece.movement.target = target;

    piece.movement.velocity = velocity;

    piece.movement.type = PieceMovementType::Linear;
    piece.movement.moving = true;
}

void GenericBoard::prepare_piece_for_three_step_move(size_t piece_index, const glm::vec3& target,
        const glm::vec3& velocity, const glm::vec3& target0, const glm::vec3& target1) {
    Piece& piece = pieces[piece_index];

    piece.movement.target = target;

    piece.movement.velocity = velocity;
    piece.movement.target0 = target0;
    piece.movement.target1 = target1;

    piece.movement.type = PieceMovementType::ThreeStep;
    piece.movement.moving = true;
}
