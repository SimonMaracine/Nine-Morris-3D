#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <resmanager/resmanager.hpp>

#include "engine/graphics/opengl/vertex_array.hpp"
#include "engine/graphics/opengl/buffer.hpp"
#include "engine/graphics/opengl/framebuffer.hpp"
#include "engine/graphics/opengl/shader.hpp"
#include "engine/graphics/renderer/renderer.hpp"
#include "engine/graphics/renderer/render_gl.hpp"
#include "engine/graphics/screen.hpp"
#include "engine/graphics/post_processing.hpp"
#include "engine/graphics/camera.hpp"
#include "engine/graphics/renderable.hpp"
#include "engine/other/file_system.hpp"

using namespace resmanager::literals;

namespace sm {
    static constexpr unsigned int PROJECTON_VIEW_UNIFORM_BLOCK_BINDING = 0;

    Renderer::Renderer(Screen& screen, int width, int height) {
        RenderGl::enable_depth_test();

        {
            FramebufferSpecification specification;
            specification.width = width;
            specification.height = height;
            specification.color_attachments = {
                Attachment(AttachmentFormat::Rgba8, AttachmentType::Texture)
            };
            specification.depth_attachment = Attachment(
                AttachmentFormat::Depth32, AttachmentType::Renderbuffer
            );

            storage.scene_framebuffer = std::make_shared<GlFramebuffer>(specification);

            screen.add_framebuffer(storage.scene_framebuffer);
        }

        {
            storage.screen_quad_shader = std::make_shared<GlShader>(
                Encrypt::encr(FileSystem::path_engine_data("shaders/screen_quad.vert")),
                Encrypt::encr(FileSystem::path_engine_data("shaders/screen_quad.frag"))
            );

            const float vertices[] = {
                -1.0f,  1.0f,
                -1.0f, -1.0f,
                 1.0f,  1.0f,
                 1.0f,  1.0f,
                -1.0f, -1.0f,
                 1.0f, -1.0f
            };

            storage.screen_quad_vertex_buffer = std::make_shared<GlVertexBuffer>(vertices, sizeof(vertices));

            VertexBufferLayout layout;
            layout.add(0, VertexBufferLayout::Float, 2);

            storage.screen_quad_vertex_array = std::make_shared<GlVertexArray>();
            storage.screen_quad_vertex_array->bind();
            storage.screen_quad_vertex_array->add_vertex_buffer(storage.screen_quad_vertex_buffer, layout);
            GlVertexArray::unbind();
        }
    }

    Renderer::~Renderer() {

    }

    void Renderer::add_renderable(const Renderable& renderable) {
        scene_list.push_back(renderable);
    }

    void Renderer::capture(const Camera& camera, const glm::vec3& position) {
        this->camera.view_matrix = camera.view_matrix;
        this->camera.projection_matrix = camera.projection_matrix;
        this->camera.projection_view_matrix = camera.projection_view_matrix;
        this->camera.position = position;
    }

    void Renderer::add_shader(std::shared_ptr<GlShader> shader) {
        scene_data.shaders.push_back(shader);
    }

    void Renderer::render(int width, int height) {
        // TODO pre-render setup

        {
            auto uniform_buffer = storage.projection_view_uniform_buffer.lock();

            if (uniform_buffer != nullptr) {
                uniform_buffer->set(&camera.projection_view_matrix, "u_projection_view_matrix"_H);
            }
        }

        for (std::weak_ptr<GlUniformBuffer> wuniform_buffer : storage.uniform_buffers) {
            std::shared_ptr<GlUniformBuffer> uniform_buffer = wuniform_buffer.lock();

            if (uniform_buffer == nullptr) {
                continue;
            }

            uniform_buffer->bind();
            uniform_buffer->upload();
        }

        GlUniformBuffer::unbind();

        // TODO draw to depth buffer for shadows

        storage.scene_framebuffer->bind();

        RenderGl::clear(RenderGl::Buffers::CD);
        RenderGl::viewport(
            storage.scene_framebuffer->get_specification().width,
            storage.scene_framebuffer->get_specification().height
        );

        draw_renderables();
        draw_renderables_outlined();

        // Do post processing and render the final 3D image to the screen
        end_rendering();

        scene_list.clear();
    }

    void Renderer::prerender_setup() {
        for (std::weak_ptr<GlShader> wshader : scene_data.shaders) {
            std::shared_ptr<GlShader> shader = wshader.lock();

            if (shader == nullptr) {
                continue;
            }

            for (const UniformBlockSpecification& block : shader->uniform_blocks) {
                auto uniform_buffer = std::make_shared<GlUniformBuffer>(block);
                shader->add_uniform_buffer(uniform_buffer);

                storage.uniform_buffers.push_back(uniform_buffer);

                if (block.binding_index == PROJECTON_VIEW_UNIFORM_BLOCK_BINDING) {
                    storage.projection_view_uniform_buffer = uniform_buffer;
                }
            }
        }
    }

    void Renderer::postrender_setup() {
        scene_data.shaders.clear();
    }

    void Renderer::draw_screen_quad(unsigned int texture) {
        storage.screen_quad_shader->bind();
        RenderGl::bind_texture_2d(texture, 0);
        RenderGl::draw_arrays(6);
    }

    void Renderer::post_processing() {

    }

    void Renderer::end_rendering() {
        storage.screen_quad_vertex_array->bind();

        RenderGl::disable_depth_test();
        RenderGl::clear_color(0.0f, 0.0f, 0.0f);

        post_processing();

        // Draw the final result to the screen; don't need clearing
        GlFramebuffer::bind_default();

        // draw_screen_quad(post_processing_context.last_texture);  // FIXME
        draw_screen_quad(storage.scene_framebuffer->get_color_attachment(0));

        RenderGl::clear_color(0.3f, 0.1f, 0.3f);
        RenderGl::enable_depth_test();

        GlVertexArray::unbind();
    }

    void Renderer::draw_renderables() {
        for (const Renderable& renderable : scene_list) {
            if (renderable.material->flags & Material::Outline) {
                continue;  // This one is rendered differently
            }

            draw_renderable(renderable);
        }

        GlVertexArray::unbind();  // Don't unbind for every renderable
    }

    void Renderer::draw_renderable(const Renderable& renderable) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, renderable.position);
        matrix = glm::rotate(matrix, renderable.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, renderable.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, renderable.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(renderable.scale));

        renderable.vertex_array->bind();
        renderable.material->bind_and_upload();

        renderable.material->get_shader()->upload_uniform_mat4("u_model_matrix"_H, matrix);

        RenderGl::draw_elements(renderable.vertex_array->get_index_buffer()->get_index_count());
    }

    void Renderer::draw_renderables_outlined() {

    }

    void Renderer::draw_renderable_outlined(const Renderable& renderable) {

    }
}
