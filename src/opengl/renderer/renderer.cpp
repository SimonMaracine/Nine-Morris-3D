#include <memory>
#include <iostream>
#include <string>
#include <exception>
#include <string.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_truetype.h>

#include "application/application.h"
#include "opengl/renderer/renderer.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/buffer.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/texture.h"
#include "opengl/renderer/framebuffer.h"
#include "nine_morris_3d/board.h"
#include "other/logging.h"
#include "other/assets.h"

namespace renderer {
    static Storage* storage = nullptr;

    Storage* initialize(int width, int height) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        storage = new Storage;

        storage->uniform_buffer = UniformBuffer::create(nullptr, sizeof(glm::mat4));
        const char* block_name = "Matrices";
        const char* uniforms[1] = {
            "u_projection_view_matrix"
        };

        storage->board_shader = Shader::create(
            assets::path(assets::BOARD_VERTEX_SHADER),
            assets::path(assets::BOARD_FRAGMENT_SHADER),
            block_name,
            uniforms, 1,
            storage->uniform_buffer
        );

        storage->board_paint_shader = Shader::create(
            assets::path(assets::BOARD_PAINT_VERTEX_SHADER),
            assets::path(assets::BOARD_PAINT_FRAGMENT_SHADER),
            block_name,
            uniforms, 1,
            storage->uniform_buffer
        );

        storage->piece_shader = Shader::create(
            assets::path(assets::PIECE_VERTEX_SHADER),
            assets::path(assets::PIECE_FRAGMENT_SHADER),
            block_name,
            uniforms, 1,
            storage->uniform_buffer
        );

        storage->shadow_shader = Shader::create(
            assets::path(assets::SHADOW_VERTEX_SHADER),
            assets::path(assets::SHADOW_FRAGMENT_SHADER)
        );

        storage->screen_quad_shader = Shader::create(
            assets::path(assets::SCREEN_QUAD_VERTEX_SHADER),
            assets::path(assets::SCREEN_QUAD_FRAGMENT_SHADER)
        );

        storage->outline_shader = Shader::create(
            assets::path(assets::OUTLINE_VERTEX_SHADER),
            assets::path(assets::OUTLINE_FRAGMENT_SHADER),
            block_name,
            uniforms, 1,
            storage->uniform_buffer
        );

        storage->node_shader = Shader::create(
            assets::path(assets::NODE_VERTEX_SHADER),
            assets::path(assets::NODE_FRAGMENT_SHADER),
            block_name,
            uniforms, 1,
            storage->uniform_buffer
        );

        storage->skybox_shader = Shader::create(
            assets::path(assets::SKYBOX_VERTEX_SHADER),
            assets::path(assets::SKYBOX_FRAGMENT_SHADER)
        );

        storage->quad2d_shader = Shader::create(
            assets::path(assets::QUAD2D_VERTEX_SHADER),
            assets::path(assets::QUAD2D_FRAGMENT_SHADER)
        );

        storage->quad3d_shader = Shader::create(
            assets::path(assets::QUAD3D_VERTEX_SHADER),
            assets::path(assets::QUAD3D_FRAGMENT_SHADER)
        );

#ifndef NDEBUG
        storage->origin_shader = Shader::create(
            assets::path(assets::ORIGIN_VERTEX_SHADER),
            assets::path(assets::ORIGIN_FRAGMENT_SHADER),
            block_name,
            uniforms, 1,
            storage->uniform_buffer
        );
#endif

        storage->text_shader = Shader::create(
            "data/shaders/text.vert",
            "data/shaders/text.frag"
        );

        storage->depth_map_framebuffer = Framebuffer::create(Framebuffer::Type::DepthMap, 2048, 2048, 1, 0);
        storage->intermediate_framebuffer = Framebuffer::create(Framebuffer::Type::Intermediate, width, height, 1, 2);

        {
            float screen_quad_vertices[] = {
                -1.0f,  1.0f,    0.0f, 1.0f,
                -1.0f, -1.0f,    0.0f, 0.0f,
                 1.0f,  1.0f,    1.0f, 1.0f,
                 1.0f,  1.0f,    1.0f, 1.0f,
                -1.0f, -1.0f,    0.0f, 0.0f,
                 1.0f, -1.0f,    1.0f, 0.0f
            };

            Rc<Buffer> buffer = Buffer::create(screen_quad_vertices, sizeof(screen_quad_vertices));
            BufferLayout layout;
            layout.add(0, BufferLayout::Type::Float, 2);
            layout.add(1, BufferLayout::Type::Float, 2);
            storage->screen_quad_vertex_array = VertexArray::create();
            storage->screen_quad_vertex_array->add_buffer(buffer, layout);

            VertexArray::unbind();
        }

        {
            float quad2d_vertices[] = {
                0.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 1.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 0.0f
            };

            Rc<Buffer> buffer = Buffer::create(quad2d_vertices, sizeof(quad2d_vertices));
            BufferLayout layout;
            layout.add(0, BufferLayout::Type::Float, 2);
            storage->quad2d_vertex_array = VertexArray::create();
            storage->quad2d_vertex_array->add_buffer(buffer, layout);

            VertexArray::unbind();
        }

#ifndef NDEBUG
        {
            float origin_vertices[] = {
                -20.0f,   0.0f,   0.0f,    1.0f, 0.0f, 0.0f,
                 20.0f,   0.0f,   0.0f,    1.0f, 0.0f, 0.0f,
                  0.0f, -20.0f,   0.0f,    0.0f, 1.0f, 0.0f,
                  0.0f,  20.0f,   0.0f,    0.0f, 1.0f, 0.0f,
                  0.0f,   0.0f, -20.0f,    0.0f, 0.0f, 1.0f,
                  0.0f,   0.0f,  20.0f,    0.0f, 0.0f, 1.0f
            };
            Rc<Buffer> buffer = Buffer::create(origin_vertices, sizeof(origin_vertices));
            BufferLayout layout;
            layout.add(0, BufferLayout::Type::Float, 3);
            layout.add(1, BufferLayout::Type::Float, 3);
            storage->origin_vertex_array = VertexArray::create();
            storage->origin_vertex_array->add_buffer(buffer, layout);

            VertexArray::unbind();
        }
#endif

        storage->splash_screen_texture = Texture::create(assets::path(assets::SPLASH_SCREEN_TEXTURE), true);

#ifndef NDEBUG
        storage->light_texture = Texture::create("data/textures/light_bulb/light.png", false);  // TODO see what to do with this
#endif

        storage->orthographic_projection_matrix = glm::ortho(0.0f, (float) width, 0.0f, (float) height);
        storage->upside_down_ortho_projection_matrix = glm::ortho(0.0f, (float) width, (float) height, 0.0f);

        return storage;
    }

    void terminate() {
        delete storage;
    }

    void set_viewport(int width, int height) {
        glViewport(0, 0, width, height);
    }

    void set_clear_color(float red, float green, float blue) {
        glClearColor(red, green, blue, 1.0f);
    }

    void clear(int buffers) {
        glClear(buffers);
    }

    void draw_screen_quad(GLuint texture) {
        glDisable(GL_DEPTH_TEST);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        storage->screen_quad_shader->bind();
        storage->screen_quad_shader->set_uniform_int("u_screen_texture", 0);
        storage->screen_quad_vertex_array->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_DEPTH_TEST);
    }

    void enable_depth() {
        glEnable(GL_DEPTH_TEST);
    }

    void disable_depth() {
        glDisable(GL_DEPTH_TEST);
    }

    void enable_stencil() {
        glEnable(GL_STENCIL_TEST);
    }

    void disable_stencil() {
        glDisable(GL_STENCIL_TEST);
    }

#ifndef NDEBUG
    void draw_origin() {
        storage->origin_shader->bind();
        storage->origin_vertex_array->bind();
        glDrawArrays(GL_LINES, 0, 6);
    }
#endif

    void draw_quad_2d(const glm::vec3& position, float scale, Rc<Texture> texture) {
        glDisable(GL_DEPTH_TEST);

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::scale(matrix, glm::vec3(texture->get_width(), texture->get_height(), 1.0f));
        matrix = glm::scale(matrix, glm::vec3(scale, scale, 1.0f));

        storage->quad2d_shader->bind();
        storage->quad2d_shader->set_uniform_matrix("u_model_matrix", matrix);
        storage->quad2d_shader->set_uniform_int("u_texture", 0);

        texture->bind(0);

        storage->quad2d_vertex_array->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_DEPTH_TEST);
    }

    void draw_quad_3d(const glm::vec3& position, float scale, Rc<Texture> texture) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));

        storage->quad3d_shader->bind();
        storage->quad3d_shader->set_uniform_matrix("u_model_matrix", matrix);
        storage->quad3d_shader->set_uniform_int("u_texture", 0);

        texture->bind(0);

        storage->screen_quad_vertex_array->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void bind_texture(GLuint texture, GLenum slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void set_stencil_mask_zero() {
        glStencilMask(0x00);
    }

    void load_projection_view(const glm::mat4& projection_view) {
        constexpr size_t SIZE = sizeof(glm::mat4);
        float* buffer[SIZE];
        memcpy(buffer, glm::value_ptr(projection_view), SIZE);
        storage->uniform_buffer->update_data(buffer, SIZE);
    }

    void disable_output_to_red(unsigned int index) {
        glColorMaski(index, GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    void enable_output_to_red(unsigned int index) {
        glColorMaski(index, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    void draw_string(const std::string& string, const glm::vec2& position, float scale,
            const glm::vec3& color, std::shared_ptr<Font> font) {
        const size_t SIZE = sizeof(float) * string.length() * 24;

        float* buffer = new float[SIZE];
        unsigned int buffer_index = 0;

        int x = 0;

        for (unsigned char character : string) {
            const Font::Glyph* glyph;

            try {
                glyph = &font->get_glyphs().at(character);
            } catch (const std::out_of_range&) {
                glyph = &font->get_glyphs()[127];
            }

            const float x0 = (float) (x + glyph->xoff);
            const float y0 = (float) -(glyph->height - glyph->yoff);
            const float x1 = (float) (x + glyph->xoff + glyph->width);
            const float y1 = (float) glyph->yoff;

            buffer[buffer_index++] = x0;
            buffer[buffer_index++] = y1;
            buffer[buffer_index++] = glyph->s0;
            buffer[buffer_index++] = glyph->t0;

            buffer[buffer_index++] = x0;
            buffer[buffer_index++] = y0;
            buffer[buffer_index++] = glyph->s0;
            buffer[buffer_index++] = glyph->t1;

            buffer[buffer_index++] = x1;
            buffer[buffer_index++] = y1;
            buffer[buffer_index++] = glyph->s1;
            buffer[buffer_index++] = glyph->t0;

            buffer[buffer_index++] = x1;
            buffer[buffer_index++] = y1;
            buffer[buffer_index++] = glyph->s1;
            buffer[buffer_index++] = glyph->t0;

            buffer[buffer_index++] = x0;
            buffer[buffer_index++] = y0;
            buffer[buffer_index++] = glyph->s0;
            buffer[buffer_index++] = glyph->t1;

            buffer[buffer_index++] = x1;
            buffer[buffer_index++] = y0;
            buffer[buffer_index++] = glyph->s1;
            buffer[buffer_index++] = glyph->t1;

            x += glyph->xadvance;
        }

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(position, 0.0f));
        matrix = glm::scale(matrix, glm::vec3(scale, scale, 1.0f));

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        font->update_data(buffer, SIZE);
        delete[] buffer;

        storage->text_shader->bind();
        storage->text_shader->set_uniform_matrix("u_projection_matrix", storage->orthographic_projection_matrix);
        storage->text_shader->set_uniform_matrix("u_transformation_matrix", matrix);
        storage->text_shader->set_uniform_int("u_bitmap", 0);  // TODO this shouldn't be set every frame
        storage->text_shader->set_uniform_vec3("u_color", color);

        font->get_vertex_array()->bind();
        bind_texture(font->get_texture(), 0);
        glDrawArrays(GL_TRIANGLES, 0, font->get_vertex_count());

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }

    void draw_board(const Board& board) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::scale(matrix, glm::vec3(board.scale, board.scale, board.scale));

        storage->board_shader->bind();
        storage->board_shader->set_uniform_matrix("u_model_matrix", matrix);

        board.vertex_array->bind();
        board.diffuse_texture->bind(0);
        glDrawElements(GL_TRIANGLES, board.index_count, GL_UNSIGNED_INT, nullptr);
    }

    void draw_board_paint(const BoardPaint& board_paint) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, board_paint.position);
        matrix = glm::scale(matrix, glm::vec3(board_paint.scale, board_paint.scale, board_paint.scale));

        storage->board_paint_shader->bind();
        storage->board_paint_shader->set_uniform_matrix("u_model_matrix", matrix);

        board_paint.vertex_array->bind();
        board_paint.diffuse_texture->bind(0);
        glDrawElements(GL_TRIANGLES, board_paint.index_count, GL_UNSIGNED_INT, nullptr);
    }

    void draw_piece(const Piece* piece, const glm::vec3& tint_color) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, piece->position);
        matrix = glm::rotate(matrix, piece->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, piece->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, piece->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(piece->scale, piece->scale, piece->scale));

        storage->piece_shader->bind();
        storage->piece_shader->set_uniform_matrix("u_model_matrix", matrix);
        storage->piece_shader->set_uniform_vec3("u_tint_color", tint_color);

        piece->vertex_array->bind();
        piece->diffuse_texture->bind(0);
        glDrawElements(GL_TRIANGLES, piece->index_count, GL_UNSIGNED_INT, nullptr);
    }

    void draw_piece_with_outline(const Piece* piece, const glm::vec3& outline_color) {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        {
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, piece->position);
            matrix = glm::rotate(matrix, piece->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matrix = glm::rotate(matrix, piece->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            matrix = glm::rotate(matrix, piece->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            matrix = glm::scale(matrix, glm::vec3(piece->scale, piece->scale, piece->scale));

            storage->piece_shader->bind();
            storage->piece_shader->set_uniform_matrix("u_model_matrix", matrix);

            piece->vertex_array->bind();
            piece->diffuse_texture->bind(0);
            glDrawElements(GL_TRIANGLES, piece->index_count, GL_UNSIGNED_INT, nullptr);
        }

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        {
            constexpr float SIZE = 3.6f;

            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, piece->position);
            matrix = glm::rotate(matrix, piece->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matrix = glm::rotate(matrix, piece->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            matrix = glm::rotate(matrix, piece->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            matrix = glm::scale(matrix, glm::vec3(piece->scale + SIZE, piece->scale + SIZE,
                    piece->scale + SIZE));

            storage->outline_shader->bind();
            storage->outline_shader->set_uniform_matrix("u_model_matrix", matrix);
            storage->outline_shader->set_uniform_vec3("u_color", outline_color);

            glDrawElements(GL_TRIANGLES, piece->index_count, GL_UNSIGNED_INT, nullptr);
        }

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);
    }

    void draw_skybox(const glm::mat4& projection_view_matrix) {
        glDepthMask(GL_FALSE);

        storage->skybox_shader->bind();
        storage->skybox_shader->set_uniform_matrix("u_projection_view_matrix", projection_view_matrix);
        storage->skybox_shader->set_uniform_int("u_skybox", 0);  // TODO this shouldn't be set every frame

        storage->skybox_vertex_array->bind();
        storage->skybox_texture->bind(0);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthMask(GL_TRUE);
    }

    void draw_node(const Node& node, const glm::vec4& color) {
        glCullFace(GL_FRONT);

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, node.position);
        matrix = glm::scale(matrix, glm::vec3(node.scale, node.scale, node.scale));

        storage->node_shader->bind();
        storage->node_shader->set_uniform_matrix("u_model_matrix", matrix);
        storage->node_shader->set_uniform_vec4("u_color", color);

        node.vertex_array->bind();
        glDrawElements(GL_TRIANGLES, node.index_count, GL_UNSIGNED_INT, nullptr);

        glCullFace(GL_BACK);
    }

    void draw_to_depth(const glm::vec3& position, const glm::vec3& rotation, float scale,
            Rc<VertexArray> vertex_array, int index_count) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));

        storage->shadow_shader->bind();
        storage->shadow_shader->set_uniform_matrix("u_model_matrix", matrix);

        vertex_array->bind();
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    }
}
