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

class Board : public Hoverable {
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

    Board() : Hoverable(HOVERABLE_NULL) {}
    Board(unsigned int id);
    ~Board() = default;

    static float scale;

    Rc<VertexArray> vertex_array;
    static int index_count;
    Rc<Texture> diffuse_texture;

    static glm::vec3 specular_color;
    static float shininess;

    // std::array<Node, 24> nodes;

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
};

// TODO undo
