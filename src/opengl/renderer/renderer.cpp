#include <memory>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/texture.h"

namespace renderer {
    void init() {
        glEnable(GL_DEPTH_TEST);
    }

    void set_viewport(GLint width, GLint height) {
        glViewport(0, 0, width, height);
    }

    void set_clear_color(GLfloat red, GLfloat green, GLfloat blue) {
        glClearColor(red, green, blue, 1.0f);
    }

    void clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void begin(std::shared_ptr<Shader> shader, const glm::mat4& view_projection_matrix) {
        shader->bind();
        shader->set_uniform_matrix("u_projection_view_matrix", view_projection_matrix);
    }

    void end() {

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

    void draw_cube_map(const glm::mat4& view_projection_matrix,
                       std::shared_ptr<Shader> shader, std::shared_ptr<VertexArray> array,
                       std::shared_ptr<Texture3D> texture) {
        glDepthMask(GL_FALSE);

        shader->bind();
        shader->set_uniform_int("u_skybox", 0);
        shader->set_uniform_matrix("u_projection_view_matrix", view_projection_matrix);

        array->bind();
        texture->bind(0);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthMask(GL_TRUE);
    }
}
