#include "nine_morris_3d_engine/graphics/scene.hpp"

#include <array>
#include <cassert>

namespace sm {
    void Scene::capture(const Camera& camera, glm::vec3 position) {
        this->camera.view_matrix = camera.view_matrix;
        this->camera.projection_matrix = camera.projection_matrix;
        this->camera.projection_view_matrix = camera.projection_view_matrix;
        this->camera.position = position;
    }

    void Scene::capture(const Camera2D& camera_2d) {
        this->camera_2d.projection_matrix = camera_2d.projection_matrix;
    }

    void Scene::skybox(std::shared_ptr<GlTextureCubemap> texture) {
        skybox_texture = texture;
    }

    void Scene::shadows(float left, float right, float bottom, float top, float near, float far, glm::vec3 position) {
        light_space.left = left;
        light_space.right = right;
        light_space.bottom = bottom;
        light_space.top = top;
        light_space.near = near;
        light_space.far = far;
        light_space.position = position;
    }

    void Scene::add_post_processing(std::shared_ptr<PostProcessingStep> step) {
        post_processing_steps.push_back(step);
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

    void Scene::debug_add_line(glm::vec3 position1, glm::vec3 position2, glm::vec3 color) {
        DebugLine line;
        line.position1 = position1;
        line.position2 = position2;
        line.color = color;

#ifndef SM_BUILD_DISTRIBUTION
        debug_lines.push_back(line);
#endif
    }

    void Scene::debug_add_lines(const std::vector<glm::vec3>& points, glm::vec3 color) {
        assert(points.size() >= 2);

        DebugLine line;
        line.color = color;

        for (std::size_t i {1}; i < points.size(); i++) {
            line.position1 = points.begin()[i - 1];
            line.position2 = points.begin()[i];

#ifndef SM_BUILD_DISTRIBUTION
            debug_lines.push_back(line);
#endif
        }
    }

    void Scene::debug_add_lines(std::initializer_list<glm::vec3> points, glm::vec3 color) {
        debug_add_lines(std::vector(points), color);
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
        texts.clear();
        quads.clear();
        skybox_texture = nullptr;
        post_processing_steps.clear();
        camera = {};
        camera_2d = {};
        light_space = {};

#ifndef SM_BUILD_DISTRIBUTION
        debug_lines.clear();
#endif
    }
}
