#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "engine/graphics/opengl/vertex_array.hpp"
#include "engine/graphics/opengl/buffer.hpp"
#include "engine/graphics/opengl/framebuffer.hpp"
#include "engine/graphics/opengl/shader.hpp"
#include "engine/graphics/screen.hpp"
#include "engine/graphics/post_processing.hpp"
#include "engine/graphics/camera.hpp"
#include "engine/graphics/renderable.hpp"
#include "engine/graphics/light.hpp"

namespace sm {
    class Application;

    class Renderer {
    public:
        Renderer(Screen& screen, int width, int height);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        void add_renderable(const Renderable& renderable);
        void add_light(const DirectionalLight& light);

        void capture(const Camera& camera, const glm::vec3& position);

        void add_shader(std::shared_ptr<GlShader> shader);
    private:
        void render(int width, int height);
        void prerender_setup();
        void postrender_setup();
        const glm::mat4& get_projection_view_matrix();

        void draw_screen_quad(unsigned int texture);
        void post_processing();
        void end_rendering();

        void draw_renderables();
        void draw_renderable(const Renderable& renderable);

        void draw_renderables_outlined();
        void draw_renderable_outlined(const Renderable& renderable);

        struct {
            std::shared_ptr<GlFramebuffer> scene_framebuffer;

            std::unique_ptr<GlShader> screen_quad_shader;
            std::unique_ptr<GlVertexArray> screen_quad_vertex_array;

            std::vector<std::weak_ptr<GlUniformBuffer>> uniform_buffers;
            std::weak_ptr<GlUniformBuffer> projection_view_uniform_buffer;
            std::weak_ptr<GlUniformBuffer> light_uniform_buffer;
            std::weak_ptr<GlUniformBuffer> view_position_uniform_buffer;
        } storage;

        PostProcessingContext post_processing_context;  // TODO implement

        struct {
            glm::mat4 view_matrix = glm::mat4(1.0f);
            glm::mat4 projection_matrix = glm::mat4(1.0f);
            glm::mat4 projection_view_matrix = glm::mat4(1.0f);
            glm::vec3 position {};
        } camera;

        struct SceneList {
            std::vector<Renderable> renderables;
            DirectionalLight directional_light;
            // std::vector<DirectionalLight> point_lights;

            void clear();
        } scene_list;

        struct {
            std::vector<std::weak_ptr<GlShader>> shaders;
        } scene_data;

        friend class Application;
    };
}
