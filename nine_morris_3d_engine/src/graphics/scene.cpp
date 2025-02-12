#include "nine_morris_3d_engine/graphics/scene.hpp"

#include <limits>
#include <array>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace sm {
    void SceneNode3D::add_node(std::shared_ptr<SceneNode3D> node) {
        m_children.push_back(node);
    }

    void SceneNode3D::clear_nodes() {
        for (const auto& child : m_children) {
            child->clear_nodes();
        }

        m_children.clear();
    }

    void SceneNode3D::traverse(const std::function<bool(std::shared_ptr<SceneNode3D>)>& process) {
        if (process(shared_from_this())) {
            return;
        }

        for (const auto& child : m_children) {
            child->traverse(process);
        }
    }

    void SceneNode3D::traverse(const std::function<bool(SceneNode3D*)>& process) {
        if (process(this)) {
            return;
        }

        for (const auto& child : m_children) {
            child->traverse(process);
        }
    }

    void SceneNode3D::traverse(const std::function<bool(const SceneNode3D*, Context3D&)>& process) const {
        traverse<Context3D>(Context3D(), [&](const SceneNode3D* node, Context3D& context) {
            if (auto outlined_model_node {dynamic_cast<const OutlinedModelNode*>(node)}; outlined_model_node != nullptr) {
                context.transform = glm::translate(context.transform, outlined_model_node->position);
                context.transform = glm::rotate(context.transform, glm::radians(outlined_model_node->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
                context.transform = glm::rotate(context.transform, glm::radians(outlined_model_node->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
                context.transform = glm::rotate(context.transform, glm::radians(outlined_model_node->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
                context.transform = glm::scale(context.transform, glm::vec3(outlined_model_node->scale));

                context.transform_scale *= outlined_model_node->scale;

                switch (outlined_model_node->outline) {
                    case NodeFlag::Inherited:
                        break;
                    case NodeFlag::Enabled:
                        context.outline = true;
                        break;
                    case NodeFlag::Disabled:
                        context.outline = false;
                        break;
                }

                switch (outlined_model_node->disable_back_face_culling) {
                    case NodeFlag::Inherited:
                        break;
                    case NodeFlag::Enabled:
                        context.disable_back_face_culling = true;
                        break;
                    case NodeFlag::Disabled:
                        context.disable_back_face_culling = false;
                        break;
                }

                switch (outlined_model_node->cast_shadow) {
                    case NodeFlag::Inherited:
                        break;
                    case NodeFlag::Enabled:
                        context.cast_shadow = true;
                        break;
                    case NodeFlag::Disabled:
                        context.cast_shadow = false;
                        break;
                }
            } else if (auto model_node {dynamic_cast<const ModelNode*>(node)}; model_node != nullptr) {
                context.transform = glm::translate(context.transform, model_node->position);
                context.transform = glm::rotate(context.transform, glm::radians(model_node->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
                context.transform = glm::rotate(context.transform, glm::radians(model_node->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
                context.transform = glm::rotate(context.transform, glm::radians(model_node->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
                context.transform = glm::scale(context.transform, glm::vec3(model_node->scale));

                context.transform_scale *= model_node->scale;

                switch (model_node->disable_back_face_culling) {
                    case NodeFlag::Inherited:
                        break;
                    case NodeFlag::Enabled:
                        context.disable_back_face_culling = true;
                        break;
                    case NodeFlag::Disabled:
                        context.disable_back_face_culling = false;
                        break;
                }

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
            } else if (auto point_light_node {dynamic_cast<const PointLightNode*>(node)}; point_light_node != nullptr) {
                context.transform = glm::translate(context.transform, point_light_node->position);
            }

            return process(node, context);
        });
    }

    std::shared_ptr<SceneNode3D> SceneNode3D::find_node(Id node_id) {
        std::shared_ptr<SceneNode3D> result;

        traverse([&](std::shared_ptr<SceneNode3D> node) {
            if (node->id == node_id) {
                result = node;
                return true;
            }

            return false;
        });

        return result;
    }

    void SceneNode2D::add_node(std::shared_ptr<SceneNode2D> node) {
        m_children.push_back(node);
    }

    void SceneNode2D::clear_nodes() {
        m_children.clear();
    }

    void SceneNode2D::traverse(const std::function<bool(std::shared_ptr<SceneNode2D>)>& process) {
        if (process(shared_from_this())) {
            return;
        }

        for (const auto& child : m_children) {
            child->traverse(process);
        }
    }

    void SceneNode2D::traverse(const std::function<bool(SceneNode2D*)>& process) {
        if (process(this)) {
            return;
        }

        for (const auto& child : m_children) {
            child->traverse(process);
        }
    }

    void SceneNode2D::traverse(const std::function<bool(const SceneNode2D*, Context2D&)>& process) const {
        traverse<Context2D>(Context2D(), [&](const SceneNode2D* node, Context2D& context) {
            if (auto image_node {dynamic_cast<const ImageNode*>(node)}; image_node != nullptr) {
                context.position += image_node->position;
                context.scale *= image_node->scale;
            } else if (auto text_node {dynamic_cast<const TextNode*>(node)}; text_node != nullptr) {
                context.position += text_node->position;
                context.scale *= text_node->scale;
            }

            return process(node, context);
        });
    }

    std::shared_ptr<SceneNode2D> SceneNode2D::find_node(Id node_id) {
        std::shared_ptr<SceneNode2D> result;

        traverse([=](std::shared_ptr<SceneNode2D> node) mutable {
            if (node->id == node_id) {
                result = node;
                return true;
            }

            return false;
        });

        return result;
    }

    void RootNode3D::debug_add_line(glm::vec3 position1, glm::vec3 position2, glm::vec3 color) {
        DebugLine line;
        line.position1 = position1;
        line.position2 = position2;
        line.color = color;

#ifndef SM_BUILD_DISTRIBUTION
        m_debug_lines.push_back(line);
#endif
    }

    void RootNode3D::debug_add_lines(const std::vector<glm::vec3>& positions, glm::vec3 color) {
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

    void RootNode3D::debug_add_point(glm::vec3 position, glm::vec3 color) {
        static constexpr float SIZE {0.3f};

        debug_add_line(glm::vec3(-SIZE, 0.0f, 0.0f) + position, glm::vec3(SIZE, 0.0f, 0.0f) + position, color);
        debug_add_line(glm::vec3(0.0f, -SIZE, 0.0f) + position, glm::vec3(0.0f, SIZE, 0.0f) + position, color);
        debug_add_line(glm::vec3(0.0f, 0.0f, -SIZE) + position, glm::vec3(0.0f, 0.0f, SIZE) + position, color);
    }

    void RootNode3D::debug_add_lamp(glm::vec3 position, glm::vec3 color) {
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

    void RootNode3D::debug_clear() {
#ifndef SM_BUILD_DISTRIBUTION
        m_debug_lines.clear();
#endif
    }

    void RootNode3D::update_shadow_box() {
        const glm::mat4 view_matrix {
            glm::lookAt(glm::vec3(0.0f), directional_light.direction, glm::vec3(0.0f, 1.0f, 0.0f))
        };

        float max_x_positive {std::numeric_limits<float>::min()};
        float max_x_negative {std::numeric_limits<float>::max()};
        float max_y_positive {std::numeric_limits<float>::min()};
        float max_y_negative {std::numeric_limits<float>::max()};
        float max_z_positive {std::numeric_limits<float>::min()};
        float max_z_negative {std::numeric_limits<float>::max()};

        traverse([&](const SceneNode3D* node, Context3D& context) {
            auto model_node {dynamic_cast<const ModelNode*>(node)};

            if (model_node == nullptr) {
                return false;
            }

            if (!context.cast_shadow) {
                return false;
            }

            const glm::vec3 position_bb {view_matrix * context.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)};
            const float radius_bb {glm::length(glm::max(model_node->get_aabb().max, model_node->get_aabb().min)) * context.transform_scale};

            max_x_positive = glm::max(max_x_positive, position_bb.x + radius_bb);
            max_x_negative = glm::min(max_x_negative, position_bb.x - radius_bb);
            max_y_positive = glm::max(max_y_positive, position_bb.y + radius_bb);
            max_y_negative = glm::min(max_y_negative, position_bb.y - radius_bb);
            max_z_positive = glm::max(max_z_positive, position_bb.z + radius_bb);
            max_z_negative = glm::min(max_z_negative, position_bb.z - radius_bb);

            return false;
        });

        shadow_box.left = max_x_negative;
        shadow_box.right = max_x_positive;
        shadow_box.bottom = max_y_negative;
        shadow_box.top = max_y_positive;

        // After calculating some bound values, offset the position according to those values
        shadow_box.position = glm::normalize(directional_light.direction) * -(max_z_positive + 1.0f);
        shadow_box.near_ = 1.0f;
        shadow_box.far_ = -max_z_negative + max_z_positive + 1.0f;
    }

    void RootNode3D::update_camera() {
        m_camera_position = camera_controller ? camera_controller->get_position() : glm::vec3(0.0f);
    }
}
