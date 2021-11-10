#pragma once

#include <array>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/texture.h"
#include "nine_morris_3d/node.h"
#include "nine_morris_3d/piece.h"
#include "nine_morris_3d/hoverable.h"

constexpr unsigned int MAX_TURNS_WITHOUT_MILLS = 40 + 1;

struct ThreefoldRepetitionHistory {
    std::vector<std::array<Piece::Type, 24>> ones;
    std::vector<std::array<Piece::Type, 24>> twos;
};

struct BoardPaint {
    glm::vec3 position;
    float scale;

    Rc<VertexArray> vertex_array;
    int index_count;
    Rc<Texture> diffuse_texture;

    glm::vec3 specular_color;
    float shininess;
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

    Board() : id(HOVERABLE_NULL) {}
    Board(hoverable::Id id);
    ~Board() = default;

    void place_piece(hoverable::Id hovered_id);
    void move_pieces(float dt);
    void take_piece(hoverable::Id hovered_id);
    void select_piece(hoverable::Id hovered_id);
    void put_piece(hoverable::Id hovered_id);
    void press(hoverable::Id hovered_id);
    void release(hoverable::Id hovered_id);
    void undo();

    hoverable::Id id;

    float scale;

    Rc<VertexArray> vertex_array;
    int index_count;
    Rc<Texture> diffuse_texture;

    glm::vec3 specular_color;
    float shininess;

    std::array<Node, 24> nodes;
    std::array<std::shared_ptr<Piece>, 18> pieces;

    Phase phase = Phase::PlacePieces;
    Player turn = Player::White;
    Ending ending = Ending::None;

    unsigned int white_pieces_count = 0;
    unsigned int black_pieces_count = 0;
    unsigned int not_placed_pieces_count = 0;
    bool should_take_piece = false;

    Node* hovered_node = nullptr;
    std::shared_ptr<Piece> hovered_piece = nullptr;
    std::shared_ptr<Piece> selected_piece = nullptr;

    std::array<bool, 2> can_jump = { false, false };

    unsigned int turns_without_mills = 0;
    ThreefoldRepetitionHistory repetition_history;

    BoardPaint paint;
private:
    std::shared_ptr<Piece> place_new_piece(Piece::Type type, float x_pos, float z_pos, Node* node);
    void take_raise_piece(Piece* piece);
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
};

// TODO undo
