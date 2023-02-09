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
    check_select_piece(hovered_id);
    check_move_piece(hovered_id);

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

void JumpBoard::computer_place_piece(Index) {
    ASSERT(false, "Unimplemented");
}

void JumpBoard::computer_move_piece(Index source_node_index, Index destination_node_index) {
    remember_state();
    move_piece(nodes.at(source_node_index).piece_index, destination_node_index);
}

void JumpBoard::computer_take_piece(Index) {
    ASSERT(false, "Unimplemented");
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

void JumpBoard::check_move_piece(identifier::Id hovered_id) {
    if (selected_piece_index == NULL_INDEX) {
        return;
    }

    for (const Node& node : nodes) {
        const bool can_move = (
            node.index == clicked_node_index && node.model->bounding_box->id == hovered_id
            && node.piece_index == NULL_INDEX
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

void JumpBoard::move_piece(Index piece_index, Index node_index) {
    ASSERT(piece_index != NULL_INDEX, "Invalid index");
    ASSERT(node_index != NULL_INDEX, "Invalid index");

    Piece& piece = pieces.at(piece_index);
    Node& node = nodes.at(node_index);

    ASSERT(node.piece_index == NULL_INDEX, "Piece must be null");

    WAIT_FOR_NEXT_MOVE();

    const auto target = glm::vec3(node.model->position.x, PIECE_Y_POSITION, node.model->position.z);
    const auto target0 = piece.model->position + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
    const auto target1 = target + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
    const auto velocity = glm::normalize(target0 - piece.model->position) * PIECE_BASE_VELOCITY;

    prepare_piece_for_three_step_move(piece_index, target, velocity, target0, target1);

    // Reset all of these
    Node& previous_node = nodes.at(piece.node_index);
    previous_node.piece_index = NULL_INDEX;

    piece.node_index = node_index;
    piece.selected = false;
    node.piece_index = piece_index;

    static constexpr auto mills = MILLS_NINE_MENS_MORRIS;
    static constexpr auto count = NINE_MENS_MORRIS_MILLS;

    if (is_mill_made(node_index, TURN_IS_WHITE_SO(PieceType::White, PieceType::Black), mills, count)) {
        DEB_DEBUG("{} mill is made", TURN_IS_WHITE_SO("White", "Black"));

        FORMATTED_MESSAGE(
            message, 64, "%s player has made a mill.",
            TURN_IS_WHITE_SO("White", "Black")
        )

        game_over(
            BoardEnding {TURN_IS_WHITE_SO(BoardEnding::WinnerWhite, BoardEnding::WinnerBlack), message}
        );

        if (turn == BoardPlayer::White) {
            set_pieces_show_outline(PieceType::White, false);
        } else {
            set_pieces_show_outline(PieceType::Black, false);
        }

        turns_without_mills = 0;
    } else {
        turns_without_mills++;
        switch_turn_and_check_turns_without_mills();
        update_piece_outlines();

        remember_position_and_check_repetition(piece_index, node_index);
    }
}

void JumpBoard::switch_turn_and_check_turns_without_mills() {
    if (phase == BoardPhase::MovePieces) {
        if (turns_without_mills == MAX_TURNS_WITHOUT_MILLS) {
            DEB_INFO("The max amount of turns without mills has been hit");

            FORMATTED_MESSAGE(
                message, 64, "%u turns have passed without a mill.",
                MAX_TURNS_WITHOUT_MILLS
            )

            game_over(BoardEnding {BoardEnding::TieBetweenBothPlayers, message});
        }
    }

    turn = TURN_IS_WHITE_SO(BoardPlayer::Black, BoardPlayer::White);
    flags.switched_turn = true;
    turn_count++;
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
    for (size_t i = 0; i < MAX_NODES; i++) {
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
    serialized.turn_count = turn_count;
    serialized.repetition_history = repetition_history;
    serialized.is_players_turn = is_players_turn;
    serialized.turns_without_mills = turns_without_mills;
}

void JumpBoard::from_serialized(const JumpBoardSerialized& serialized) {
    auto& data = app->user_data<Data>();

    for (size_t i = 0; i < MAX_NODES; i++) {
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
                scene->objects.get<renderables::Model>(hs("piece" + std::to_string(ser_index))),
                // scene->scene.model[hs("piece" + std::to_string(ser_index))].get(),
                app->res.al_source[hs("piece" + std::to_string(ser_index))]
            };

            piece.model->position = ser_piece.position;
            piece.model->rotation = ser_piece.rotation;
            const char* piece_tag = piece.type == PieceType::White ? "white_piece" : "black_piece";
            piece.model->vertex_array = app->res.vertex_array[hs(piece_tag + std::to_string(ser_index))];
            piece.model->index_buffer = (
                ser_piece.type == PieceType::White
                    ?
                    app->res.index_buffer["white_piece"_H]
                    :
                    app->res.index_buffer["black_piece"_H]
            );
            piece.model->scale = WORLD_SCALE;
            piece.model->material = app->res.material_instance[hs("piece" + std::to_string(ser_index))];
            piece.model->outline_color = std::make_optional<glm::vec3>(1.0f);
            piece.model->bounding_box = std::make_optional<renderables::Model::BoundingBox>();
            piece.model->bounding_box->id = data.piece_ids[ser_index];
            piece.model->bounding_box->size = PIECE_BOUNDING_BOX;
            piece.model->cast_shadow = true;

            piece.in_use = ser_piece.in_use;
            piece.node_index = ser_piece.node_index;
            piece.show_outline = ser_piece.show_outline;
            piece.to_take = ser_piece.to_take;
            piece.pending_remove = ser_piece.pending_remove;

            scene->scene_list.add(piece.model);
            // app->renderer->add_model(piece.model);
            pieces[ser_index] = piece;
        }
    }

    std::vector<Index> to_remove;

    for (auto& [index, piece] : pieces) {
        if (serialized.pieces.find(index) == serialized.pieces.end()) {
            scene->scene_list.remove(piece.model);
            // app->renderer->remove_model(piece.model);
            to_remove.push_back(index);
        }
    }

    for (Index index : to_remove) {
        pieces.erase(index);
    }

    phase = serialized.phase;
    turn = serialized.turn;
    ending = serialized.ending;
    repetition_history = serialized.repetition_history;
    must_take_piece = serialized.must_take_piece;
    is_players_turn = serialized.is_players_turn;
    turn_count = serialized.turn_count;
    turns_without_mills = serialized.turns_without_mills;

    // Reset these values
    clicked_node_index = NULL_INDEX;
    clicked_piece_index = NULL_INDEX;
    selected_piece_index = NULL_INDEX;
}
