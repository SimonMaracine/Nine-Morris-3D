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
    StandardBoard(StandardBoard&&) noexcept = default;
    StandardBoard& operator=(StandardBoard&&) noexcept = default;

    virtual void click(identifier::Id hovered_id) override;
    virtual Board::Flags release(identifier::Id hovered_id) override;
    virtual void place_piece(size_t node_index) override;
    virtual void move_piece(size_t source_node_index, size_t destination_node_index) override;
    virtual void take_piece(size_t node_index) override;

    void _place_piece(size_t node_index);
    void _move_piece(size_t piece_index, size_t node_index);
    void _take_piece(size_t piece_index);

    void check_select_piece(identifier::Id hovered_id);
    void check_place_piece(identifier::Id hovered_id);
    void check_move_piece(identifier::Id hovered_id);
    void check_take_piece(identifier::Id hovered_id);

    void check_phase_two();
    void switch_turn_and_check_turns_without_mills();
    bool can_go(size_t piece_index, size_t destination_node_index);
    void check_player_number_of_pieces(BoardPlayer player);
    bool can_player_jump(BoardPlayer player);
    bool can_player_jump(PieceType piece);
    bool is_player_blocked(BoardPlayer player);
    void set_game_over_player_blocked();
    void remember_state();
    void to_serialized(StandardBoardSerialized& serialized);
    void from_serialized(const StandardBoardSerialized& serialized);

    std::array<bool, 2> can_jump = { false, false };  // White first and black second

    unsigned int turns_without_mills = 0;  // Only used in pahse two

    UndoRedoState<StandardBoardSerialized>* undo_redo_state = nullptr;
};
