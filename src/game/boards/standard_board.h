#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/boards/generic_board.h"
#include "game/piece.h"
#include "game/node.h"
#include "game/constants.h"

class StandardBoard : public GenericBoard {
public:
    StandardBoard() = default;
    StandardBoard(Application* app)
        : GenericBoard(app) {}
    ~StandardBoard() = default;

    virtual bool player_must_take_piece() override;

    unsigned int get_white_pieces_count() { return white_pieces_count; }
    unsigned int get_black_pieces_count() { return black_pieces_count; }
    unsigned int get_not_placed_pieces_count() { return not_placed_pieces_count; }
    unsigned int get_turns_without_mills() { return turns_without_mills; }
private:
    void switch_turn_and_check_turns_without_mills();
    bool can_go(Node* source_node, Node* destination_node);
    void check_player_number_of_pieces(BoardPlayer player);
    bool is_player_blocked(BoardPlayer player);

    unsigned int white_pieces_count = 0;  // Number of pieces on the board
    unsigned int black_pieces_count = 0;
    unsigned int not_placed_pieces_count = 18;  // Number of pieces floating

    bool must_take_piece = false;

    unsigned int turns_without_mills = 0;
};
