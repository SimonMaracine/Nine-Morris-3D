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
    std::vector<std::array<std::shared_ptr<Piece>, 24>> ones;
    std::vector<std::array<std::shared_ptr<Piece>, 24>> twos;
};

struct BoardPaint {
    glm::vec3 position;
    float scale;

    Rc<VertexArray> vertex_array;
    static int index_count;
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

    void press(hoverable::Id hovered_id);
    void release(hoverable::Id hovered_id);
    void place_piece(hoverable::Id hovered_id);

    hoverable::Id id;

    static float scale;

    Rc<VertexArray> vertex_array;
    static int index_count;
    Rc<Texture> diffuse_texture;

    static glm::vec3 specular_color;
    static float shininess;

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
    Piece* hovered_piece = nullptr;
    Piece* selected_piece = nullptr;

    std::array<bool, 2> can_jump = { false, false };

    unsigned int turns_without_mills = 0;
    ThreefoldRepetitionHistory repetition_history;

    BoardPaint paint;
private:
    std::shared_ptr<Piece> place_new_piece(Piece::Type type, float x_pos, float z_pos, Node* node);
    void take_raise_piece(Piece* piece);
    void set_pieces_show_outline(Piece::Type type, bool show);
    void game_over(Board::Ending ending, Piece::Type type_to_hide);
    void switch_turn();
    bool is_windmill_made(Node* node, Piece::Type type);
    void set_pieces_to_take(Piece::Type type, bool take);
    unsigned int number_of_pieces_in_windmills(Piece::Type type);
};

// TODO undo
