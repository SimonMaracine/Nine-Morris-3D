#pragma once

#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/buffer.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/texture.h"
#include "opengl/renderer/framebuffer.h"

#define DEFAULT_MSAA 2

namespace renderer {
    enum {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT
    };

    struct Storage {
        Rc<Buffer> uniform_buffer = nullptr;

        Rc<Shader> board_shader = nullptr;
        Rc<Shader> board_paint_shader = nullptr;
        Rc<Shader> piece_shader = nullptr;
        Rc<Shader> shadow_shader = nullptr;
        Rc<Shader> screen_quad_shader = nullptr;
        Rc<Shader> outline_shader = nullptr;
        Rc<Shader> node_shader = nullptr;
        Rc<Shader> skybox_shader = nullptr;

        Rc<Framebuffer> scene_framebuffer = nullptr;
        Rc<Framebuffer> depth_map_framebuffer = nullptr;
        Rc<Framebuffer> intermediate_framebuffer = nullptr;

        Rc<VertexArray> screen_quad_vertex_array = nullptr;
        Rc<Buffer> screen_quad_vertex_buffer = nullptr;

        Rc<VertexArray> quad2d_vertex_array = nullptr;
        Rc<Buffer> quad2d_vertex_buffer = nullptr;

        Rc<Shader> origin_shader = nullptr;
        Rc<VertexArray> origin_vertex_array = nullptr;
        Rc<Buffer> origin_vertex_buffer = nullptr;

        Rc<Shader> quad2d_shader = nullptr;
        Rc<Shader> quad3d_shader = nullptr;

        Rc<Texture> light_texture = nullptr;
        Rc<Texture> loading_texture = nullptr;

        glm::mat4 orthographic_projection_matrix;

        Rc<Texture> board_diffuse_texture = nullptr;
        Rc<Texture> board_paint_texture = nullptr;
        Rc<Texture> white_piece_diffuse_texture = nullptr;
        Rc<Texture> black_piece_diffuse_texture = nullptr;
        Rc<VertexArray> skybox_vertex_array = nullptr;
        Rc<Texture3D> skybox_texture = nullptr;
        Rc<Texture> white_indicator_texture = nullptr;
        Rc<Texture> black_indicator_texture = nullptr;
    };

    Storage* init(int width, int height);
    void terminate();
    void set_viewport(GLint width, GLint height);
    void set_clear_color(GLfloat red, GLfloat green, GLfloat blue);
    void clear(int buffers);
    void draw_screen_quad();
    void draw_loading();
    void draw_origin();
    void draw_quad_2d(const glm::vec3& position, float scale, Rc<Texture> texture);
    void draw_quad_3d(const glm::vec3& position, float scale, Rc<Texture> texture);
    void bind_texture(GLuint texture, GLenum slot);
    void set_stencil_mask_zero();
    void load_projection_view(const glm::mat4& matrix);

    void draw_board(const glm::vec3& position,
                    const glm::vec3& rotation,
                    float scale,
                    Rc<VertexArray> vertex_array,
                    const glm::vec3& specular_color,
                    float shininess,
                    GLuint index_count);

    void draw_board_paint(const glm::vec3& position,
                          const glm::vec3& rotation,
                          float scale,
                          Rc<VertexArray> vertex_array,
                          const glm::vec3& specular_color,
                          float shininess,
                          GLuint index_count);

    void draw_piece(const glm::vec3& position,
                    const glm::vec3& rotation,
                    float scale,
                    Rc<VertexArray> vertex_array,
                    Rc<Texture> diffuse_texture,
                    const glm::vec3& specular_color,
                    float shininess,
                    GLuint index_count,
                    const glm::vec3& tint_color);

    void draw_piece_outline(const glm::vec3& position,
                            const glm::vec3& rotation,
                            float scale,
                            Rc<VertexArray> vertex_array,
                            Rc<Texture> diffuse_texture,
                            const glm::vec3& specular_color,
                            float shininess,
                            GLuint index_count,
                            const glm::vec3& outline_color);

    void draw_skybox(const glm::mat4& view_projection_matrix);

    void draw_node(const glm::vec3& position,
                   float scale,
                   Rc<VertexArray> vertex_array,
                   const glm::vec4& color,
                   GLuint index_count);

    void draw_to_depth(const glm::vec3& position,
                       const glm::vec3& rotation,
                       float scale,
                       Rc<VertexArray> vertex_array,
                       GLuint index_count);
}
