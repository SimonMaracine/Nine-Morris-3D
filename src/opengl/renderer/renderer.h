#pragma once

#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/texture.h"

namespace renderer {
    void init();
    void set_viewport(GLint width, GLint height);
    void set_clear_color(GLfloat red, GLfloat green, GLfloat blue);
    void clear();
    void begin(std::shared_ptr<Shader> shader, const glm::mat4& view_projection_matrix);
    void end();

    void draw_model(const glm::vec3& position, const glm::vec3& rotation,
                    float scale, std::shared_ptr<Shader> shader,
                    std::shared_ptr<VertexArray> array,
                    std::shared_ptr<Texture> diffuse_map, GLuint index_count);
    void draw_cube_map(const glm::mat4& view_projection_matrix,
                       std::shared_ptr<Shader> shader, std::shared_ptr<VertexArray> array,
                       std::shared_ptr<Texture3D> texture);
}
