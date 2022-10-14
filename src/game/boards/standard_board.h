#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/boards/generic_board.h"
#include "game/piece.h"
#include "game/node.h"
#include "other/constants.h"

struct StandardBoard : public GenericBoard {
    StandardBoard() = default;
    StandardBoard(Application* app)
        : GenericBoard(app) {}
    ~StandardBoard() = default;

    virtual void click(identifier::Id hovered_id) override;
    virtual std::tuple<bool, bool, bool> release(identifier::Id hovered_id) override;

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

    unsigned int white_pieces_count = 0;  // Number of pieces on the board
    unsigned int black_pieces_count = 0;
    unsigned int not_placed_pieces_count = 18;  // Number of pieces floating

    unsigned int turns_without_mills = 0;
};
