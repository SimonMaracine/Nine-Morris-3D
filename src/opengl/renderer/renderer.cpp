#include <memory>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "opengl/renderer/renderer.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/texture.h"

namespace renderer {
    static Storage storage;

    const Storage* init() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        storage.quad_shader = Shader::create("data/shaders/quad.vert",
                                             "data/shaders/quad.frag");
        constexpr float quad_vertices[] = {
            -1.0f,  1.0f,    0.0f, 1.0f,
            -1.0f, -1.0f,    0.0f, 0.0f,
            1.0f,  1.0f,    1.0f, 1.0f,
            1.0f,  1.0f,    1.0f, 1.0f,
            -1.0f, -1.0f,    0.0f, 0.0f,
            1.0f, -1.0f,    1.0f, 0.0f
        };
        storage.quad_vertex_buffer = VertexBuffer::create_with_data(quad_vertices,
                                                                    sizeof(quad_vertices));
        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 2);
        layout.add(1, BufferLayout::Type::Float, 2);
        storage.quad_vertex_array = VertexArray::create();
        storage.quad_vertex_array->add_buffer(storage.quad_vertex_buffer, layout);

        VertexArray::unbind();

        storage.outline_shader = Shader::create("data/shaders/outline.vert",
                                                "data/shaders/outline.frag");

        return &storage;
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

    void end() {

    }

    void draw_quad() {
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

    void draw_model(const glm::vec3& position, const glm::vec3& rotation,
                    float scale, std::shared_ptr<Shader> shader,
                    std::shared_ptr<VertexArray> array,
                    std::shared_ptr<Texture> diffuse_map, GLuint index_count) {
        shader->bind();
        shader->set_uniform_int("u_diffuse", 0);

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

    void draw_model_outline(const glm::vec3& position, const glm::vec3& rotation,
                            float scale, std::shared_ptr<Shader> shader,
                            std::shared_ptr<VertexArray> array,
                            std::shared_ptr<Texture> diffuse_map, GLuint index_count,
                            const glm::vec3& outline_color) {
        glStencilFunc(GL_ALWAYS, 1, 0xFF); 
        glStencilMask(0xFF);

        {
            shader->bind();
            shader->set_uniform_int("u_diffuse", 0);

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

        storage.outline_shader->bind();
        storage.outline_shader->set_uniform_vec3("u_color", outline_color);

        {
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, position);
            matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            matrix = glm::scale(matrix, glm::vec3(scale + 0.04f, scale + 0.04f, scale + 0.04f));

            shader->set_uniform_matrix("u_model_matrix", matrix);

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
}
