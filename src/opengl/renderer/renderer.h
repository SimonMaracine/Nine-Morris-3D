#pragma once

#include <memory>
#include <array>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/buffer.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/texture.h"
#include "opengl/renderer/framebuffer.h"
#include "nine_morris_3d/hoverable.h"

namespace renderer {
    enum {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT
    };

    struct Storage {
        Rc<Buffer> uniform_buffer;

        Rc<Shader> board_shader;
        Rc<Shader> board_paint_shader;
        Rc<Shader> piece_shader;
        Rc<Shader> shadow_shader;
        Rc<Shader> screen_quad_shader;
        Rc<Shader> outline_shader;
        Rc<Shader> node_shader;
        Rc<Shader> skybox_shader;
        Rc<Shader> quad2d_shader;
        Rc<Shader> quad3d_shader;

#ifndef NDEBUG
        Rc<Shader> origin_shader;
#endif

        Rc<Framebuffer> scene_framebuffer;
        Rc<Framebuffer> depth_map_framebuffer;
        Rc<Framebuffer> intermediate_framebuffer;

        Rc<VertexArray> screen_quad_vertex_array;
        Rc<VertexArray> quad2d_vertex_array;

#ifndef NDEBUG
        Rc<VertexArray> origin_vertex_array;
#endif

        // TODO store them here?
        Rc<Texture> board_diffuse_texture;
        Rc<Texture> board_paint_texture;
        Rc<Texture> white_piece_diffuse_texture;
        Rc<Texture> black_piece_diffuse_texture;
        Rc<Texture3D> skybox_texture;
        Rc<Texture> white_indicator_texture;
        Rc<Texture> black_indicator_texture;
        Rc<Texture> loading_texture;

#ifndef NDEBUG
        Rc<Texture> light_texture;
#endif

        Rc<VertexArray> board_vertex_array;
        Rc<VertexArray> board_paint_vertex_array;
        std::array<Rc<VertexArray>, 18> piece_vertex_arrays;
        std::array<Rc<VertexArray>, 24> node_vertex_arrays;
        Rc<VertexArray> skybox_vertex_array;

        glm::mat4 orthographic_projection_matrix;

        unsigned int board_id = HOVERABLE_NULL;
        unsigned int pieces_id[18] = { HOVERABLE_NULL };
        unsigned int nodes_id[24] = { HOVERABLE_NULL };
    };

    Storage* initialize(int width, int height);
    void terminate();
    void set_viewport(int width, int height);
    void set_clear_color(float red, float green, float blue);
    void clear(int buffers);
    void draw_screen_quad(GLuint texture);
    void enable_depth();
    void disable_depth();
    void enable_stencil();
    void disable_stencil();
#ifndef NDEBUG
    void draw_origin();
#endif
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
