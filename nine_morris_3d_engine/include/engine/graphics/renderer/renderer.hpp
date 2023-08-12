#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "engine/graphics/opengl/vertex_array.hpp"
#include "engine/graphics/opengl/buffer.hpp"
#include "engine/graphics/opengl/framebuffer.hpp"
#include "engine/graphics/material.hpp"
#include "engine/graphics/post_processing.hpp"
#include "engine/graphics/camera.hpp"

namespace sm {
    struct Renderable {
        std::shared_ptr<GlVertexArray> vertex_array;
        std::shared_ptr<GlIndexBuffer> index_buffer;
        std::shared_ptr<MaterialInstance> material;

        glm::vec3 position {};
        glm::vec3 rotation {};
        float scale = 1.0f;

        glm::vec3 outline_color {};
    };

    class Renderer {
    public:
        Renderer(int width, int height);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        // TODO add light
        void add_renderable(const Renderable& renderable);

        void capture(const Camera& camera, const glm::vec3& position);

        void acknowledge_shader(std::shared_ptr<GlShader> shader);
    private:
        void render(int width, int height);

        void draw_screen_quad(unsigned int texture);
        void post_processing();
        void end_rendering();

        void draw_renderables();
        void draw_renderable(const Renderable& renderable);

        void draw_renderables_outlined();
        void draw_renderable_outlined(const Renderable& renderable);

        struct Storage {
            std::shared_ptr<GlFramebuffer> scene_framebuffer;

            std::shared_ptr<GlUniformBuffer> projection_view_uniform_buffer;

            std::shared_ptr<GlShader> screen_quad_shader;
            std::shared_ptr<GlVertexArray> screen_quad_vertex_array;
            std::shared_ptr<GlVertexBuffer> screen_quad_vertex_buffer;
        } storage;

        PostProcessingContext post_processing_context;  // TODO implement

        struct {
            glm::mat4 view_matrix = glm::mat4(1.0f);
            glm::mat4 projection_matrix = glm::mat4(1.0f);
            glm::mat4 projection_view_matrix = glm::mat4(1.0f);
            glm::vec3 position {};
        } camera;

        std::vector<Renderable> scene_list;

        friend class Application;
    };
}
