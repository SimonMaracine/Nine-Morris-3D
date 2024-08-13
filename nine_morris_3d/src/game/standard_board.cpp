#include "game/standard_board.hpp"

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

// https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms

static bool ray_aabb_collision(glm::vec3 ray, glm::vec3 origin, const sm::utils::AABB& aabb) {
    // r.dir is unit direction vector of ray
    const float dirfrac_x {1.0f / ray.x};  // FIXME
    const float dirfrac_y {1.0f / ray.y};
    const float dirfrac_z {1.0f / ray.z};

    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    // r.org is origin of ray
    const float t1 {(aabb.min.x - origin.x) * dirfrac_x};
    const float t2 {(aabb.max.x - origin.x) * dirfrac_x};
    const float t3 {(aabb.min.y - origin.y) * dirfrac_y};
    const float t4 {(aabb.max.y - origin.y) * dirfrac_y};
    const float t5 {(aabb.min.z - origin.z) * dirfrac_z};
    const float t6 {(aabb.max.z - origin.z) * dirfrac_z};

    const float tmin {glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6))};
    const float tmax {glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6))};

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0) {
        return false;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax) {
        return false;
    }

    return true;
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

    // LOG_DEBUG("{}, {}, {}", ray.x, ray.y, ray.z);

    LOG_DEBUG("node: {}, piece: {}", m_hovered_node_index, m_hovered_piece_index);
}

void StandardBoard::update_hovered_index(glm::vec3 ray, glm::vec3 camera) {
    for (const auto& node : m_nodes) {
        if (ray_aabb_collision(ray, camera, node.get_aabb())) {
            m_hovered_node_index = node.get_index();
        }
    }

    for (const auto& piece : m_pieces) {
        if (ray_aabb_collision(ray, camera, piece.get_aabb())) {
            m_hovered_piece_index = piece.get_index();
        }
    }
}
