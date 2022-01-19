#pragma once

#include <memory>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "graphics/renderer/buffer.h"

class Shader {
public:
    Shader(GLuint program, GLuint vertex_shader, GLuint fragment_shader, const std::string& name);
    ~Shader();

    static std::shared_ptr<Shader> create(const std::string& vertex_source,
                                          const std::string& fragment_source);
    static std::shared_ptr<Shader> create(const std::string& vertex_source,
                                          const std::string& fragment_source,
                                          const char* block_name,
                                          const char** uniforms,
                                          int uniforms_count,
                                          std::shared_ptr<UniformBuffer> uniform_buffer);

    void bind() const;
    static void unbind();

    void set_uniform_matrix(const std::string& name, const glm::mat4& matrix) const;
    void set_uniform_int(const std::string& name, int value) const;
    void set_uniform_vec3(const std::string& name, const glm::vec3& vector) const;
    void set_uniform_vec4(const std::string& name, const glm::vec4& vector) const;
    void set_uniform_float(const std::string& name, float value) const;
private:
    GLint get_uniform_location(const std::string& name) const;

    static GLuint compile_shader(const std::string& source_path, GLenum type);
    static void check_compilation(GLuint shader, GLenum type);
    static void check_linking(GLuint program);
    static std::string get_name(const std::string& vertex_source, const std::string& fragment_source);

    GLuint program;
    GLuint vertex_shader;
    GLuint fragment_shader;

    std::string name;
};

template<typename T>
using Rc = std::shared_ptr<T>;
