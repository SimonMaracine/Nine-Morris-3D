#pragma once

#include <engine/engine_graphics.h>

#include "game/entities/board.h"
#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "game/entities/serialization/jump_board_serialized.h"
#include "game/undo_redo_state.h"
#include "other/constants.h"

struct JumpBoard : public Board {  // Also good for jump plus variant
    JumpBoard() = default;
    virtual ~JumpBoard() = default;

    JumpBoard(const JumpBoard&) = delete;
    JumpBoard& operator=(const JumpBoard&) = delete;
    JumpBoard(JumpBoard&&) noexcept = default;
    JumpBoard& operator=(JumpBoard&&) noexcept = default;

    virtual void click(identifier::Id hovered_id) override;
    virtual Board::Flags release(identifier::Id hovered_id) override;
    virtual void place_piece(Index node_index) override;
    virtual void move_piece(Index source_node_index, Index destination_node_index) override;
    virtual void take_piece(Index node_index) override;

    void _move_piece(Index piece_index, Index node_index);

    void check_select_piece(identifier::Id hovered_id);
    void check_move_piece(identifier::Id hovered_id);

    void switch_turn_and_check_turns_without_mills();
    void remember_state();
    void to_serialized(JumpBoardSerialized& serialized);
    void from_serialized(const JumpBoardSerialized& serialized);

    unsigned int turns_without_mills = 0;

    UndoRedoState<JumpBoardSerialized>* undo_redo_state = nullptr;
};
