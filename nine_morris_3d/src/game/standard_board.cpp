#include "game/standard_board.hpp"

static float NODE_Y_POSITION = 0.063f;
static glm::vec3 NODE_POSITIONS[24] = {
    glm::vec3(2.046f, NODE_Y_POSITION, 2.062f),    // 0
    glm::vec3(-0.008f, NODE_Y_POSITION, 2.089f),   // 1
    glm::vec3(-2.101f, NODE_Y_POSITION, 2.076f),   // 2
    glm::vec3(1.480f, NODE_Y_POSITION, 1.512f),    // 3
    glm::vec3(0.001f, NODE_Y_POSITION, 1.513f),    // 4
    glm::vec3(-1.509f, NODE_Y_POSITION, 1.502f),   // 5
    glm::vec3(0.889f, NODE_Y_POSITION, 0.898f),    // 6
    glm::vec3(0.001f, NODE_Y_POSITION, 0.906f),    // 7
    glm::vec3(-0.930f, NODE_Y_POSITION, 0.892f),   // 8
    glm::vec3(2.058f, NODE_Y_POSITION, 0.031f),    // 9
    glm::vec3(1.481f, NODE_Y_POSITION, 0.025f),    // 10
    glm::vec3(0.894f, NODE_Y_POSITION, 0.026f),    // 11
    glm::vec3(-0.934f, NODE_Y_POSITION, 0.050f),   // 12
    glm::vec3(-1.508f, NODE_Y_POSITION, 0.050f),   // 13
    glm::vec3(-2.083f, NODE_Y_POSITION, 0.047f),   // 14
    glm::vec3(0.882f, NODE_Y_POSITION, -0.894f),   // 15
    glm::vec3(0.011f, NODE_Y_POSITION, -0.900f),   // 16
    glm::vec3(-0.930f, NODE_Y_POSITION, -0.890f),  // 17
    glm::vec3(1.477f, NODE_Y_POSITION, -1.455f),   // 18
    glm::vec3(0.006f, NODE_Y_POSITION, -1.463f),   // 19
    glm::vec3(-1.493f, NODE_Y_POSITION, -1.458f),  // 20
    glm::vec3(2.063f, NODE_Y_POSITION, -2.046f),   // 21
    glm::vec3(0.001f, NODE_Y_POSITION, -2.061f),   // 22
    glm::vec3(-2.081f, NODE_Y_POSITION, -2.045f)   // 23
};

StandardBoard::StandardBoard(
    const sm::Renderable& board,
    const sm::Renderable& board_paint,
    const std::vector<sm::Renderable>& nodes,
    const std::vector<sm::Renderable>& white_pieces,
    const std::vector<sm::Renderable>& black_pieces
) {
    m_renderable = board;
    m_renderable.transform.scale = 20.0f;

    m_paint_renderable = board_paint;
    m_paint_renderable.transform.scale = 20.0f;
    m_paint_renderable.transform.position.y = 0.062f;

    for (unsigned int i {0}; i < 24; i++) {
        m_nodes[i] = Node(i, NODE_POSITIONS[i], nodes[i]);
    }

    for (unsigned int i {0}; i < 9; i++) {
        m_pieces[i] = Piece(i, glm::vec3(3.0f, 0.5f, static_cast<float>(i) * 0.5f - 2.0f), white_pieces[i]);
    }

    for (unsigned int i {9}; i < 18; i++) {
        m_pieces[i] = Piece(i, glm::vec3(-3.0f, 0.5f, static_cast<float>(i - 9) * 0.5f - 2.0f), black_pieces[i - 9]);
    }
}

void StandardBoard::update(sm::Ctx& ctx) {
    ctx.add_renderable(m_renderable);
    ctx.add_renderable(m_paint_renderable);

    for (auto& node : m_nodes) {
        node.update(ctx);
    }

    for (auto& piece : m_pieces) {
        piece.update(ctx);
    }
}
