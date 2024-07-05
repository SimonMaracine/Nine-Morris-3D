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
    class Ctx;
    class Renderer;
    class DebugRenderer;
    class DebugUi;

    class Scene {
    public:
        // Immediate
        void capture(const Camera& camera, glm::vec3 position);
        void capture(const Camera2D& camera_2d);
        void skybox(std::shared_ptr<GlTextureCubemap> texture);
        void shadow(ShadowBox& box);
        void add_post_processing(std::shared_ptr<PostProcessingStep> step);

        // 3D API
        void add_renderable(Renderable& renderable);
        void add_light(DirectionalLight& light);
        void add_light(PointLight& light);

        // 2D API
        void add_text(Text& text);
        void add_quad(Quad& quad);

        // Debug API
        void debug_add_line(glm::vec3 position1, glm::vec3 position2, glm::vec3 color);
        void debug_add_lines(const std::vector<glm::vec3>& points, glm::vec3 color);
        void debug_add_lines(std::initializer_list<glm::vec3> points, glm::vec3 color);
        void debug_add_point(glm::vec3 position, glm::vec3 color);
        void debug_add_lamp(glm::vec3 position, glm::vec3 color);

        void clear();
    private:
        void shadow(const ShadowBox& box);
        void add_renderable(const Renderable& renderable);
        void add_light(const DirectionalLight& light);
        void add_light(const PointLight& light);
        void add_text(const Text& text);
        void add_quad(const Quad& quad);

        std::vector<Renderable> renderables;
        DirectionalLight directional_light;
        std::vector<PointLight> point_lights;
        ShadowBox shadow_box;
        std::vector<Text> texts;
        std::vector<Quad> quads;
        std::vector<std::shared_ptr<PostProcessingStep>> post_processing_steps;
        std::shared_ptr<GlTextureCubemap> skybox_texture;
        Camera camera;
        glm::vec3 camera_position {};
        Camera2D camera_2d;

#ifndef SM_BUILD_DISTRIBUTION
        struct {
            std::vector<DebugLine> debug_lines;
            std::vector<Renderable*> renderables;
            DirectionalLight* directional_light {};
            std::vector<PointLight*> point_lights;
            ShadowBox* shadow_box {};
            std::vector<Text*> texts;
            std::vector<Quad*> quads;
        } debug;
#endif

        friend class Ctx;
        friend class Renderer;
        friend class DebugRenderer;
        friend class DebugUi;
    };
};
