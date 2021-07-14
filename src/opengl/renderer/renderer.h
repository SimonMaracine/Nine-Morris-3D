#pragma once

#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/texture.h"
#include "opengl/renderer/framebuffer.h"

namespace renderer {
    enum {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT
    };

    struct Storage {
        std::shared_ptr<Shader> basic_shader = nullptr;
        std::shared_ptr<Framebuffer> framebuffer = nullptr;

        std::shared_ptr<Shader> quad_shader = nullptr;
        std::shared_ptr<VertexArray> quad_vertex_array = nullptr;
        std::shared_ptr<VertexBuffer> quad_vertex_buffer = nullptr;

        std::shared_ptr<Shader> outline_shader = nullptr;
    };

    const Storage* init();
    void set_viewport(GLint width, GLint height);
    void set_clear_color(GLfloat red, GLfloat green, GLfloat blue);
    void clear(int buffers);
    // void begin(std::shared_ptr<Shader> shader, const glm::mat4& view_projection_matrix);
    // void end();
    void draw_quad();
    void enable_depth();
    void disable_depth();
    void bind_texture(GLuint texture);
    void set_stencil_mask_zero();

    void draw_model(const glm::vec3& position, const glm::vec3& rotation,
                    float scale, std::shared_ptr<Shader> shader,
                    std::shared_ptr<VertexArray> array,
                    std::shared_ptr<Texture> diffuse_map, GLuint index_count);
    void draw_model_outline(const glm::vec3& position, const glm::vec3& rotation,
                            float scale, std::shared_ptr<Shader> shader,
                            std::shared_ptr<VertexArray> array,
                            std::shared_ptr<Texture> diffuse_map, GLuint index_count,
                            const glm::vec3& outline_color);
    void draw_cube_map(const glm::mat4& view_projection_matrix,
                       std::shared_ptr<Shader> shader, std::shared_ptr<VertexArray> array,
                       std::shared_ptr<Texture3D> texture);
}
