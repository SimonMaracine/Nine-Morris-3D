#include <memory>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "opengl/renderer/renderer.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/texture.h"
#include "opengl/renderer/framebuffer.h"

namespace renderer {
    static Storage* storage = new Storage;

    const Storage* init() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        storage->basic_shader = Shader::create("data/shaders/basic.vert",
                                               "data/shaders/basic.frag");

        Specification specification;
        specification.width = 1024;
        specification.height = 576;
        specification.attachments = { TextureFormat::RGBA8, TextureFormat::RedInteger,
                                      TextureFormat::Depth24Stencil8 };
        storage->framebuffer = Framebuffer::create(specification);

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
            storage->quad_vertex_buffer = VertexBuffer::create_with_data(quad_vertices,
                                                                         sizeof(quad_vertices));
            BufferLayout layout;
            layout.add(0, BufferLayout::Type::Float, 2);
            layout.add(1, BufferLayout::Type::Float, 2);
            storage->quad_vertex_array = VertexArray::create();
            storage->quad_vertex_array->add_buffer(storage->quad_vertex_buffer, layout);

            VertexArray::unbind();
        }

        storage->outline_shader = Shader::create("data/shaders/outline.vert",
                                                 "data/shaders/outline.frag");

        storage->node_shader = Shader::create("data/shaders/node.vert",
                                              "data/shaders/node.frag");

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
            storage->origin_vertex_buffer = VertexBuffer::create_with_data(origin_vertices,
                                                                           sizeof(origin_vertices));
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
            storage->light_vertex_buffer = VertexBuffer::create_with_data(light_vertices,
                                                                          sizeof(light_vertices));
            BufferLayout layout;
            layout.add(0, BufferLayout::Type::Float, 2);
            layout.add(1, BufferLayout::Type::Float, 2);
            storage->light_vertex_array = VertexArray::create();
            storage->light_vertex_array->add_buffer(storage->light_vertex_buffer, layout);
            storage->light_texture = Texture::create("data/textures/light.png");
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

    // void begin(std::shared_ptr<Shader> shader, const glm::mat4& view_projection_matrix) {
    //     shader->bind();
    //     shader->set_uniform_matrix("u_projection_view_matrix", view_projection_matrix);
    // }

    // void end() {

    // }

    void draw_quad() {
        glDrawArrays(GL_TRIANGLES, 0, 6);
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

    void enable_depth() {
        glEnable(GL_DEPTH_TEST);
    }

    void disable_depth() {
        glDisable(GL_DEPTH_TEST);
    }

    void bind_texture(GLuint texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void set_stencil_mask_zero() {
        glStencilMask(0x00);
    }

    void draw_model(const glm::vec3& position,
                    const glm::vec3& rotation,
                    float scale,
                    std::shared_ptr<Shader> shader,
                    std::shared_ptr<VertexArray> array,
                    std::shared_ptr<Texture> diffuse_map,
                    const glm::vec3& specular_color,
                    float shininess,
                    GLuint index_count) {
        shader->bind();
        shader->set_uniform_int("u_material.diffuse", 0);
        shader->set_uniform_vec3("u_material.specular", specular_color);
        shader->set_uniform_float("u_material.shininess", shininess);

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));

        shader->set_uniform_matrix("u_model_matrix", matrix);

        array->bind();
        diffuse_map->bind(0);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    }

    void draw_model_outline(const glm::vec3& position,
                            const glm::vec3& rotation,
                            float scale,
                            std::shared_ptr<Shader> shader,
                            std::shared_ptr<VertexArray> array,
                            std::shared_ptr<Texture> diffuse_map,
                            const glm::vec3& specular_color,
                            float shininess,
                            GLuint index_count,
                            const glm::vec3& outline_color,
                            float outline_size) {
        glStencilFunc(GL_ALWAYS, 1, 0xFF); 
        glStencilMask(0xFF);

        {
            shader->bind();
            shader->set_uniform_int("u_material.diffuse", 0);
            shader->set_uniform_vec3("u_material.specular", specular_color);
            shader->set_uniform_float("u_material.shininess", shininess);

            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, position);
            matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));

            shader->set_uniform_matrix("u_model_matrix", matrix);

            array->bind();
            diffuse_map->bind(0);
            glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
        }

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        {
            storage->outline_shader->bind();
            storage->outline_shader->set_uniform_vec3("u_color", outline_color);

            const float size = outline_size;

            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, position);
            matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            matrix = glm::scale(matrix, glm::vec3(scale + size, scale + size, scale + size));
    
            storage->outline_shader->set_uniform_matrix("u_model_matrix", matrix);

            glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
        }

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
    }

    void draw_cube_map(const glm::mat4& view_projection_matrix,
                       std::shared_ptr<Shader> shader, std::shared_ptr<VertexArray> array,
                       std::shared_ptr<Texture3D> texture) {
        glDepthMask(GL_FALSE);
        glDisable(GL_STENCIL_TEST);

        shader->bind();
        shader->set_uniform_int("u_skybox", 0);
        shader->set_uniform_matrix("u_projection_view_matrix", view_projection_matrix);

        array->bind();
        texture->bind(0);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthMask(GL_TRUE);
        glEnable(GL_STENCIL_TEST);
    }

    void draw_node(const glm::vec3& position,
                   float scale,
                   std::shared_ptr<Shader> shader,
                   std::shared_ptr<VertexArray> array,
                   const glm::vec4& color,
                   GLuint index_count) {
        glCullFace(GL_FRONT);
    
        shader->bind();
        shader->set_uniform_vec4("u_color", color);

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));

        shader->set_uniform_matrix("u_model_matrix", matrix);

        array->bind();
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);

        glCullFace(GL_BACK);
    }
}
