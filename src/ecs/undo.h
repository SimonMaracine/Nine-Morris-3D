#pragma once

#include <map>

#include <entt/entt.hpp>

namespace undo {
    struct PlacedPiece {
        entt::entity node = entt::null;
        entt::entity piece = entt::null;
    };

    struct MovedPiece {    
        entt::entity source_node = entt::null;
        entt::entity destination_node = entt::null;
        entt::entity piece = entt::null;
    };

    struct TakenPiece {
        entt::entity node = entt::null;
        entt::entity piece = entt::null;
    };

    struct MovesHistory {
        unsigned int moves = 0;

        std::map<unsigned int, PlacedPiece> placed_pieces;
        std::map<unsigned int, MovedPiece> moved_pieces;
        std::map<unsigned int, TakenPiece> taken_pieces;
    };

    enum class MoveType {
        Place, Move, Take
    };

    void remember_place(MovesHistory& history, entt::entity node, entt::entity piece);
    void remember_move(MovesHistory& history, entt::entity source_node, entt::entity destination_node,
                       entt::entity piece);
    void remember_take(MovesHistory& history, entt::entity node, entt::entity piece);

    MoveType get_undo_type(MovesHistory& history);

    PlacedPiece undo_place(MovesHistory& history);
    MovedPiece undo_move(MovesHistory& history);
    TakenPiece undo_take(MovesHistory& history);
}
