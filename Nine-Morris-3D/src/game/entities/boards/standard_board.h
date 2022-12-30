#pragma once

#include <engine/engine_graphics.h>

#include "game/entities/board.h"
#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "game/entities/serialization/standard_board_serialized.h"
#include "game/undo_redo_state.h"
#include "other/constants.h"

struct StandardBoard : public Board {
    StandardBoard() = default;
    virtual ~StandardBoard() = default;

    StandardBoard(const StandardBoard&) = delete;
    StandardBoard& operator=(const StandardBoard&) = delete;
    StandardBoard(StandardBoard&&) = default;
    StandardBoard& operator=(StandardBoard&&) = default;

    virtual void click(identifier::Id hovered_id) override;
    virtual Board::Flags release(identifier::Id hovered_id) override;
    virtual void computer_place_piece(size_t node_index) override;
    virtual void computer_move_piece(size_t source_node_index, size_t destination_node_index) override;
    virtual void computer_take_piece(size_t node_index) override;

    void check_select_piece(identifier::Id hovered_id);
    void check_place_piece(identifier::Id hovered_id);
    void check_move_piece(identifier::Id hovered_id);
    void check_take_piece(identifier::Id hovered_id);
    void place_piece(size_t node_index);
    void move_piece(size_t piece_index, size_t node_index);
    void take_piece(size_t piece_index);
    void switch_turn_and_check_turns_without_mills();
    bool can_go(size_t piece_index, size_t destination_node_index);
    void check_player_number_of_pieces(BoardPlayer player);
    bool is_player_blocked(BoardPlayer player);
    void remember_state();
    void to_serialized(StandardBoardSerialized& serialized);
    void from_serialized(const StandardBoardSerialized& serialized);

    unsigned int white_pieces_count = 0;  // Number of pieces on the board
    unsigned int black_pieces_count = 0;
    unsigned int not_placed_white_pieces_count = 9;  // Number of pieces floating
    unsigned int not_placed_black_pieces_count = 9;

    unsigned int turns_without_mills = 0;

    UndoRedoState<StandardBoardSerialized>* undo_redo_state = nullptr;
};
