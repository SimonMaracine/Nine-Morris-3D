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
        std::shared_ptr<Shader> board_shader = nullptr;
        std::shared_ptr<Shader> piece_shader = nullptr;
        std::shared_ptr<Framebuffer> framebuffer = nullptr;

        std::shared_ptr<Shader> quad_shader = nullptr;
        std::shared_ptr<VertexArray> quad_vertex_array = nullptr;
        std::shared_ptr<VertexBuffer> quad_vertex_buffer = nullptr;

        std::shared_ptr<Shader> outline_shader = nullptr;
        std::shared_ptr<Shader> node_shader = nullptr;
        std::shared_ptr<Shader> skybox_shader = nullptr;

        std::shared_ptr<Shader> origin_shader = nullptr;
        std::shared_ptr<VertexArray> origin_vertex_array = nullptr;
        std::shared_ptr<VertexBuffer> origin_vertex_buffer = nullptr;

        std::shared_ptr<Shader> light_shader = nullptr;
        std::shared_ptr<VertexArray> light_vertex_array = nullptr;
        std::shared_ptr<VertexBuffer> light_vertex_buffer = nullptr;
        std::shared_ptr<Texture> light_texture = nullptr;

        std::shared_ptr<Shader> loading_shader = nullptr;
        std::shared_ptr<VertexArray> loading_vertex_array = nullptr;
        std::shared_ptr<VertexBuffer> loading_vertex_buffer = nullptr;
        std::shared_ptr<Texture> loading_texture = nullptr;
    };

    const Storage* init();
    void terminate();
    void set_viewport(GLint width, GLint height);
    void set_clear_color(GLfloat red, GLfloat green, GLfloat blue);
    void clear(int buffers);
    void draw_quad();
    void draw_loading();
    void draw_origin();
    void draw_light(const glm::vec3& position);
    void enable_depth();
    void disable_depth();
    void enable_stencil();
    void disable_stencil();
    void bind_texture(GLuint texture);
    void set_stencil_mask_zero();

    void draw_board(const glm::vec3& position,
                    const glm::vec3& rotation,
                    float scale,
                    std::shared_ptr<Shader> shader,
                    std::shared_ptr<VertexArray> array,
                    std::shared_ptr<Texture> diffuse_map,
                    const glm::vec3& specular_color,
                    float shininess,
                    GLuint index_count);

    void draw_piece(const glm::vec3& position,
                    const glm::vec3& rotation,
                    float scale,
                    std::shared_ptr<Shader> shader,
                    std::shared_ptr<VertexArray> array,
                    std::shared_ptr<Texture> diffuse_map,
                    const glm::vec3& specular_color,
                    float shininess,
                    GLuint index_count,
                    const glm::vec3& tint_color);

    void draw_piece_outline(const glm::vec3& position,
                            const glm::vec3& rotation,
                            float scale,
                            std::shared_ptr<Shader> shader,
                            std::shared_ptr<VertexArray> array,
                            std::shared_ptr<Texture> diffuse_map,
                            const glm::vec3& specular_color,
                            float shininess,
                            GLuint index_count,
                            const glm::vec3& outline_color);

    void draw_cube_map(const glm::mat4& view_projection_matrix,
                       std::shared_ptr<Shader> shader, std::shared_ptr<VertexArray> array,
                       std::shared_ptr<Texture3D> texture);

    void draw_node(const glm::vec3& position,
                   float scale,
                   std::shared_ptr<Shader> shader,
                   std::shared_ptr<VertexArray> array,
                   const glm::vec4& color,
                   GLuint index_count);
}
