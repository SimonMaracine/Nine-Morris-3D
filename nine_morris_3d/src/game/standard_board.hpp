#pragma once

#include <array>
#include <vector>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/node.hpp"
#include "game/piece.hpp"

class StandardBoard {
public:
    StandardBoard() = default;
    StandardBoard(
        const sm::Renderable& board,
        const sm::Renderable& board_paint,
        const std::vector<sm::Renderable>& nodes,
        const std::vector<sm::Renderable>& white_pieces,
        const std::vector<sm::Renderable>& black_pieces
    );

    void update(sm::Ctx& ctx, glm::vec3 ray, glm::vec3 camera);
private:
    void update_hovered_index(glm::vec3 ray, glm::vec3 camera);

    int m_hovered_node_index {-1};
    int m_hovered_piece_index {-1};

    std::array<Node, 24> m_nodes {};
    std::array<Piece, 18> m_pieces {};

    sm::Renderable m_renderable;
    sm::Renderable m_paint_renderable;
};
