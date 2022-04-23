#pragma once

#include <array>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "graphics/renderer/main_renderer.h"
#include "graphics/renderer/hoverable.h"
#include "nine_morris_3d/node.h"
#include "nine_morris_3d/piece.h"

constexpr unsigned int MAX_TURNS_WITHOUT_MILLS = 40 + 1;

struct ThreefoldRepetitionHistory {
    std::vector<std::array<Piece::Type, 24>> ones;
    std::vector<std::array<Piece::Type, 24>> twos;
};

class Board {
public:
    enum class Phase {
        PlacePieces = 1,
        MovePieces = 2,
        GameOver,
        None
    };

    enum class Player {
        White = 0,
        Black = 1
    };

    enum class Ending {
        None,
        WinnerWhite,
        WinnerBlack,
        TieBetweenBothPlayers
    };

    Board() = default;
    Board(std::shared_ptr<std::vector<Board>> board_state_history);
    ~Board() = default;

    bool place_piece(hoverable::Id hovered_id);
    void move_pieces(float dt);
    bool take_piece(hoverable::Id hovered_id);
    void select_piece(hoverable::Id hovered_id);
    bool put_piece(hoverable::Id hovered_id);
    void press(hoverable::Id hovered_id);
    void release();
    void undo();
    unsigned int not_placed_pieces_count();
    void finalize_pieces_state();
    void update_cursor();
    void update_nodes(hoverable::Id hovered_id);
    void update_pieces(hoverable::Id hovered_id);

    Renderer::Model model;
    Renderer::Model paint_model;

    std::array<Node, 24> nodes;
    std::array<Piece, 18> pieces;

    Phase phase = Phase::PlacePieces;
    Player turn = Player::White;
    Ending ending = Ending::None;

    unsigned int white_pieces_count = 0;  // Number of pieces on the board
    unsigned int black_pieces_count = 0;
    unsigned int not_placed_white_pieces_count = 9;  // Number of pieces floating
    unsigned int not_placed_black_pieces_count = 9;
    bool should_take_piece = false;

    Node* hovered_node = nullptr;
    Piece* hovered_piece = nullptr;
    Piece* selected_piece = nullptr;

    std::array<bool, 2> can_jump = { false, false };  // White first and black second

    unsigned int turns_without_mills = 0;
    ThreefoldRepetitionHistory repetition_history;

    std::shared_ptr<std::vector<Board>> state_history;
    bool next_move = true;  // It is false when any piece is in air and true otherwise
private:
    Piece* place_new_piece(Piece::Type type, float x_pos, float z_pos, Node* node);
    void take_and_raise_piece(Piece* piece);
    void set_pieces_show_outline(Piece::Type type, bool show);
    void game_over(Ending ending, Piece::Type type_to_hide);
    void switch_turn();
    bool is_windmill_made(Node* node, Piece::Type type);
    void set_pieces_to_take(Piece::Type type, bool take);
    unsigned int number_of_pieces_in_windmills(Piece::Type type);
    void unselect_other_pieces(Piece* currently_selected_piece);
    void update_outlines();
    bool can_go(Node* source_node, Node* destination_node);
    void check_player_number_of_pieces(Player player);
    bool check_player_blocked(Player player);
    std::array<Piece::Type, 24> get_position();
    void remember_position_and_check_repetition();
    void remember_state();
    void arrive_at_node(Piece* piece);
    void prepare_piece_for_linear_move(Piece* piece, const glm::vec3& target, const glm::vec3& velocity);
    void prepare_piece_for_threestep_move(Piece* piece, const glm::vec3& target, const glm::vec3& velocity,
            const glm::vec3& target0, const glm::vec3& target1);
};
