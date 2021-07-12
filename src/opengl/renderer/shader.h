#pragma once

#include <memory>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(GLuint program, GLuint vertex_shader, GLuint fragment_shader);
    ~Shader();

    static std::shared_ptr<Shader> create(const std::string& vertex_source,
                                          const std::string& fragment_source);

    void bind() const;
    static void unbind();

    void set_uniform_matrix(const std::string& name, const glm::mat4& matrix) const;
    void set_uniform_int(const std::string& name, int value) const;
private:
    GLint get_uniform_location(const std::string& name) const;

    static GLuint compile_shader(const std::string& source_path, GLenum type);
    static void check_compilation(GLuint shader, GLenum type);
    static void check_linking(GLuint program);

    GLuint program;
    GLuint vertex_shader;
    GLuint fragment_shader;
};
