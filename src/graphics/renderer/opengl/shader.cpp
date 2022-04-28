#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <stdexcept>
#include <cassert>
#include <string.h>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "application/platform.h"
#include "graphics/debug_opengl.h"
#include "graphics/renderer/opengl/shader.h"
#include "graphics/renderer/opengl/buffer.h"
#include "other/logging.h"

#define DELETE_SHADER(program, vertex_shader, fragment_shader) \
    glDetachShader(program, vertex_shader); \
    glDetachShader(program, fragment_shader); \
    glDeleteShader(vertex_shader); \
    glDeleteShader(fragment_shader); \
    glDeleteProgram(program);

#define CASE(_enum, count, type) case _enum: size = count * sizeof(type); break;

static size_t type_size(GLenum type) {
    size_t size;

    switch (type) {
        CASE(GL_FLOAT_VEC3, 3, GLfloat)
        CASE(GL_FLOAT_MAT4, 16, GLfloat)
        default:
            REL_CRITICAL("Unknown type: {}, exiting...", type);
            exit(1);
    }

    return size;
}

Shader::Shader(GLuint program, GLuint vertex_shader, GLuint fragment_shader, std::string_view name,
        const std::vector<std::string>& uniforms, std::string_view vertex_source_path,
        std::string_view fragment_source_path)
    : program(program), vertex_shader(vertex_shader), fragment_shader(fragment_shader), name(name),
      vertex_source_path(vertex_source_path), fragment_source_path(fragment_source_path),
      uniforms(uniforms) {
    for (const std::string& uniform : uniforms) {
        GLint location = glGetUniformLocation(program, uniform.c_str());
        if (location == -1) {
            DEB_ERROR("Uniform variable '{}' in shader '{}' not found", uniform.c_str(), name.data());
            continue;
        }
        cache[uniform] = location;
    }

    DEB_DEBUG("Created shader {} ({})", program, name.data());
}

Shader::~Shader() {
    DELETE_SHADER(program, vertex_shader, fragment_shader);

    DEB_DEBUG("Deleted shader {} ({})", program, name.c_str());
}

std::shared_ptr<Shader> Shader::create(std::string_view vertex_source_path,
        std::string_view fragment_source_path, const std::vector<std::string>& uniforms) {
    GLuint vertex_shader = compile_shader(vertex_source_path, GL_VERTEX_SHADER);
    GLuint fragment_shader = compile_shader(fragment_source_path, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glValidateProgram(program);

    if (!check_linking(program)) {
        REL_CRITICAL("Exiting...");
        exit(1);
    }

    std::string name = get_name(vertex_source_path, fragment_source_path);

    return std::make_shared<Shader>(program, vertex_shader, fragment_shader, name, uniforms,
            vertex_source_path, fragment_source_path);
}

std::shared_ptr<Shader> Shader::create(std::string_view vertex_source_path,
        std::string_view fragment_source_path, const std::vector<std::string>& uniforms,
        const std::vector<UniformBlockSpecification>& uniform_blocks) {
    GLuint vertex_shader;
    GLuint fragment_shader;
    try {
        vertex_shader = compile_shader(vertex_source_path, GL_VERTEX_SHADER);
        fragment_shader = compile_shader(fragment_source_path, GL_FRAGMENT_SHADER);
    } catch (const std::runtime_error& e) {
        REL_CRITICAL("{}, exiting...", e.what());
        exit(1);
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glValidateProgram(program);

    if (!check_linking(program)) {
        REL_CRITICAL("Exiting...");
        exit(1);
    }

    for (const UniformBlockSpecification& block : uniform_blocks) {
        GLuint block_index = glGetUniformBlockIndex(program, block.block_name.c_str());

        if (block_index == GL_INVALID_INDEX) {
            REL_CRITICAL("Invalid block index, exiting...");
            exit(1);
        }

        if (!block.uniform_buffer->configured) {
            GLint block_size;
            glGetActiveUniformBlockiv(program, block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &block_size);

            block.uniform_buffer->data = new char[block_size];
            block.uniform_buffer->size = block_size;

            glBindBuffer(GL_UNIFORM_BUFFER, block.uniform_buffer->buffer);
            glBufferData(GL_UNIFORM_BUFFER, block_size, nullptr, GL_STREAM_DRAW);
            LOG_ALLOCATION(block_size);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            assert(block.field_count <= 16);

            GLuint indices[16];
            GLint offsets[16];
            GLint sizes[16];
            GLint types[16];

            glGetUniformIndices(program, block.field_count, block.field_names, indices);

            for (unsigned int i = 0; i < block.field_count; i++) {
                if (indices[i] == GL_INVALID_INDEX) {
                    REL_CRITICAL("Invalid field index, exiting...");
                    exit(1);
                }
            }

            glGetActiveUniformsiv(program, block.field_count, indices, GL_UNIFORM_OFFSET, offsets);
            glGetActiveUniformsiv(program, block.field_count, indices, GL_UNIFORM_SIZE, sizes);
            glGetActiveUniformsiv(program, block.field_count, indices, GL_UNIFORM_TYPE, types);

            for (unsigned int i = 0; i < block.field_count; i++) {
                block.uniform_buffer->fields[i] = {
                    static_cast<size_t>(offsets[i]),
                    static_cast<size_t>(sizes[i]) * type_size(types[i])
                };
            }

            block.uniform_buffer->configured = true;
        }

        glBindBufferBase(GL_UNIFORM_BUFFER, block.binding_index, block.uniform_buffer->buffer);  // TODO max uniform buffer bindings
    }

    std::string name = get_name(vertex_source_path, fragment_source_path);

    return std::make_shared<Shader>(program, vertex_shader, fragment_shader, name, uniforms,
            vertex_source_path, fragment_source_path);
}

void Shader::bind() {
    glUseProgram(program);
}

void Shader::unbind() {
    glUseProgram(0);
}

void Shader::set_uniform_mat4(std::string_view name, const glm::mat4& matrix) {
    GLint location = get_uniform_location(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::set_uniform_int(std::string_view name, int value) {
    GLint location = get_uniform_location(name);
    glUniform1i(location, value);
}

void Shader::set_uniform_float(std::string_view name, float value) {
    GLint location = get_uniform_location(name);
    glUniform1f(location, value);
}

void Shader::set_uniform_vec2(std::string_view name, glm::vec2 vector) {
    GLint location = get_uniform_location(name);
    glUniform2f(location, vector.x, vector.y);
}

void Shader::set_uniform_vec3(std::string_view name, const glm::vec3& vector) {
    GLint location = get_uniform_location(name);
    glUniform3f(location, vector.x, vector.y, vector.z);
}

void Shader::set_uniform_vec4(std::string_view name, const glm::vec4& vector) {
    GLint location = get_uniform_location(name);
    glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void Shader::recompile() {
    GLuint new_vertex_shader;
    GLuint new_fragment_shader;
    try {
        new_vertex_shader = compile_shader(vertex_source_path, GL_VERTEX_SHADER);
        new_fragment_shader = compile_shader(fragment_source_path, GL_FRAGMENT_SHADER);
    } catch (const std::runtime_error&) {
        DEB_ERROR("Abort recompiling");
        return;
    }

    GLuint new_program = glCreateProgram();
    glAttachShader(new_program, new_vertex_shader);
    glAttachShader(new_program, new_fragment_shader);
    glLinkProgram(new_program);
    glValidateProgram(new_program);

    if (!check_linking(new_program)) {
        DEB_ERROR("Abort recompiling");
        DELETE_SHADER(new_program, new_vertex_shader, new_fragment_shader);
        return;
    }

    uniforms.clear();
    for (const std::string& uniform : uniforms) {
        GLint location = glGetUniformLocation(program, uniform.c_str());
        if (location == -1) {
            DEB_ERROR("Uniform variable '{}' in shader '{}' not found", uniform.c_str(), name.data());
            continue;
        }
        cache[uniform] = location;
    }

    DELETE_SHADER(program, vertex_shader, fragment_shader);

    DEB_DEBUG("Recompiled old shader {} to new shader {} ({})", program, new_program, name.c_str());

    program = new_program;
    vertex_shader = new_vertex_shader;
    fragment_shader = new_fragment_shader;
}

GLint Shader::get_uniform_location(std::string_view name) {
#if defined(NINE_MORRIS_3D_RELEASE)
    return cache[std::string(name)];
#elif defined(NINE_MORRIS_3D_DEBUG)
    try {
        return cache.at(std::string(name));
    } catch (const std::out_of_range&) {
        DEB_CRITICAL("Uniform variable '{}' unspecified for shader '{}', exiting...", name.data(),
                this->name.data());
        exit(1);
    }
#endif
}

GLuint Shader::compile_shader(std::string_view source_path, GLenum type) noexcept(false) {
    std::ifstream file {std::string(source_path)};
    std::string source;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            source.append(line).append("\n");
        }
    } else {
        REL_CRITICAL("Could not open file '{}', exiting...", source_path.data());
        exit(1);
    }
    file.close();

    GLuint shader = glCreateShader(type);
    const char* s = source.c_str();
    glShaderSource(shader, 1, &s, nullptr);
    glCompileShader(shader);

    if (!check_compilation(shader, type)) {
        throw std::runtime_error("Shader compilation error");
    }

    return shader;
}

bool Shader::check_compilation(GLuint shader, GLenum type) {
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
            REL_CRITICAL("{} shader compilation error with no message", t);
        } else {
            char* log_message = new char[log_length];
            glGetShaderInfoLog(shader, log_length, nullptr, log_message);

            REL_CRITICAL("{} shader compilation error\n{}", t, log_message);
            delete[] log_message;
        }

        return false;
    }

    return true;
}

bool Shader::check_linking(GLuint program) {
    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);

    if (link_status == GL_FALSE) {
        GLsizei log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

        if (log_length == 0) {
            REL_CRITICAL("Shader linking error with no message");
        } else {
            char* log_message = new char[log_length];
            glGetProgramInfoLog(program, log_length, nullptr, log_message);

            REL_CRITICAL("Shader linking error\n{}", log_message);
            delete[] log_message;
        }

        return false;
    }

    return true;
}

std::string Shader::get_name(std::string_view vertex_source, std::string_view fragment_source) {
    size_t last_slash_v = vertex_source.find_last_of("/");
    assert(last_slash_v != std::string::npos);
    const std::string vertex = std::string(vertex_source.substr(last_slash_v + 1));

    size_t last_slash_f = fragment_source.find_last_of("/");
    assert(last_slash_f != std::string::npos);
    const std::string fragment = std::string(fragment_source.substr(last_slash_f + 1));

    return vertex + " & " + fragment;
}
