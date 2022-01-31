#include <memory>
#include <iostream>
#include <string>
#include <exception>
#include <array>
#include <string.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_truetype.h>

#include "application/application.h"
#include "graphics/renderer/renderer.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/buffer.h"
#include "graphics/renderer/shader.h"
#include "graphics/renderer/texture.h"
#include "graphics/renderer/framebuffer.h"
#include "nine_morris_3d/board.h"
#include "other/logging.h"
#include "other/assets.h"

namespace renderer {
    static Storage* storage = nullptr;

    Storage* initialize(Application* app) {
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

        {
            const std::vector<std::string> uniforms = {
                "u_model_matrix",
                "u_light_space_matrix",
                "u_view_position",
                "u_shadow_map",
                "u_material.diffuse",
                "u_material.specular",
                "u_material.shininess",
                "u_light.position",
                "u_light.ambient",
                "u_light.diffuse",
                "u_light.specular"
            };
            storage->board_shader = Shader::create(
                assets::path(assets::BOARD_VERTEX_SHADER),
                assets::path(assets::BOARD_FRAGMENT_SHADER),
                uniforms,
                block_name, 1,
                storage->uniform_buffer
            );
        }

        {
            const std::vector<std::string> uniforms = {
                "u_model_matrix",
                "u_light_space_matrix",
                "u_view_position",
                "u_shadow_map",
                "u_material.diffuse",
                "u_material.specular",
                "u_material.shininess",
                "u_light.position",
                "u_light.ambient",
                "u_light.diffuse",
                "u_light.specular"
            };
            storage->board_paint_shader = Shader::create(
                assets::path(assets::BOARD_PAINT_VERTEX_SHADER),
                assets::path(assets::BOARD_PAINT_FRAGMENT_SHADER),
                uniforms,
                block_name, 1,
                storage->uniform_buffer
            );
        }

        {
            const std::vector<std::string> uniforms = {
                "u_model_matrix",
                "u_light_space_matrix",
                "u_view_position",
                "u_tint_color",
                "u_shadow_map",
                "u_material.diffuse",
                "u_material.specular",
                "u_material.shininess",
                "u_light.position",
                "u_light.ambient",
                "u_light.diffuse",
                "u_light.specular"
            };
            storage->piece_shader = Shader::create(
                assets::path(assets::PIECE_VERTEX_SHADER),
                assets::path(assets::PIECE_FRAGMENT_SHADER),
                uniforms,
                block_name, 1,
                storage->uniform_buffer
            );
        }

        {
            const std::vector<std::string> uniforms = {
                "u_model_matrix",
                "u_light_space_matrix"
            };
            storage->shadow_shader = Shader::create(
                assets::path(assets::SHADOW_VERTEX_SHADER),
                assets::path(assets::SHADOW_FRAGMENT_SHADER),
                uniforms
            );
        }

        {
            const std::vector<std::string> uniforms = {
                "u_screen_texture"
            };
            storage->screen_quad_shader = Shader::create(
                assets::path(assets::SCREEN_QUAD_VERTEX_SHADER),
                assets::path(assets::SCREEN_QUAD_FRAGMENT_SHADER),
                uniforms
            );
        }

        {
            const std::vector<std::string> uniforms = {
                "u_model_matrix",
                "u_color"
            };
            storage->outline_shader = Shader::create(
                assets::path(assets::OUTLINE_VERTEX_SHADER),
                assets::path(assets::OUTLINE_FRAGMENT_SHADER),
                uniforms,
                block_name, 1,
                storage->uniform_buffer
            );
        }

        {
            const std::vector<std::string> uniforms = {
                "u_model_matrix",
                "u_color"
            };
            storage->node_shader = Shader::create(
                assets::path(assets::NODE_VERTEX_SHADER),
                assets::path(assets::NODE_FRAGMENT_SHADER),
                uniforms,
                block_name, 1,
                storage->uniform_buffer
            );
        }

        {
            const std::vector<std::string> uniforms = {
                "u_projection_view_matrix",
                "u_skybox"
            };
            storage->skybox_shader = Shader::create(
                assets::path(assets::SKYBOX_VERTEX_SHADER),
                assets::path(assets::SKYBOX_FRAGMENT_SHADER),
                uniforms
            );
        }

        {
            const std::vector<std::string> uniforms = {
                "u_model_matrix",
                "u_projection_matrix",
                "u_texture"
            };
            storage->quad2d_shader = Shader::create(
                assets::path(assets::QUAD2D_VERTEX_SHADER),
                assets::path(assets::QUAD2D_FRAGMENT_SHADER),
                uniforms
            );
        }

        {
            const std::vector<std::string> uniforms = {
                "u_model_matrix",
                "u_view_matrix",
                "u_projection_matrix",
                "u_texture"
            };
            storage->quad3d_shader = Shader::create(
                assets::path(assets::QUAD3D_VERTEX_SHADER),
                assets::path(assets::QUAD3D_FRAGMENT_SHADER),
                uniforms
            );
        }

#ifndef NDEBUG
        {
            const std::vector<std::string> uniforms;
            storage->origin_shader = Shader::create(
                assets::path(assets::ORIGIN_VERTEX_SHADER),
                assets::path(assets::ORIGIN_FRAGMENT_SHADER),
                uniforms,
                block_name, 1,
                storage->uniform_buffer
            );
        }
#endif

        {
            const std::vector<std::string> uniforms = {
                "u_model_matrix",
                "u_projection_matrix",
                "u_bitmap",
                "u_color",
                "u_border_width",
                "u_offset"
            };
            storage->text_shader = Shader::create(
                assets::path(assets::TEXT_VERTEX_SHADER),
                assets::path(assets::TEXT_FRAGMENT_SHADER),
                uniforms
            );
        }

        {
            FramebufferSpecification specification;
            specification.width = 2048;
            specification.height = 2048;
            specification.depth_attachment = Attachment(AttachmentFormat::DEPTH32,
                    AttachmentType::Texture);
            specification.enable_depth_attachment = true;
            specification.resizable = false;

            storage->depth_map_framebuffer = Framebuffer::create(specification);

            app->purge_framebuffers();
            app->add_framebuffer(storage->depth_map_framebuffer);
        }
        
        {
            FramebufferSpecification specification;
            specification.width = app->data.width;
            specification.height = app->data.height;
            specification.color_attachments = {
                Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture),
                Attachment(AttachmentFormat::RED_I, AttachmentType::Texture)
            };
            specification.depth_attachment = Attachment(AttachmentFormat::DEPTH24_STENCIL8,
                    AttachmentType::Renderbuffer);
            specification.enable_depth_attachment = true;

            storage->intermediate_framebuffer = Framebuffer::create(specification);

            app->purge_framebuffers();
            app->add_framebuffer(storage->intermediate_framebuffer);
        }

        {
            float screen_quad_vertices[] = {
                -1.0f,  1.0f,    0.0f, 1.0f,
                -1.0f, -1.0f,    0.0f, 0.0f,
                 1.0f,  1.0f,    1.0f, 1.0f,
                 1.0f,  1.0f,    1.0f, 1.0f,
                -1.0f, -1.0f,    0.0f, 0.0f,
                 1.0f, -1.0f,    1.0f, 0.0f
            };

            std::shared_ptr<Buffer> buffer = Buffer::create(screen_quad_vertices, sizeof(screen_quad_vertices));
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

            std::shared_ptr<Buffer> buffer = Buffer::create(quad2d_vertices, sizeof(quad2d_vertices));
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
            std::shared_ptr<Buffer> buffer = Buffer::create(origin_vertices, sizeof(origin_vertices));
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

        storage->orthographic_projection_matrix = glm::ortho(0.0f, static_cast<float>(app->data.width),
                0.0f, static_cast<float>(app->data.height));

        storage->good_dog_plain_font = std::make_shared<Font>(assets::path(assets::GOOD_DOG_PLAIN_FONT),
                50.0f, 5, 180, 40, 512);

        storage->good_dog_plain_font->begin_baking();  // TODO maybe move part of texture baking to thread
        storage->good_dog_plain_font->bake_characters(32, 127);
        storage->good_dog_plain_font->end_baking();

        // Setup uniforms for these shaders for rendering
        storage->screen_quad_shader->bind();
        storage->screen_quad_shader->set_uniform_int("u_screen_texture", 0);
        storage->quad2d_shader->bind();
        storage->quad2d_shader->set_uniform_int("u_texture", 0);
        storage->quad2d_shader->set_uniform_matrix("u_projection_matrix",
                storage->orthographic_projection_matrix);
        storage->quad3d_shader->bind();
        storage->quad3d_shader->set_uniform_int("u_texture", 0);
        storage->text_shader->bind();
        storage->text_shader->set_uniform_int("u_bitmap", 0);
        storage->text_shader->set_uniform_matrix("u_projection_matrix",
                storage->orthographic_projection_matrix);
        storage->skybox_shader->bind();
        storage->skybox_shader->set_uniform_int("u_skybox", 0);
        Shader::unbind();

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

    void draw_quad_2d(const glm::vec2& position, float additional_scale, std::shared_ptr<Texture> texture) {
        glDisable(GL_DEPTH_TEST);

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(position, 0.0f));
        matrix = glm::scale(matrix, glm::vec3(texture->get_width(), texture->get_height(), 1.0f));
        matrix = glm::scale(matrix, glm::vec3(additional_scale, additional_scale, 1.0f));

        storage->quad2d_shader->bind();
        storage->quad2d_shader->set_uniform_matrix("u_model_matrix", matrix);

        texture->bind(0);

        storage->quad2d_vertex_array->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_DEPTH_TEST);
    }

    void draw_quad_2d(const glm::vec2& position, const glm::vec2& scale, std::shared_ptr<Texture> texture) {
        glDisable(GL_DEPTH_TEST);

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(position, 0.0f));
        matrix = glm::scale(matrix, glm::vec3(scale.x, scale.y, 1.0f));

        storage->quad2d_shader->bind();
        storage->quad2d_shader->set_uniform_matrix("u_model_matrix", matrix);

        texture->bind(0);
        storage->quad2d_vertex_array->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_DEPTH_TEST);
    }

    void draw_quad_3d(const glm::vec3& position, float scale, std::shared_ptr<Texture> texture) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));

        storage->quad3d_shader->bind();
        storage->quad3d_shader->set_uniform_matrix("u_model_matrix", matrix);

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
        size_t size;
        float* buffer;
        font->render(string, &size, &buffer);

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(position, 0.0f));
        matrix = glm::scale(matrix, glm::vec3(scale, scale, 1.0f));

        font->update_data(buffer, size);
        delete[] buffer;

        storage->text_shader->bind();
        storage->text_shader->set_uniform_matrix("u_model_matrix", matrix);
        storage->text_shader->set_uniform_vec3("u_color", color);
        storage->text_shader->set_uniform_float("u_border_width", 0.0f);
        storage->text_shader->set_uniform_vec2("u_offset", glm::vec2(0.0f, 0.0f));

        glDisable(GL_DEPTH_TEST);

        font->get_vertex_array()->bind();
        bind_texture(font->get_texture(), 0);
        glDrawArrays(GL_TRIANGLES, 0, font->get_vertex_count());

        glEnable(GL_DEPTH_TEST);
    }

    void draw_string_with_shadows(const std::string& string, const glm::vec2& position, float scale,
                const glm::vec3& color, std::shared_ptr<Font> font) {
        size_t size;
        float* buffer;
        font->render(string, &size, &buffer);

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(position, 0.0f));
        matrix = glm::scale(matrix, glm::vec3(scale, scale, 1.0f));

        font->update_data(buffer, size);
        delete[] buffer;

        storage->text_shader->bind();
        storage->text_shader->set_uniform_matrix("u_model_matrix", matrix);
        storage->text_shader->set_uniform_vec3("u_color", color);
        storage->text_shader->set_uniform_float("u_border_width", 0.3f);
        storage->text_shader->set_uniform_vec2("u_offset", glm::vec2(-0.003f, -0.003f));

        glDisable(GL_DEPTH_TEST);

        font->get_vertex_array()->bind();
        bind_texture(font->get_texture(), 0);
        glDrawArrays(GL_TRIANGLES, 0, font->get_vertex_count());

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
            std::shared_ptr<VertexArray> vertex_array, int index_count) {
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
