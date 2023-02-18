#include "game/minimax/common.h"

Move Move::create_place(PieceType piece, size_t place_node_index) {
    Move move;
    move.type = MoveType::Place;
    move.piece = piece;
    move.place_node_index = place_node_index;
    return move;
}

Move Move::create_move(PieceType piece, size_t move_source_node_index, size_t move_destination_node_index) {
    Move move;
    move.type = MoveType::Move;
    move.piece = piece;
    move.move_source_node_index = move_source_node_index;
    move.move_destination_node_index = move_destination_node_index;
    return move;
}

Move Move::create_place_take(PieceType piece, size_t place_node_index, size_t take_node_index) {
    Move move;
    move.type = MoveType::PlaceTake;
    move.piece = piece;
    move.place_node_index = place_node_index;
    move.take_node_index = take_node_index;
    return move;
}

Move Move::create_move_take(PieceType piece, size_t move_source_node_index, size_t move_destination_node_index, size_t take_node_index) {
    Move move;
    move.type = MoveType::MoveTake;
    move.piece = piece;
    move.move_source_node_index = move_source_node_index;
    move.move_destination_node_index = move_destination_node_index;
    move.take_node_index = take_node_index;
    return move;
}
