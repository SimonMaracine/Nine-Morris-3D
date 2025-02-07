#include "nine_morris_3d_engine/graphics/scene.hpp"

#include <limits>
#include <array>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace sm {
    void SceneNode3D::add_node(std::shared_ptr<SceneNode3D> node) {
        m_children.push_back(node);
    }

    void SceneNode3D::traverse_transform_cast_shadow(const std::function<void(const SceneNode3D*, TransformCastShadow&)>& process) {
        traverse<TransformCastShadow>(TransformCastShadow(), [&](const SceneNode3D* node, TransformCastShadow& context) {
            if (auto model_node {dynamic_cast<ModelNode*>(this)}; model_node != nullptr) {
                context.position += model_node->transform.position;
                context.rotation += model_node->transform.rotation;
                context.scale *= model_node->transform.scale;

                switch (model_node->cast_shadow) {
                    case NodeFlag::Inherited:
                        break;
                    case NodeFlag::Enabled:
                        context.cast_shadow = true;
                        break;
                    case NodeFlag::Disabled:
                        context.cast_shadow = false;
                        break;
                }
            } else if (auto point_light_node {dynamic_cast<PointLightNode*>(this)}; point_light_node != nullptr) {
                context.position += point_light_node->position;
            }

            process(node, context);
        });
    }

    void SceneNode2D::add_node(std::shared_ptr<SceneNode2D> node) {
        m_children.push_back(node);
    }

    // void SceneNode2D::traverse_transform(const std::function<void(const SceneNode2D*, Transform&)>& process) {
    //     traverse<Transform>(Transform(), [&](const SceneNode2D* node, Transform& transform) {
    //         if (auto image_node {dynamic_cast<const ImageNode*>(this)}; image_node != nullptr) {
    //             transform.position += image_node->position;
    //             transform.scale *= image_node->scale;
    //         } else if (auto text_node {dynamic_cast<const TextNode*>(this)}; text_node != nullptr) {
    //             transform.position += text_node->position;
    //             transform.scale *= text_node->scale;
    //         }

    //         process(node, transform);
    //     });
    // }

    void Root3DNode::set_camera(const Camera& camera, glm::vec3 position) {
        m_camera = camera;
        m_camera_position = position;
    }

    void Root3DNode::set_skybox(const Skybox& skybox) {
        m_skybox = skybox;
    }

    void Root3DNode::set_directional_light(const DirectionalLight& directional_light) {
        m_directional_light = directional_light;
    }

    void Root3DNode::set_shadow_box(const ShadowBox& shadow_box) {
        m_shadow_box = shadow_box;
    }

    void Root3DNode::debug_add_line(glm::vec3 position1, glm::vec3 position2, glm::vec3 color) {
        DebugLine line;
        line.position1 = position1;
        line.position2 = position2;
        line.color = color;

#ifndef SM_BUILD_DISTRIBUTION
        m_debug_lines.push_back(line);
#endif
    }

    void Root3DNode::debug_add_lines(const std::vector<glm::vec3>& positions, glm::vec3 color) {
        assert(positions.size() >= 2);

        DebugLine line;
        line.color = color;

        for (std::size_t i {1}; i < positions.size(); i++) {
            line.position1 = positions.begin()[i - 1];
            line.position2 = positions.begin()[i];

#ifndef SM_BUILD_DISTRIBUTION
            m_debug_lines.push_back(line);
#endif
        }
    }

    void Root3DNode::debug_add_point(glm::vec3 position, glm::vec3 color) {
        static constexpr float SIZE {0.3f};

        debug_add_line(glm::vec3(-SIZE, 0.0f, 0.0f) + position, glm::vec3(SIZE, 0.0f, 0.0f) + position, color);
        debug_add_line(glm::vec3(0.0f, -SIZE, 0.0f) + position, glm::vec3(0.0f, SIZE, 0.0f) + position, color);
        debug_add_line(glm::vec3(0.0f, 0.0f, -SIZE) + position, glm::vec3(0.0f, 0.0f, SIZE) + position, color);
    }

    void Root3DNode::debug_add_lamp(glm::vec3 position, glm::vec3 color) {
        static constexpr float SIZE {0.3f};
        static constexpr float SIZE2 {0.15f};
        static constexpr float SIZE3 {0.5f};
        static constexpr float OFFSET {-(SIZE + SIZE3)};
        const std::array<DebugLine, 24> LINES {
            // Top
            DebugLine {glm::vec3(SIZE, -SIZE, SIZE), glm::vec3(SIZE, -SIZE, -SIZE), color},
            DebugLine {glm::vec3(SIZE, -SIZE, SIZE), glm::vec3(SIZE, SIZE, SIZE), color},
            DebugLine {glm::vec3(SIZE, -SIZE, SIZE), glm::vec3(-SIZE, -SIZE, SIZE), color},

            DebugLine {glm::vec3(-SIZE, -SIZE, -SIZE), glm::vec3(-SIZE, -SIZE, SIZE), color},
            DebugLine {glm::vec3(-SIZE, -SIZE, -SIZE), glm::vec3(-SIZE, SIZE, -SIZE), color},
            DebugLine {glm::vec3(-SIZE, -SIZE, -SIZE), glm::vec3(SIZE, -SIZE, -SIZE), color},

            DebugLine {glm::vec3(SIZE, -SIZE, -SIZE), glm::vec3(SIZE, SIZE, -SIZE), color},
            DebugLine {glm::vec3(-SIZE, -SIZE, SIZE), glm::vec3(-SIZE, SIZE, SIZE), color},

            DebugLine {glm::vec3(SIZE, SIZE, SIZE), glm::vec3(SIZE, SIZE, -SIZE), color},
            DebugLine {glm::vec3(SIZE, SIZE, SIZE), glm::vec3(-SIZE, SIZE, SIZE), color},
            DebugLine {glm::vec3(-SIZE, SIZE, -SIZE), glm::vec3(SIZE, SIZE, -SIZE), color},
            DebugLine {glm::vec3(-SIZE, SIZE, -SIZE), glm::vec3(-SIZE, SIZE, SIZE), color},

            // Bottom
            DebugLine {glm::vec3(SIZE2, -SIZE3 + OFFSET, SIZE2), glm::vec3(SIZE2, -SIZE3 + OFFSET, -SIZE2), color},
            DebugLine {glm::vec3(SIZE2, -SIZE3 + OFFSET, SIZE2), glm::vec3(SIZE2, SIZE3 + OFFSET, SIZE2), color},
            DebugLine {glm::vec3(SIZE2, -SIZE3 + OFFSET, SIZE2), glm::vec3(-SIZE2, -SIZE3 + OFFSET, SIZE2), color},

            DebugLine {glm::vec3(-SIZE2, -SIZE3 + OFFSET, -SIZE2), glm::vec3(-SIZE2, -SIZE3 + OFFSET, SIZE2), color},
            DebugLine {glm::vec3(-SIZE2, -SIZE3 + OFFSET, -SIZE2), glm::vec3(-SIZE2, SIZE3 + OFFSET, -SIZE2), color},
            DebugLine {glm::vec3(-SIZE2, -SIZE3 + OFFSET, -SIZE2), glm::vec3(SIZE2, -SIZE3 + OFFSET, -SIZE2), color},

            DebugLine {glm::vec3(SIZE2, -SIZE3 + OFFSET, -SIZE2), glm::vec3(SIZE2, SIZE3 + OFFSET, -SIZE2), color},
            DebugLine {glm::vec3(-SIZE2, -SIZE3 + OFFSET, SIZE2), glm::vec3(-SIZE2, SIZE3 + OFFSET, SIZE2), color},

            DebugLine {glm::vec3(SIZE2, SIZE3 + OFFSET, SIZE2), glm::vec3(SIZE2, SIZE3 + OFFSET, -SIZE2), color},
            DebugLine {glm::vec3(SIZE2, SIZE3 + OFFSET, SIZE2), glm::vec3(-SIZE2, SIZE3 + OFFSET, SIZE2), color},
            DebugLine {glm::vec3(-SIZE2, SIZE3 + OFFSET, -SIZE2), glm::vec3(SIZE2, SIZE3 + OFFSET, -SIZE2), color},
            DebugLine {glm::vec3(-SIZE2, SIZE3 + OFFSET, -SIZE2), glm::vec3(-SIZE2, SIZE3 + OFFSET, SIZE2), color},
        };

        for (const DebugLine& line : LINES) {
            debug_add_line(line.position1 + position, line.position2 + position, line.color);
        }
    }

    void Root3DNode::debug_clear() {
        m_debug_lines.clear();
    }

    void Root3DNode::update_shadow_box() {
        const glm::mat4 view_matrix {
            glm::lookAt(glm::vec3(0.0f), m_directional_light.direction, glm::vec3(0.0f, 1.0f, 0.0f))
        };

        float max_x_positive {std::numeric_limits<float>::min()};
        float max_x_negative {std::numeric_limits<float>::max()};
        float max_y_positive {std::numeric_limits<float>::min()};
        float max_y_negative {std::numeric_limits<float>::max()};
        float max_z_positive {std::numeric_limits<float>::min()};
        float max_z_negative {std::numeric_limits<float>::max()};

        traverse_transform_cast_shadow([&](const SceneNode3D* node, SceneNode3D::TransformCastShadow& context) {
            auto model_node {dynamic_cast<const ModelNode*>(node)};

            if (model_node == nullptr) {
                return;
            }

            if (!context.cast_shadow) {
                return;
            }

            const glm::vec3 position_bb {view_matrix * glm::vec4(context.position, 1.0f)};
            const float radius_bb {glm::length(glm::max(model_node->get_aabb().max, model_node->get_aabb().min)) * context.scale};

            max_x_positive = glm::max(max_x_positive, position_bb.x + radius_bb);
            max_x_negative = glm::min(max_x_negative, position_bb.x - radius_bb);
            max_y_positive = glm::max(max_y_positive, position_bb.y + radius_bb);
            max_y_negative = glm::min(max_y_negative, position_bb.y - radius_bb);
            max_z_positive = glm::max(max_z_positive, position_bb.z + radius_bb);
            max_z_negative = glm::min(max_z_negative, position_bb.z - radius_bb);
        });

        m_shadow_box.left = max_x_negative;
        m_shadow_box.right = max_x_positive;
        m_shadow_box.bottom = max_y_negative;
        m_shadow_box.top = max_y_positive;

        // After calculating some bound values, offset the position according to those values
        m_shadow_box.position = glm::normalize(m_directional_light.direction) * -(max_z_positive + 1.0f);
        m_shadow_box.near_ = 1.0f;
        m_shadow_box.far_ = -max_z_negative + max_z_positive + 1.0f;
    }

    void Root2DNode::set_camera(const Camera2D& camera) {
        m_camera = camera;
    }
}
