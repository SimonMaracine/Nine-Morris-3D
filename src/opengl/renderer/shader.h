#pragma once

#include <memory>
#include <string>

#include <glad/glad.h>

class Shader {
public:
    Shader(GLuint program, GLuint vertex_shader, GLuint fragment_shader);
    ~Shader();

    static std::shared_ptr<Shader> create(const std::string& vertex_source,
                                          const std::string& fragment_source);

    void bind();
    static void unbind();
private:
    GLint get_uniform_location(const std::string& name);

    static GLuint compile_shader(const std::string& source_path, GLenum type);
    static void check_compilation(GLuint shader, GLenum type);
    static void check_linking(GLuint program);

    GLuint program;
    GLuint vertex_shader;
    GLuint fragment_shader;
};
