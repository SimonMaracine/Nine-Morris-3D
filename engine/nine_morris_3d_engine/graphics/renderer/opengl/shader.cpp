#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <cppblowfish.h>

#include "nine_morris_3d_engine/application/platform.h"
#include "nine_morris_3d_engine/graphics/debug_opengl.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/shader.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/buffer.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"
#include "nine_morris_3d_engine/other/encrypt.h"

#define DELETE_SHADER(program, vertex_shader, fragment_shader) \
    glDetachShader(program, vertex_shader); \
    glDetachShader(program, fragment_shader); \
    glDeleteShader(vertex_shader); \
    glDeleteShader(fragment_shader); \
    glDeleteProgram(program);

#define CASE(_enum, count, type) case _enum: size = (count) * sizeof(type); break;

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

Shader::Shader(std::string_view vertex_source_path, std::string_view fragment_source_path,
        const std::vector<std::string>& uniforms, const std::vector<UniformBlockSpecification>& uniform_blocks)
    : vertex_source_path(vertex_source_path), fragment_source_path(fragment_source_path),
      uniforms(uniforms) {
    name = get_name(vertex_source_path, fragment_source_path);    

    try {
        vertex_shader = compile_shader(vertex_source_path, GL_VERTEX_SHADER, name);
        fragment_shader = compile_shader(fragment_source_path, GL_FRAGMENT_SHADER, name);
    } catch (const std::runtime_error& e) {
        REL_CRITICAL("{}, exiting...", e.what());
        exit(1);
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glValidateProgram(program);

    if (!check_linking(program, name)) {
        REL_CRITICAL("Exiting...");
        exit(1);
    }

    check_and_cache_uniforms(uniforms);

    if (!uniform_blocks.empty()) {
        configure_uniform_blocks(program, uniform_blocks);
    }

    DEB_DEBUG("Created shader {} ({})", program, name);
}

Shader::Shader(encrypt::EncryptedFile vertex_source_path, encrypt::EncryptedFile fragment_source_path,
        const std::vector<std::string>& uniforms, const std::vector<UniformBlockSpecification>& uniform_blocks)
    : vertex_source_path(vertex_source_path), fragment_source_path(fragment_source_path),
      uniforms(uniforms) {
    name = get_name(vertex_source_path, fragment_source_path);

    const cppblowfish::Buffer buffer_vertex = encrypt::load_file(vertex_source_path);
    const cppblowfish::Buffer buffer_fragment = encrypt::load_file(fragment_source_path);

    try {
        vertex_shader = compile_shader(buffer_vertex, GL_VERTEX_SHADER, name);
        fragment_shader = compile_shader(buffer_fragment, GL_FRAGMENT_SHADER, name);
    } catch (const std::runtime_error& e) {
        REL_CRITICAL("{}, exiting...", e.what());
        exit(1);
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glValidateProgram(program);

    if (!check_linking(program, name)) {
        REL_CRITICAL("Exiting...");
        exit(1);
    }

    check_and_cache_uniforms(uniforms);

    if (!uniform_blocks.empty()) {
        configure_uniform_blocks(program, uniform_blocks);
    }

    DEB_DEBUG("Created shader {} ({})", program, name);
}

Shader::~Shader() {
    DELETE_SHADER(program, vertex_shader, fragment_shader);

    DEB_DEBUG("Deleted shader {} ({})", program, name);
}

void Shader::bind() {
    glUseProgram(program);
}

void Shader::unbind() {
    glUseProgram(0);
}

void Shader::upload_uniform_mat4(std::string_view name, const glm::mat4& matrix) {
    const GLint location = get_uniform_location(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::upload_uniform_int(std::string_view name, int value) {
    const GLint location = get_uniform_location(name);
    glUniform1i(location, value);
}

void Shader::upload_uniform_float(std::string_view name, float value) {
    const GLint location = get_uniform_location(name);
    glUniform1f(location, value);
}

void Shader::upload_uniform_vec2(std::string_view name, glm::vec2 vector) {
    const GLint location = get_uniform_location(name);
    glUniform2f(location, vector.x, vector.y);
}

void Shader::upload_uniform_vec3(std::string_view name, const glm::vec3& vector) {
    const GLint location = get_uniform_location(name);
    glUniform3f(location, vector.x, vector.y, vector.z);
}

void Shader::upload_uniform_vec4(std::string_view name, const glm::vec4& vector) {
    const GLint location = get_uniform_location(name);
    glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void Shader::recompile() {
    GLuint new_vertex_shader;
    GLuint new_fragment_shader;
    try {
        new_vertex_shader = compile_shader(vertex_source_path, GL_VERTEX_SHADER, name);
        new_fragment_shader = compile_shader(fragment_source_path, GL_FRAGMENT_SHADER, name);
    } catch (const std::runtime_error&) {
        DEB_ERROR("Abort recompiling");
        return;
    }

    GLuint new_program = glCreateProgram();
    glAttachShader(new_program, new_vertex_shader);
    glAttachShader(new_program, new_fragment_shader);
    glLinkProgram(new_program);
    glValidateProgram(new_program);

    if (!check_linking(new_program, name)) {
        DEB_ERROR("Abort recompiling");
        DELETE_SHADER(new_program, new_vertex_shader, new_fragment_shader);
        return;
    }

    uniforms.clear();
    for (const std::string& uniform : uniforms) {
        const GLint location = glGetUniformLocation(program, uniform.c_str());
        if (location == -1) {
            DEB_ERROR("Uniform variable '{}' in shader '{}' not found", uniform.c_str(), name);
            continue;
        }
        cache[uniform] = location;
    }

    DELETE_SHADER(program, vertex_shader, fragment_shader);

    DEB_DEBUG("Recompiled old shader {} to new shader {} ({})", program, new_program, name);

    program = new_program;
    vertex_shader = new_vertex_shader;
    fragment_shader = new_fragment_shader;
}

GLint Shader::get_uniform_location(std::string_view name) {
#if defined(PLATFORM_GAME_RELEASE)
    return cache[std::string(name)];
#elif defined(PLATFORM_GAME_DEBUG)
    try {
        return cache.at(std::string(name));
    } catch (const std::out_of_range&) {
        DEB_CRITICAL("Uniform variable '{}' unspecified for shader '{}', exiting...", name.data(), this->name);
        exit(1);
    }
#endif
}

void Shader::check_and_cache_uniforms(const std::vector<std::string>& uniforms) {
    for (const auto& uniform : uniforms) {
        const GLint location = glGetUniformLocation(program, uniform.c_str());
        if (location == -1) {
            DEB_ERROR("Uniform variable '{}' in shader '{}' not found", uniform.c_str(), name);
            continue;
        }
        cache[uniform] = location;
    }
}

GLuint Shader::compile_shader(std::string_view source_path, GLenum type, std::string_view name) noexcept(false) {
    std::ifstream file {std::string(source_path)};
    std::string source;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            source.append(line).append("\n");
        }
    } else {
        REL_CRITICAL("Could not open file '{}', exiting...", source_path);
        exit(1);
    }
    file.close();

    GLuint shader = glCreateShader(type);
    const char* s = source.c_str();
    glShaderSource(shader, 1, &s, nullptr);
    glCompileShader(shader);

    if (!check_compilation(shader, type, name)) {
        throw std::runtime_error("Shader compilation error");
    }

    return shader;
}

GLuint Shader::compile_shader(const cppblowfish::Buffer& source_buffer, GLenum type, std::string_view name) noexcept(false) {
    GLuint shader = glCreateShader(type);
    const int length = source_buffer.size() - source_buffer.padding();
    char* source = reinterpret_cast<char*>(source_buffer.get());
    const char* const s = const_cast<const char* const>(source);  // TODO warning
    glShaderSource(shader, 1, &s, &length);
    glCompileShader(shader);

    if (!check_compilation(shader, type, name)) {
        throw std::runtime_error("Shader compilation error");
    }

    return shader;
}

bool Shader::check_compilation(GLuint shader, GLenum type, std::string_view name) noexcept(false) {
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
            REL_CRITICAL("{} shader compilation error with no message in shader '{}'", t, name);
        } else {
            char* log_message = new char[log_length];
            glGetShaderInfoLog(shader, log_length, nullptr, log_message);

            REL_CRITICAL("{} shader compilation error in shader '{}'\n{}", t, name, log_message);
            delete[] log_message;
        }

        return false;
    }

    return true;
}

bool Shader::check_linking(GLuint program, std::string_view name) {
    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);

    if (link_status == GL_FALSE) {
        GLsizei log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

        if (log_length == 0) {
            REL_CRITICAL("Shader linking error with no message in shader '{}'", name);
        } else {
            char* log_message = new char[log_length];
            glGetProgramInfoLog(program, log_length, nullptr, log_message);

            REL_CRITICAL("Shader linking error in shader '{}'\n{}", name, log_message);
            delete[] log_message;
        }

        return false;
    }

    return true;
}

void Shader::configure_uniform_blocks(GLuint program, const std::vector<UniformBlockSpecification>& uniform_blocks) {
    for (const UniformBlockSpecification& block : uniform_blocks) {
        const GLuint block_index = glGetUniformBlockIndex(program, block.block_name.c_str());

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
            LOG_ALLOCATION(block_size)
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            ASSERT(block.field_count <= 16, "Maximum 16 fields for now");

            GLuint indices[16];
            GLint offsets[16];
            GLint sizes[16];
            GLint types[16];

            char* field_names[16];
            for (size_t i = 0; i < block.field_count; i++) {
                const std::string& name = block.field_names[i];

                field_names[i] = static_cast<char*>(malloc(name.size() + 1));
                strcpy(field_names[i], name.c_str());
            }

            glGetUniformIndices(program, block.field_count, const_cast<const char* const*>(field_names), indices);

            for (size_t i = 0; i < block.field_count; i++) {
                free(field_names[i]);
            }

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

        glBindBufferBase(GL_UNIFORM_BUFFER, block.binding_index, block.uniform_buffer->buffer);
    }
}

std::string Shader::get_name(std::string_view vertex_source, std::string_view fragment_source) {
    size_t last_slash_v = vertex_source.find_last_of("/");
    ASSERT(last_slash_v != std::string::npos, "Could not find slash");
    const std::string vertex = std::string(vertex_source.substr(last_slash_v + 1));

    size_t last_slash_f = fragment_source.find_last_of("/");
    ASSERT(last_slash_f != std::string::npos, "Could not find slash");
    const std::string fragment = std::string(fragment_source.substr(last_slash_f + 1));

    return vertex + " & " + fragment;
}
