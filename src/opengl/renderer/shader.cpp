#include <string>
#include <fstream>
#include <iostream>
#include <memory>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "opengl/renderer/shader.h"
#include "other/logging.h"

Shader::Shader(GLuint program, GLuint vertex_shader, GLuint fragment_shader)
        : program(program), vertex_shader(vertex_shader), fragment_shader(fragment_shader) {
    SPDLOG_DEBUG("Created shader {}", program);
}

Shader::~Shader() {
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(program);

    SPDLOG_DEBUG("Deleted shader {}", program);
}

std::shared_ptr<Shader> Shader::create(const std::string& vertex_source_path,
                                       const std::string& fragment_source_path) {
    GLuint vertex_shader = compile_shader(vertex_source_path, GL_VERTEX_SHADER);
    GLuint fragment_shader = compile_shader(fragment_source_path, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glValidateProgram(program);

    check_linking(program);

    return std::make_shared<Shader>(program, vertex_shader, fragment_shader);
}

void Shader::bind() const {
    glUseProgram(program);
}

void Shader::unbind() {
    glUseProgram(0);
}

void Shader::set_uniform_matrix(const std::string& name, const glm::mat4& matrix) const {
    GLint location = get_uniform_location(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::set_uniform_int(const std::string& name, int value) const {
    GLint location = get_uniform_location(name);
    glUniform1i(location, value);
}

GLint Shader::get_uniform_location(const std::string& name) const {
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        SPDLOG_ERROR("Uniform variable '{}' not found", name.c_str());
    }

    return location;
}    

GLuint Shader::compile_shader(const std::string& source_path, GLenum type) {
    std::ifstream file = std::ifstream(source_path.c_str(), std::ifstream::in);
    std::string source;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            source.append(line).append("\n");
        }
    } else {
        spdlog::critical("Could not open file '{}'", source_path.c_str());
        std::exit(1);
    }
    file.close();

    GLuint shader = glCreateShader(type);
    const char* s = source.c_str();
    glShaderSource(shader, 1, &s, nullptr);
    glCompileShader(shader);

    check_compilation(shader, type);

    return shader;
}

void Shader::check_compilation(GLuint shader, GLenum type) {
    GLint compile_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

    if (compile_status == GL_FALSE) {
        GLsizei log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        const char* t;
        switch (type) {
            case GL_VERTEX_SHADER: t = "Vertex"; break;
            case GL_FRAGMENT_SHADER: t = "Fragment"; break;
        }

        if (log_length == 0) {
            spdlog::critical("{} shader compilation error with no message", t);
        } else {
            char* log_message = new char[log_length];
            glGetShaderInfoLog(shader, log_length, nullptr, log_message);

            spdlog::critical("{} shader compilation error\n{}", t, log_message);
            delete[] log_message;
        }

        std::exit(1);
    }
}

void Shader::check_linking(GLuint program) {
    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);

    if (link_status == GL_FALSE) {
        GLsizei log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

        if (log_length == 0) {
            spdlog::critical("Shader linking error with no message");
        } else {
            char* log_message = new char[log_length];
            glGetProgramInfoLog(program, log_length, nullptr, log_message);

            spdlog::critical("Shader linking error\n{}", log_message);
            delete[] log_message;
        }

        std::exit(1);
    }
}
