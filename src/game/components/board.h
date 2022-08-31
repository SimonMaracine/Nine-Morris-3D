#pragma once

#include <entt/entt.hpp>

#include "game/constants.h"

struct ThreefoldRepetitionHistory {
    struct PositionPlusInfo {
        bool operator==(const PositionPlusInfo& other) const {
            return (
                position == other.position &&
                piece_index == other.piece_index &&
                node_index == other.node_index
            );
        }

        std::array<PieceType, 24> position;
        size_t piece_index;
        size_t node_index;
    };

    std::vector<PositionPlusInfo> ones;
    std::vector<PositionPlusInfo> twos;
};

struct BoardComponent {
    // Renderer::Model model;
    // Renderer::Model paint_model;

    // std::array<Node, 24> nodes;
    // std::array<Piece, 18> pieces;

    // Handles to nodes and pieces
    std::array<entt::entity, 24> nodes;
    std::array<entt::entity, 18> pieces;

    BoardPhase phase = BoardPhase::PlacePieces;
    BoardPlayer turn = BoardPlayer::White;
    BoardEnding ending = BoardEnding::None;

    std::string ending_message;

    unsigned int white_pieces_count = 0;  // Number of pieces on the board
    unsigned int black_pieces_count = 0;
    unsigned int not_placed_pieces_count = 18;  // Number of pieces floating
    bool should_take_piece = false;

    // Node* hovered_node = nullptr;
    // Piece* hovered_piece = nullptr;
    // Piece* selected_piece = nullptr;
    entt::entity hovered_node = entt::null;
    entt::entity hovered_piece = entt::null;
    entt::entity selected_piece = entt::null;

    std::array<bool, 2> can_jump = { false, false };  // White first and black second

    unsigned int turns_without_mills = 0;

    ThreefoldRepetitionHistory repetition_history;

    // StateHistory* state_history = nullptr;
    // KeyboardControls* keyboard = nullptr;
    // GameContext* game_context = nullptr;

    entt::entity state_history = entt::null;
    entt::entity keyboard = entt::null;
    entt::entity game_context = entt::null;

    bool next_move = true;  // It is false when any piece is in air and true otherwise
    bool is_players_turn = true;
    bool switched_turn = false;
};
