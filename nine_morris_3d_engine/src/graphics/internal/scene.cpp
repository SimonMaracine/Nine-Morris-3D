#include "nine_morris_3d_engine/graphics/internal/scene.hpp"

#include <numeric>
#include <array>
#include <cassert>

#include <glm/gtc/matrix_transform.hpp>

namespace sm::internal {
    void Scene::capture(const Camera& camera, glm::vec3 position) {
        this->camera = camera;
        camera_position = position;
    }

    void Scene::capture(const Camera2D& camera_2d) {
        this->camera_2d = camera_2d;
    }

    void Scene::skybox(std::shared_ptr<GlTextureCubemap> texture) {
        skybox_texture = texture;
    }

    void Scene::shadow(ShadowBox& box) {
        const glm::mat4 view_matrix {
            glm::lookAt(glm::vec3(0.0f), directional_light.direction, glm::vec3(0.0f, 1.0f, 0.0f))
        };

        float max_x_positive {std::numeric_limits<float>::min()};
        float max_x_negative {std::numeric_limits<float>::max()};
        float max_y_positive {std::numeric_limits<float>::min()};
        float max_y_negative {std::numeric_limits<float>::max()};
        float max_z_positive {std::numeric_limits<float>::min()};
        float max_z_negative {std::numeric_limits<float>::max()};

        for (const Renderable& renderable : renderables) {
            if (!(renderable.get_material()->flags & Material::CastShadow)) {
                continue;
            }

            const glm::vec3 position {view_matrix * glm::vec4(renderable.transform.position, 1.0f)};
            const float radius {
                glm::length(glm::max(renderable.get_aabb().max, renderable.get_aabb().min)) * renderable.transform.scale
            };

            max_x_positive = glm::max(max_x_positive, position.x + radius);
            max_x_negative = glm::min(max_x_negative, position.x - radius);
            max_y_positive = glm::max(max_y_positive, position.y + radius);
            max_y_negative = glm::min(max_y_negative, position.y - radius);
            max_z_positive = glm::max(max_z_positive, position.z + radius);
            max_z_negative = glm::min(max_z_negative, position.z - radius);
        }

        box.left = max_x_negative;
        box.right = max_x_positive;
        box.bottom = max_y_negative;
        box.top = max_y_positive;

        // After calculating some bound values, offset the position according to those values
        box.position = glm::normalize(directional_light.direction) * -max_z_positive;
        box.near = 1.0f;
        box.far = -max_z_negative + max_z_positive;

        shadow(const_cast<const ShadowBox&>(box));

#ifndef SM_BUILD_DISTRIBUTION
        debug.shadow_box = &box;
#endif
    }

    void Scene::add_post_processing(std::shared_ptr<PostProcessingStep> step) {
        post_processing_steps.push_back(step);
    }

    void Scene::add_renderable(Renderable& renderable) {
        add_renderable(const_cast<const Renderable&>(renderable));

#ifndef SM_BUILD_DISTRIBUTION
        debug.renderables.push_back(&renderable);
#endif
    }

    void Scene::add_light(DirectionalLight& light) {
        add_light(const_cast<const DirectionalLight&>(light));

#ifndef SM_BUILD_DISTRIBUTION
        debug.directional_light = &light;
#endif
    }

    void Scene::add_light(PointLight& light) {
        add_light(const_cast<const PointLight&>(light));

#ifndef SM_BUILD_DISTRIBUTION
        debug.point_lights.push_back(&light);
#endif
    }

    void Scene::add_text(Text& text) {
        add_text(const_cast<const Text&>(text));

#ifndef SM_BUILD_DISTRIBUTION
        debug.texts.push_back(&text);
#endif
    }

    void Scene::add_quad(Quad& quad) {
        add_quad(const_cast<const Quad&>(quad));

#ifndef SM_BUILD_DISTRIBUTION
        debug.quads.push_back(&quad);
#endif
    }

    void Scene::debug_add_line(glm::vec3 position1, glm::vec3 position2, glm::vec3 color) {
        DebugLine line;
        line.position1 = position1;
        line.position2 = position2;
        line.color = color;

#ifndef SM_BUILD_DISTRIBUTION
        debug.debug_lines.push_back(line);
#endif
    }

    void Scene::debug_add_lines(const std::vector<glm::vec3>& positions, glm::vec3 color) {
        assert(positions.size() >= 2);

        DebugLine line;
        line.color = color;

        for (std::size_t i {1}; i < positions.size(); i++) {
            line.position1 = positions.begin()[i - 1];
            line.position2 = positions.begin()[i];

#ifndef SM_BUILD_DISTRIBUTION
            debug.debug_lines.push_back(line);
#endif
        }
    }

    void Scene::debug_add_lines(std::initializer_list<glm::vec3> positions, glm::vec3 color) {
        debug_add_lines(std::vector(positions), color);
    }

    void Scene::debug_add_point(glm::vec3 position, glm::vec3 color) {
        static constexpr float SIZE {0.5f};

        debug_add_line(glm::vec3(-SIZE, 0.0f, 0.0f) + position, glm::vec3(SIZE, 0.0f, 0.0f) + position, color);
        debug_add_line(glm::vec3(0.0f, -SIZE, 0.0f) + position, glm::vec3(0.0f, SIZE, 0.0f) + position, color);
        debug_add_line(glm::vec3(0.0f, 0.0f, -SIZE) + position, glm::vec3(0.0f, 0.0f, SIZE) + position, color);
    }

    void Scene::debug_add_lamp(glm::vec3 position, glm::vec3 color) {
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

    void Scene::clear() {
        renderables.clear();
        directional_light = {};
        point_lights.clear();
        shadow_box = {};
        texts.clear();
        quads.clear();
        skybox_texture = {};
        post_processing_steps.clear();
        camera = {};
        camera_2d = {};

#ifndef SM_BUILD_DISTRIBUTION
        debug.debug_lines.clear();
        debug.renderables.clear();
        debug.directional_light = {};
        debug.point_lights.clear();
        debug.shadow_box = {};
        debug.texts.clear();
        debug.quads.clear();
#endif
    }

    void Scene::shadow(const ShadowBox& box) {
        shadow_box = box;
    }

    void Scene::add_renderable(const Renderable& renderable) {
        renderables.push_back(renderable);
    }

    void Scene::add_light(const DirectionalLight& light) {
        directional_light = light;
    }

    void Scene::add_light(const PointLight& light) {
        point_lights.push_back(light);
    }

    void Scene::add_text(const Text& text) {
        texts.push_back(text);
    }

    void Scene::add_quad(const Quad& quad) {
        quads.push_back(quad);
    }
}
