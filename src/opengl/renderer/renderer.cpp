#include <memory>
#include <iostream>
#include <string.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengl/renderer/renderer.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/buffer.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/texture.h"
#include "opengl/renderer/framebuffer.h"

namespace renderer {
    static Storage* storage = new Storage;

    Storage* init(int width, int height) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        storage->uniform_buffer = Buffer::create_uniform(nullptr, sizeof(glm::mat4));
        const char* block_name = "Matrices";
        const char* uniforms[] = {
            "u_projection_view_matrix"
        };

        storage->board_shader = Shader::create("data/shaders/board.vert",
                                               "data/shaders/board.frag",
                                               block_name,
                                               uniforms, 1,
                                               storage->uniform_buffer);

        storage->piece_shader = Shader::create("data/shaders/piece.vert",
                                               "data/shaders/piece.frag",
                                               block_name,
                                               uniforms, 1,
                                               storage->uniform_buffer);

        storage->shadow_shader = Shader::create("data/shaders/shadow.vert",
                                                "data/shaders/shadow.frag");

        storage->scene_framebuffer = Framebuffer::create(Framebuffer::Type::Scene, width, height, DEFAULT_MSAA, 2);
        storage->depth_map_framebuffer = Framebuffer::create(Framebuffer::Type::DepthMap, 2048, 2048, 1, 0);
        storage->intermediate_framebuffer = Framebuffer::create(Framebuffer::Type::Intermediate, width, height, 1, 2);

        {
            storage->quad_shader = Shader::create("data/shaders/quad.vert",
                                                  "data/shaders/quad.frag");
            constexpr float quad_vertices[] = {
                -1.0f,  1.0f,    0.0f, 1.0f,
                -1.0f, -1.0f,    0.0f, 0.0f,
                 1.0f,  1.0f,    1.0f, 1.0f,
                 1.0f,  1.0f,    1.0f, 1.0f,
                -1.0f, -1.0f,    0.0f, 0.0f,
                 1.0f, -1.0f,    1.0f, 0.0f
            };
            storage->quad_vertex_buffer = Buffer::create(quad_vertices, sizeof(quad_vertices));
            BufferLayout layout;
            layout.add(0, BufferLayout::Type::Float, 2);
            layout.add(1, BufferLayout::Type::Float, 2);
            storage->quad_vertex_array = VertexArray::create();
            storage->quad_vertex_array->add_buffer(storage->quad_vertex_buffer, layout);

            VertexArray::unbind();
        }

        storage->outline_shader = Shader::create("data/shaders/outline.vert",
                                                 "data/shaders/outline.frag",
                                                 block_name,
                                                 uniforms, 1,
                                                 storage->uniform_buffer);

        storage->node_shader = Shader::create("data/shaders/node.vert",
                                              "data/shaders/node.frag",
                                              block_name,
                                              uniforms, 1,
                                              storage->uniform_buffer);

        storage->skybox_shader = Shader::create("data/shaders/cubemap.vert",
                                                "data/shaders/cubemap.frag");

        {
            storage->origin_shader = Shader::create("data/shaders/origin.vert",
                                                    "data/shaders/origin.frag");
            constexpr float origin_vertices[] = {
                -20.0f,   0.0f,   0.0f,    1.0f, 0.0f, 0.0f,
                 20.0f,   0.0f,   0.0f,    1.0f, 0.0f, 0.0f,
                  0.0f, -20.0f,   0.0f,    0.0f, 1.0f, 0.0f,
                  0.0f,  20.0f,   0.0f,    0.0f, 1.0f, 0.0f,
                  0.0f,   0.0f, -20.0f,    0.0f, 0.0f, 1.0f,
                  0.0f,   0.0f,  20.0f,    0.0f, 0.0f, 1.0f
            };
            storage->origin_vertex_buffer = Buffer::create(origin_vertices, sizeof(origin_vertices));
            BufferLayout layout;
            layout.add(0, BufferLayout::Type::Float, 3);
            layout.add(1, BufferLayout::Type::Float, 3);
            storage->origin_vertex_array = VertexArray::create();
            storage->origin_vertex_array->add_buffer(storage->origin_vertex_buffer, layout);
        }

        {
            storage->light_shader = Shader::create("data/shaders/light.vert",
                                                   "data/shaders/light.frag");
            constexpr float light_vertices[] = {
                -1.0f,  1.0f,    0.0f, 1.0f,
                -1.0f, -1.0f,    0.0f, 0.0f,
                 1.0f,  1.0f,    1.0f, 1.0f,
                 1.0f,  1.0f,    1.0f, 1.0f,
                -1.0f, -1.0f,    0.0f, 0.0f,
                 1.0f, -1.0f,    1.0f, 0.0f
            };
            storage->light_vertex_buffer = Buffer::create(light_vertices, sizeof(light_vertices));
            BufferLayout layout;
            layout.add(0, BufferLayout::Type::Float, 2);
            layout.add(1, BufferLayout::Type::Float, 2);
            storage->light_vertex_array = VertexArray::create();
            storage->light_vertex_array->add_buffer(storage->light_vertex_buffer, layout);
            storage->light_texture = Texture::create("data/textures/light.png", false);
        }

        {
            storage->loading_shader = Shader::create("data/shaders/loading.vert",
                                                     "data/shaders/loading.frag");
            constexpr float loading_vertices[] = {
                -1.0f,  1.0f,    0.0f, 1.0f,
                -1.0f, -1.0f,    0.0f, 0.0f,
                 1.0f,  1.0f,    1.0f, 1.0f,
                 1.0f,  1.0f,    1.0f, 1.0f,
                -1.0f, -1.0f,    0.0f, 0.0f,
                 1.0f, -1.0f,    1.0f, 0.0f
            };
            storage->loading_vertex_buffer = Buffer::create(loading_vertices, sizeof(loading_vertices));
            BufferLayout layout;
            layout.add(0, BufferLayout::Type::Float, 2);
            layout.add(1, BufferLayout::Type::Float, 2);
            storage->loading_vertex_array = VertexArray::create();
            storage->loading_vertex_array->add_buffer(storage->loading_vertex_buffer, layout);
            storage->loading_texture = Texture::create("data/textures/loading.png", false);
        }

        return storage;
    }

    void terminate() {
        delete storage;
    }

    void set_viewport(GLint width, GLint height) {
        glViewport(0, 0, width, height);
    }

    void set_clear_color(GLfloat red, GLfloat green, GLfloat blue) {
        glClearColor(red, green, blue, 1.0f);
    }

    void clear(int buffers) {
        glClear(buffers);
    }

    void draw_quad() {
        glDisable(GL_DEPTH_TEST);

        storage->quad_vertex_array->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_DEPTH_TEST);
    }

    void draw_loading() {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);

        storage->loading_shader->bind();
        storage->loading_shader->set_uniform_int("u_texture", 0);
        storage->loading_vertex_array->bind();
        storage->loading_texture->bind(0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_STENCIL_TEST);
        glEnable(GL_DEPTH_TEST);
    }

    void draw_origin() {
        storage->origin_vertex_array->bind();
        glDrawArrays(GL_LINES, 0, 6);
    }

    void draw_light(const glm::vec3& position) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::scale(matrix, glm::vec3(0.3f, 0.3f, 0.3f));

        storage->light_shader->bind();
        storage->light_shader->set_uniform_matrix("u_model_matrix", matrix);
        storage->light_shader->set_uniform_int("u_texture", 0);

        storage->light_vertex_array->bind();
        storage->light_texture->bind(0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void bind_texture(GLuint texture, GLenum slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void set_stencil_mask_zero() {
        glStencilMask(0x00);
    }

    void load_projection_view(const glm::mat4& matrix) {
        constexpr std::size_t size = sizeof(glm::mat4);
        float* buffer[size];
        memcpy(buffer, glm::value_ptr(matrix), size);
        storage->uniform_buffer->update_data(buffer, size);
    }

    void draw_board(const glm::vec3& position,
                    const glm::vec3& rotation,
                    float scale,
                    Rc<Shader> shader,
                    Rc<VertexArray> array,
                    Rc<Texture> diffuse_map,
                    const glm::vec3& specular_color,
                    float shininess,
                    GLuint index_count) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));

        shader->bind();
        shader->set_uniform_int("u_material.diffuse", 0);
        shader->set_uniform_vec3("u_material.specular", specular_color);
        shader->set_uniform_float("u_material.shininess", shininess);

        shader->set_uniform_matrix("u_model_matrix", matrix);

        array->bind();
        diffuse_map->bind(0);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    }

    void draw_piece(const glm::vec3& position,
                    const glm::vec3& rotation,
                    float scale,
                    Rc<Shader> shader,
                    Rc<VertexArray> array,
                    Rc<Texture> diffuse_map,
                    const glm::vec3& specular_color,
                    float shininess,
                    GLuint index_count,
                    const glm::vec3& tint_color) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));

        shader->bind();
        shader->set_uniform_int("u_material.diffuse", 0);
        shader->set_uniform_vec3("u_material.specular", specular_color);
        shader->set_uniform_float("u_material.shininess", shininess);
        shader->set_uniform_vec3("u_tint_color", tint_color);

        shader->set_uniform_matrix("u_model_matrix", matrix);

        array->bind();
        diffuse_map->bind(0);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    }

    void draw_piece_outline(const glm::vec3& position,
                            const glm::vec3& rotation,
                            float scale,
                            Rc<Shader> shader,
                            Rc<VertexArray> array,
                            Rc<Texture> diffuse_map,
                            const glm::vec3& specular_color,
                            float shininess,
                            GLuint index_count,
                            const glm::vec3& outline_color) {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        {
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, position);
            matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));

            shader->bind();
            shader->set_uniform_int("u_material.diffuse", 0);
            shader->set_uniform_vec3("u_material.specular", specular_color);
            shader->set_uniform_float("u_material.shininess", shininess);

            shader->set_uniform_matrix("u_model_matrix", matrix);

            array->bind();
            diffuse_map->bind(0);
            glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
        }

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        {
            constexpr float size = 3.6f;

            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, position);
            matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            matrix = glm::scale(matrix, glm::vec3(scale + size, scale + size, scale + size));

            storage->outline_shader->bind();
            storage->outline_shader->set_uniform_vec3("u_color", outline_color);
            storage->outline_shader->set_uniform_matrix("u_model_matrix", matrix);

            glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
        }

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);
    }

    void draw_cube_map(const glm::mat4& view_projection_matrix,
                       Rc<Shader> shader, Rc<VertexArray> array,
                       Rc<Texture3D> texture) {
        glDepthMask(GL_FALSE);

        shader->bind();
        shader->set_uniform_int("u_skybox", 0);
        shader->set_uniform_matrix("u_projection_view_matrix", view_projection_matrix);

        array->bind();
        texture->bind(0);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthMask(GL_TRUE);
    }

    void draw_node(const glm::vec3& position,
                   float scale,
                   Rc<Shader> shader,
                   Rc<VertexArray> array,
                   const glm::vec4& color,
                   GLuint index_count) {
        glCullFace(GL_FRONT);

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));

        shader->bind();
        shader->set_uniform_vec4("u_color", color);
        shader->set_uniform_matrix("u_model_matrix", matrix);

        array->bind();
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);

        glCullFace(GL_BACK);
    }

    void draw_to_depth(const glm::vec3& position,
                       const glm::vec3& rotation,
                       float scale,
                       Rc<Shader> shader,
                       Rc<VertexArray> array,
                       GLuint index_count) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));

        shader->bind();
        shader->set_uniform_matrix("u_model_matrix", matrix);

        array->bind();
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    }
}
