#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/graphics/opengl/texture.hpp"
#include "nine_morris_3d_engine/graphics/renderable.hpp"
#include "nine_morris_3d_engine/graphics/light.hpp"
#include "nine_morris_3d_engine/graphics/camera.hpp"
#include "nine_morris_3d_engine/graphics/post_processing_step.hpp"

namespace sm {
    class Renderer;
    class DebugRenderer;
    class DebugUi;

    class Scene {
    public:
        // Immediate
        void capture(const Camera& camera, glm::vec3 position);
        void capture(const Camera2D& camera_2d);
        void skybox(std::shared_ptr<GlTextureCubemap> texture);
        void shadows(float left, float right, float bottom, float top, float near, float far, glm::vec3 position);
        void add_post_processing(std::shared_ptr<PostProcessingStep> step);

        // 3D API
        void add_renderable(const Renderable& renderable);
        void add_light(const DirectionalLight& light);
        void add_light(const PointLight& light);

        // 2D API
        void add_text(const Text& text);
        void add_quad(const Quad& quad);

        // Debug API
        void debug_add_line(glm::vec3 position1, glm::vec3 position2, glm::vec3 color);
        void debug_add_lines(const std::vector<glm::vec3>& points, glm::vec3 color);
        void debug_add_lines(std::initializer_list<glm::vec3> points, glm::vec3 color);
        void debug_add_point(glm::vec3 position, glm::vec3 color);
        void debug_add_lamp(glm::vec3 position, glm::vec3 color);

        void clear();
    private:
        std::vector<Renderable> renderables;
        DirectionalLight directional_light;
        std::vector<PointLight> point_lights;
        std::vector<Text> texts;
        std::vector<Quad> quads;
        std::vector<std::shared_ptr<PostProcessingStep>> post_processing_steps;
        std::shared_ptr<GlTextureCubemap> skybox_texture;

#ifndef SM_BUILD_DISTRIBUTION
        std::vector<DebugLine> debug_lines;
#endif

        struct {
            glm::mat4 view_matrix {1.0f};
            glm::mat4 projection_matrix {1.0f};
            glm::mat4 projection_view_matrix {1.0f};
            glm::vec3 position {};
        } camera;

        struct {
            glm::mat4 projection_matrix {1.0f};
        } camera_2d;

        struct {
            float left {};
            float right {};
            float bottom {};
            float top {};
            float near {1.0f};
            float far {1.0f};
            glm::vec3 position {};
        } light_space;

        friend class Renderer;
        friend class DebugRenderer;
        friend class DebugUi;
    };
};
