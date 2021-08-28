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
        Rc<Shader> piece_shader = nullptr;
        Rc<Shader> shadow_shader = nullptr;
        Rc<Framebuffer> scene_framebuffer = nullptr;
        Rc<Framebuffer> depth_map_framebuffer = nullptr;
        Rc<Framebuffer> intermediate_framebuffer = nullptr;

        Rc<Shader> quad_shader = nullptr;
        Rc<VertexArray> quad_vertex_array = nullptr;
        Rc<Buffer> quad_vertex_buffer = nullptr;

        Rc<Shader> outline_shader = nullptr;
        Rc<Shader> node_shader = nullptr;
        Rc<Shader> skybox_shader = nullptr;

        Rc<Shader> origin_shader = nullptr;
        Rc<VertexArray> origin_vertex_array = nullptr;
        Rc<Buffer> origin_vertex_buffer = nullptr;

        Rc<Shader> light_shader = nullptr;
        Rc<VertexArray> light_vertex_array = nullptr;
        Rc<Buffer> light_vertex_buffer = nullptr;
        Rc<Texture> light_texture = nullptr;

        Rc<Shader> loading_shader = nullptr;
        Rc<VertexArray> loading_vertex_array = nullptr;
        Rc<Buffer> loading_vertex_buffer = nullptr;
        Rc<Texture> loading_texture = nullptr;
    };

    Storage* init(int width, int height);
    void terminate();
    void set_viewport(GLint width, GLint height);
    void set_clear_color(GLfloat red, GLfloat green, GLfloat blue);
    void clear(int buffers);
    void draw_quad();
    void draw_loading();
    void draw_origin();
    void draw_light(const glm::vec3& position);
    void bind_texture(GLuint texture, GLenum slot);
    void set_stencil_mask_zero();
    void load_projection_view(const glm::mat4& matrix);

    void draw_board(const glm::vec3& position,
                    const glm::vec3& rotation,
                    float scale,
                    Rc<Shader> shader,
                    Rc<VertexArray> array,
                    Rc<Texture> diffuse_map,
                    const glm::vec3& specular_color,
                    float shininess,
                    GLuint index_count);

    void draw_piece(const glm::vec3& position,
                    const glm::vec3& rotation,
                    float scale,
                    Rc<Shader> shader,
                    Rc<VertexArray> array,
                    Rc<Texture> diffuse_map,
                    const glm::vec3& specular_color,
                    float shininess,
                    GLuint index_count,
                    const glm::vec3& tint_color);

    void draw_piece_outline(const glm::vec3& position,
                            const glm::vec3& rotation,
                            float scale,
                            Rc<Shader> shader,
                            Rc<VertexArray> array,
                            Rc<Texture> diffuse_map,
                            const glm::vec3& specular_color,
                            float shininess,
                            GLuint index_count,
                            const glm::vec3& outline_color);

    void draw_cube_map(const glm::mat4& view_projection_matrix,
                       Rc<Shader> shader, Rc<VertexArray> array,
                       Rc<Texture3D> texture);

    void draw_node(const glm::vec3& position,
                   float scale,
                   Rc<Shader> shader,
                   Rc<VertexArray> array,
                   const glm::vec4& color,
                   GLuint index_count);

    void draw_to_depth(const glm::vec3& position,
                       const glm::vec3& rotation,
                       float scale,
                       Rc<Shader> shader,
                       Rc<VertexArray> array,
                       GLuint index_count);
}
