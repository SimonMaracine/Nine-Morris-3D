#pragma once

#include <array>
#include <string>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "application/platform.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/buffer.h"
#include "graphics/renderer/shader.h"
#include "graphics/renderer/texture.h"
#include "graphics/renderer/framebuffer.h"
#include "graphics/renderer/font.h"
#include "graphics/renderer/hoverable.h"
#include "nine_morris_3d/board.h"

class Application;

namespace renderer {
    enum {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT
    };

    struct Storage {
        std::shared_ptr<UniformBuffer> uniform_buffer;
        std::array<std::shared_ptr<PixelBuffer>, 4> pixel_buffers;

        std::shared_ptr<Shader> board_shader;
        std::shared_ptr<Shader> board_paint_shader;
        std::shared_ptr<Shader> piece_shader;
        std::shared_ptr<Shader> shadow_shader;
        std::shared_ptr<Shader> screen_quad_shader;
        std::shared_ptr<Shader> outline_shader;
        std::shared_ptr<Shader> node_shader;
        std::shared_ptr<Shader> skybox_shader;
        std::shared_ptr<Shader> quad2d_shader;
        std::shared_ptr<Shader> quad3d_shader;
#ifdef NINE_MORRIS_3D_DEBUG
        std::shared_ptr<Shader> origin_shader;
#endif
        std::shared_ptr<Shader> text_shader;

        std::shared_ptr<Framebuffer> scene_framebuffer;
        std::shared_ptr<Framebuffer> depth_map_framebuffer;
        std::shared_ptr<Framebuffer> intermediate_framebuffer;

        std::shared_ptr<VertexArray> screen_quad_vertex_array;
        std::shared_ptr<VertexArray> quad2d_vertex_array;
#ifdef NINE_MORRIS_3D_DEBUG
        std::shared_ptr<VertexArray> origin_vertex_array;
#endif

        std::shared_ptr<Texture> board_wood_diff_texture;
        std::shared_ptr<Texture> board_paint_diff_texture;
        std::shared_ptr<Texture> white_piece_diff_texture;
        std::shared_ptr<Texture> black_piece_diff_texture;
        std::shared_ptr<Texture3D> skybox_texture;
        std::shared_ptr<Texture> white_indicator_texture;
        std::shared_ptr<Texture> black_indicator_texture;
        std::shared_ptr<Texture> splash_screen_texture;
#ifdef NINE_MORRIS_3D_DEBUG
        std::shared_ptr<Texture> light_texture;
#endif

        std::shared_ptr<VertexArray> board_vertex_array;
        std::shared_ptr<VertexArray> board_paint_vertex_array;
        std::array<std::shared_ptr<VertexArray>, 18> piece_vertex_arrays;
        std::array<std::shared_ptr<VertexArray>, 24> node_vertex_arrays;
        std::shared_ptr<VertexArray> skybox_vertex_array;

        glm::mat4 orthographic_projection_matrix;

        hoverable::Id board_id = hoverable::null;
        hoverable::Id pieces_id[18] = { hoverable::null };
        hoverable::Id nodes_id[24] = { hoverable::null };

        std::shared_ptr<Font> good_dog_plain_font;
    };

    Storage* initialize(Application* app);
    void terminate();
    void set_viewport(int width, int height);
    void set_clear_color(float red, float green, float blue);
    void clear(int buffers);
    void draw_screen_quad(GLuint texture);
    void enable_depth();
    void disable_depth();
    void enable_stencil();
    void disable_stencil();
#ifdef NINE_MORRIS_3D_DEBUG
    void draw_origin();
#endif
    void draw_quad_2d(const glm::vec2& position, float additional_scale, std::shared_ptr<Texture> texture);
    void draw_quad_2d(const glm::vec2& position, const glm::vec2& scale, std::shared_ptr<Texture> texture);
    void draw_quad_3d(const glm::vec3& position, float scale, std::shared_ptr<Texture> texture);
    void bind_texture(GLuint texture, GLenum slot);
    void set_stencil_mask_zero();
    void load_projection_view(const glm::mat4& matrix);
    void disable_output_to_red(unsigned int index);
    void enable_output_to_red(unsigned int index);
    void draw_string(const std::string& string, const glm::vec2& position, float scale,
                const glm::vec3& color, std::shared_ptr<Font> font);
    void draw_string_with_shadows(const std::string& string, const glm::vec2& position, float scale,
                const glm::vec3& color, std::shared_ptr<Font> font);
    void draw_board(const Board& board);
//     void draw_board_paint(const BoardPaint& board_paint);
    void draw_piece(const Piece* piece, const glm::vec3& tint_color);
    void draw_piece_with_outline(const Piece* piece, const glm::vec3& outline_color);
    void draw_skybox(const glm::mat4& projection_view_matrix);
    void draw_node(const Node& node, const glm::vec4& color);
    void draw_to_depth(const glm::vec3& position, const glm::vec3& rotation, float scale,
            std::shared_ptr<VertexArray> vertex_array, int index_count);
}
