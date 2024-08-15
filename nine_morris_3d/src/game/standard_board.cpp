#include "game/standard_board.hpp"

#include <nine_morris_3d_engine/external/glm.h++>

#include "game/ray.hpp"

static constexpr float NODE_Y_POSITION = 0.063f;
static const glm::vec3 NODE_POSITIONS[24] = {
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

static glm::mat4 transformation_matrix(glm::vec3 position, glm::vec3 rotation, float scale) {
    glm::mat4 matrix {1.0f};
    matrix = glm::translate(matrix, position);
    matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::scale(matrix, glm::vec3(scale));

    return matrix;
}

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

    for (int i {0}; i < 24; i++) {
        m_nodes[i] = Node(i, NODE_POSITIONS[i], nodes[i]);
    }

    for (int i {0}; i < 9; i++) {
        m_pieces[i] = Piece(i, glm::vec3(3.0f, 0.5f, static_cast<float>(i) * 0.5f - 2.0f), white_pieces[i]);
    }

    for (int i {9}; i < 18; i++) {
        m_pieces[i] = Piece(i, glm::vec3(-3.0f, 0.5f, static_cast<float>(i - 9) * 0.5f - 2.0f), black_pieces[i - 9]);
    }
}

void StandardBoard::update(sm::Ctx& ctx, glm::vec3 ray, glm::vec3 camera) {
    ctx.add_renderable(m_renderable);
    ctx.add_renderable(m_paint_renderable);

    for (auto& node : m_nodes) {
        node.update(ctx);
    }

    for (auto& piece : m_pieces) {
        piece.update(ctx);
    }

    update_hovered_index(ray, camera);

    LOG_DEBUG("node: {}, piece: {}", m_hovered_node_index, m_hovered_piece_index);
}

void StandardBoard::update_hovered_index(glm::vec3 ray, glm::vec3 camera) {
    if (camera.y > 0.0f) {
        bool hover {false};

        for (const auto& node : m_nodes) {
            const auto& transform {node.get_renderable().transform};
            const glm::mat4 to_world_space {transformation_matrix(transform.position, transform.rotation, transform.scale)};

            sm::utils::AABB aabb;
            aabb.min = to_world_space * glm::vec4(node.get_renderable().get_aabb().min, 1.0f);
            aabb.max = to_world_space * glm::vec4(node.get_renderable().get_aabb().max, 1.0f);

            if (ray_aabb_collision(ray, camera, aabb)) {
                m_hovered_node_index = node.get_index();
                hover = true;
            }
        }

        if (!hover) {
            m_hovered_node_index = -1;
        }
    } else {
        m_hovered_node_index = -1;
    }

    {
        bool hover {false};

        for (const auto& piece : m_pieces) {
            const auto& transform {piece.get_renderable().transform};
            const glm::mat4 to_world_space {transformation_matrix(transform.position, transform.rotation, transform.scale)};

            sm::utils::AABB aabb;
            aabb.min = to_world_space * glm::vec4(piece.get_renderable().get_aabb().min, 1.0f);
            aabb.max = to_world_space * glm::vec4(piece.get_renderable().get_aabb().max, 1.0f);

            if (ray_aabb_collision(ray, camera, aabb)) {
                m_hovered_piece_index = piece.get_index();
                hover = true;
            }
        }

        if (!hover) {
            m_hovered_piece_index = -1;
        }
    }
}
