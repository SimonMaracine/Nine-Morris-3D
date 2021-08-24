#include <utility>
#include <cassert>

#include "ecs/undo.h"
#include "ecs/game.h"
#include "other/logging.h"

namespace undo {
    void remember_place(MovesHistory& history, entt::entity node, entt::entity piece) {
        SPDLOG_DEBUG("Remember place");

        PlacedPiece placed_piece;
        placed_piece.node = node;
        placed_piece.piece = piece;

        history.placed_pieces[history.moves] = placed_piece;

        history.moves++;
    }

    void remember_move(MovesHistory& history, entt::entity source_node,
                       entt::entity destination_node, entt::entity piece) {
        SPDLOG_DEBUG("Remember move");

        MovedPiece moved_piece;
        moved_piece.source_node = source_node;
        moved_piece.destination_node = destination_node;
        moved_piece.piece = piece;

        history.moved_pieces[history.moves] = moved_piece;

        history.moves++;
    }

    void remember_take(MovesHistory& history, entt::entity node, Piece piece_type) {
        SPDLOG_DEBUG("Remember take");

        assert(node != entt::null);

        TakenPiece taken_piece;
        taken_piece.node = node;
        taken_piece.piece_type = piece_type;

        history.taken_pieces[history.moves] = taken_piece;

        history.moves++;
    }

    MoveType get_undo_type(MovesHistory& history) {
        history.moves--;

        for (const std::pair<unsigned int, PlacedPiece>& pair : history.placed_pieces) {
            if (history.moves == pair.first) {
                return MoveType::Place;
            }
        }

        for (const std::pair<unsigned int, MovedPiece>& pair : history.moved_pieces) {
            if (history.moves == pair.first) {
                return MoveType::Move;
            }
        }

        for (const std::pair<unsigned int, TakenPiece>& pair : history.taken_pieces) {
            if (history.moves == pair.first) {
                return MoveType::Take;
            }
        }

        assert(false);
    }

    PlacedPiece undo_place(MovesHistory& history) {
        PlacedPiece placed_piece = history.placed_pieces.at(history.moves);
        history.placed_pieces.erase(history.moves);

        return placed_piece;
    }

    MovedPiece undo_move(MovesHistory& history) {
        MovedPiece moved_piece = history.moved_pieces.at(history.moves);
        history.moved_pieces.erase(history.moves);

        return moved_piece;
    }

    TakenPiece undo_take(MovesHistory& history) {
        TakenPiece taken_piece = history.taken_pieces.at(history.moves);
        history.taken_pieces.erase(history.moves);

        return taken_piece;
    }
}
