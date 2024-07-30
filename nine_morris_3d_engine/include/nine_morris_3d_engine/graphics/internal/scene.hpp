#pragma once

#include <vector>
#include <memory>
#include <initializer_list>

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/graphics/opengl/texture.hpp"
#include "nine_morris_3d_engine/graphics/renderable.hpp"
#include "nine_morris_3d_engine/graphics/light.hpp"
#include "nine_morris_3d_engine/graphics/camera.hpp"
#include "nine_morris_3d_engine/graphics/post_processing_step.hpp"

namespace sm::internal {
    class Renderer;
    class DebugRenderer;
    class DebugUi;

    class Scene {
    public:
        // Immediate
        void capture(const Camera& camera, glm::vec3 position) noexcept;
        void capture(const Camera2D& camera_2d) noexcept;
        void skybox(std::shared_ptr<GlTextureCubemap> texture) noexcept;
        void shadow(ShadowBox& box) noexcept;
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
        void debug_add_lines(const std::vector<glm::vec3>& positions, glm::vec3 color);
        void debug_add_lines(std::initializer_list<glm::vec3> positions, glm::vec3 color);
        void debug_add_point(glm::vec3 position, glm::vec3 color);
        void debug_add_lamp(glm::vec3 position, glm::vec3 color);

        void clear() noexcept;

        void shadow(const ShadowBox& box) noexcept;
        void add_renderable(const Renderable& renderable);
        void add_light(const DirectionalLight& light);
        void add_light(const PointLight& light);
        void add_text(const Text& text);
        void add_quad(const Quad& quad);
    private:
        std::vector<Renderable> m_renderables;
        DirectionalLight m_directional_light;
        std::vector<PointLight> m_point_lights;
        ShadowBox m_shadow_box;
        std::vector<Text> m_texts;
        std::vector<Quad> m_quads;
        std::vector<std::shared_ptr<PostProcessingStep>> m_post_processing_steps;
        std::shared_ptr<GlTextureCubemap> m_skybox_texture;
        Camera m_camera;
        glm::vec3 m_camera_position {};
        Camera2D m_camera_2d;

#ifndef SM_BUILD_DISTRIBUTION
        struct {
            std::vector<DebugLine> debug_lines;
            std::vector<Renderable*> renderables;
            DirectionalLight* directional_light {};
            std::vector<PointLight*> point_lights;
            ShadowBox* shadow_box {};
            std::vector<Text*> texts;
            std::vector<Quad*> quads;
        } m_debug;
#endif

        friend class Renderer;
        friend class DebugRenderer;
        friend class DebugUi;
    };
}
