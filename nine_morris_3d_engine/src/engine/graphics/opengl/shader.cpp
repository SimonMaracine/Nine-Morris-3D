#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <cppblowfish/cppblowfish.h>

#include "engine/application/platform.h"
#include "engine/graphics/opengl/shader.h"
#include "engine/graphics/opengl/buffer.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"
#include "engine/other/encrypt.h"
#include "engine/other/exit.h"

#define CASE(_enum, count, type) case _enum: size = (count) * sizeof(type); break;

static size_t type_size(GLenum type) {
    size_t size = 0;

    switch (type) {
        CASE(GL_FLOAT_VEC3, 3, GLfloat)
        CASE(GL_FLOAT_MAT4, 16, GLfloat)
        default:
            REL_CRITICAL("Unknown type `{}`, exiting...", type);
            application_exit::panic();
    }

    return size;
}

static void delete_shader(GLuint program, GLuint vertex_shader, GLuint fragment_shader) {
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(program);
}

static bool check_compilation(GLuint shader, GLenum type, std::string_view name) noexcept(false) {
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
            REL_CRITICAL("{} shader compilation error with no message in shader `{}`", t, name);
        } else {
            char* log_message = new char[log_length];
            glGetShaderInfoLog(shader, log_length, nullptr, log_message);

            REL_CRITICAL("{} shader compilation error in shader `{}`\n{}", t, name, log_message);
            delete[] log_message;
        }

        return false;
    }

    return true;
}

static GLuint compile_shader(std::string_view source_path, GLenum type, std::string_view name) noexcept(false) {
    std::ifstream file {std::string(source_path), std::ios::binary};

    if (!file.is_open()) {
        REL_CRITICAL("Could not open file `{}` for reading, exiting...", source_path);
        application_exit::panic();
    }

    file.seekg(0, file.end);
    const size_t length = file.tellg();
    file.seekg(0, file.beg);

    char* buffer = new char[length];
    file.read(buffer, length);

    GLuint shader = glCreateShader(type);

    const char* const source = buffer;
    const int source_length = length;

    glShaderSource(shader, 1, &source, &source_length);
    glCompileShader(shader);

    delete[] buffer;

    if (!check_compilation(shader, type, name)) {
        throw std::runtime_error("Shader compilation error");
    }

    return shader;
}

static GLuint compile_shader(const cppblowfish::Buffer& source_buffer, GLenum type, std::string_view name) noexcept(false) {
    GLuint shader = glCreateShader(type);

    const int length = source_buffer.size() - source_buffer.padding();
    char* source = reinterpret_cast<char*>(source_buffer.get());
    const char* const s = source;

    glShaderSource(shader, 1, &s, &length);
    glCompileShader(shader);

    if (!check_compilation(shader, type, name)) {
        throw std::runtime_error("Shader compilation error");
    }

    return shader;
}

static bool check_linking(GLuint program, std::string_view name) {
    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);

    if (link_status == GL_FALSE) {
        GLsizei log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

        if (log_length == 0) {
            REL_CRITICAL("Shader linking error with no message in shader `{}`", name);
        } else {
            char* log_message = new char[log_length];
            glGetProgramInfoLog(program, log_length, nullptr, log_message);

            REL_CRITICAL("Shader linking error in shader `{}`\n{}", name, log_message);
            delete[] log_message;
        }

        return false;
    }

    return true;
}

static std::string get_name_sources(std::string_view vertex_source, std::string_view fragment_source) {
    size_t last_slash_v = vertex_source.find_last_of("/");
    ASSERT(last_slash_v != std::string::npos, "Could not find slash");
    const std::string vertex = std::string(vertex_source.substr(last_slash_v + 1));

    size_t last_slash_f = fragment_source.find_last_of("/");
    ASSERT(last_slash_f != std::string::npos, "Could not find slash");
    const std::string fragment = std::string(fragment_source.substr(last_slash_f + 1));

    return vertex + " & " + fragment;
}

namespace gl {
    Shader::Shader(std::string_view vertex_source_path, std::string_view fragment_source_path,
            const std::vector<std::string>& uniforms, const UniformBlocks& uniform_blocks)
        : vertex_source_path(vertex_source_path), fragment_source_path(fragment_source_path), uniforms(uniforms) {
        name = get_name_sources(vertex_source_path, fragment_source_path);

        try {
            vertex_shader = compile_shader(vertex_source_path, GL_VERTEX_SHADER, name);
            fragment_shader = compile_shader(fragment_source_path, GL_FRAGMENT_SHADER, name);
        } catch (const std::runtime_error& e) {
            REL_CRITICAL("Could not compile shaders: {}, exiting...", e.what());
            application_exit::panic();
        }

        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
        glValidateProgram(program);

        if (!check_linking(program, name)) {
            REL_CRITICAL("Exiting...");
            application_exit::panic();
        }

        check_and_cache_uniforms(uniforms);

        if (uniform_blocks.size() > 0) {
            configure_uniform_blocks(program, uniform_blocks);
        }

        DEB_DEBUG("Created GL shader {} ({})", program, name);
    }

    Shader::Shader(encrypt::EncryptedFile vertex_source_path, encrypt::EncryptedFile fragment_source_path,
            const std::vector<std::string>& uniforms, const UniformBlocks& uniform_blocks)
        : vertex_source_path(vertex_source_path), fragment_source_path(fragment_source_path), uniforms(uniforms) {
        name = get_name_sources(vertex_source_path, fragment_source_path);

        const cppblowfish::Buffer buffer_vertex = encrypt::load_file(vertex_source_path);
        const cppblowfish::Buffer buffer_fragment = encrypt::load_file(fragment_source_path);

        try {
            vertex_shader = compile_shader(buffer_vertex, GL_VERTEX_SHADER, name);
            fragment_shader = compile_shader(buffer_fragment, GL_FRAGMENT_SHADER, name);
        } catch (const std::runtime_error& e) {
            REL_CRITICAL("Could not compile shaders: {}, exiting...", e.what());
            application_exit::panic();
        }

        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
        glValidateProgram(program);

        if (!check_linking(program, name)) {
            REL_CRITICAL("Exiting...");
            application_exit::panic();
        }

        check_and_cache_uniforms(uniforms);

        if (uniform_blocks.size() > 0) {
            configure_uniform_blocks(program, uniform_blocks);
        }

        DEB_DEBUG("Created GL shader {} ({})", program, name);
    }

    Shader::~Shader() {
        delete_shader(program, vertex_shader, fragment_shader);

        DEB_DEBUG("Deleted GL shader {} ({})", program, name);
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
            delete_shader(new_program, new_vertex_shader, new_fragment_shader);
            return;
        }

        cache.clear();
        check_and_cache_uniforms(uniforms);

        delete_shader(program, vertex_shader, fragment_shader);

        DEB_DEBUG("Recompiled old shader {} to new shader {} ({})", program, new_program, name);

        program = new_program;
        vertex_shader = new_vertex_shader;
        fragment_shader = new_fragment_shader;
    }

    GLint Shader::get_uniform_location(std::string_view name) {
#if defined(NM3D_PLATFORM_RELEASE)
        return cache[std::string(name)];
#elif defined(NM3D_PLATFORM_DEBUG)
        try {
            return cache.at(std::string(name));
        } catch (const std::out_of_range&) {
            DEB_ERROR("Cannot get uniform variable `{}` from shader `{}`", name.data(), this->name);
            return -1;
        }
#endif
    }

    void Shader::check_and_cache_uniforms(const std::vector<std::string>& uniforms) {
        for (const auto& uniform : uniforms) {
            const GLint location = glGetUniformLocation(program, uniform.c_str());

            if (location == -1) {
                DEB_ERROR("Uniform variable `{}` in shader `{}` not found", uniform.c_str(), name);
                continue;
            }

            cache[uniform] = location;
        }
    }

    void Shader::configure_uniform_blocks(GLuint program, const UniformBlocks& uniform_blocks) {
        if (uniform_blocks.size() == 0) {
            DEB_WARNING("Uniform blocks structure is empty; this function does nothing");
        }

        for (const gl::UniformBlockSpecification& block : uniform_blocks) {
            const GLuint block_index = glGetUniformBlockIndex(program, block.block_name.c_str());

            if (block_index == GL_INVALID_INDEX) {
                REL_CRITICAL("Invalid block index, exiting...");
                application_exit::panic();
            }

            // If it's already configured, skip to the binding call
            if (block.uniform_buffer->configured) {
                goto just_bind;
            }

            // Get data block size
            GLint block_size;
            glGetActiveUniformBlockiv(program, block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &block_size);

            // Allocate that amount on the CPU side
            block.uniform_buffer->data = new char[block_size];
            block.uniform_buffer->size = block_size;

            // Allocate that amount on the GPU side
            glBindBuffer(GL_UNIFORM_BUFFER, block.uniform_buffer->buffer);
            glBufferData(GL_UNIFORM_BUFFER, block_size, nullptr, GL_STREAM_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            ASSERT(block.field_count <= 16, "Maximum 16 fields for now");

            GLuint indices[16];
            GLint offsets[16];
            GLint sizes[16];
            GLint types[16];

            char* field_names[16];

            for (size_t i = 0; i < block.field_count; i++) {
                const std::string& name = block.field_names[i];
                const size_t size = name.size() + 1;

                field_names[i] = new char[size];
                strncpy(field_names[i], name.c_str(), size);
            }

            // Get uniform indices just to later get offsets, sizes and types
            glGetUniformIndices(
                program, block.field_count, const_cast<const char* const*>(field_names), indices
            );

            for (size_t i = 0; i < block.field_count; i++) {
                delete[] field_names[i];
            }

            for (size_t i = 0; i < block.field_count; i++) {
                if (indices[i] == GL_INVALID_INDEX) {
                    REL_CRITICAL("Invalid field index, exiting...");
                    application_exit::panic();
                }
            }

            glGetActiveUniformsiv(program, block.field_count, indices, GL_UNIFORM_OFFSET, offsets);
            glGetActiveUniformsiv(program, block.field_count, indices, GL_UNIFORM_SIZE, sizes);
            glGetActiveUniformsiv(program, block.field_count, indices, GL_UNIFORM_TYPE, types);

            // Finally setup the uniform block fields
            for (size_t i = 0; i < block.field_count; i++) {
                block.uniform_buffer->fields[i] = {
                    static_cast<size_t>(offsets[i]),
                    static_cast<size_t>(sizes[i]) * type_size(types[i])
                };
            }

            block.uniform_buffer->configured = true;

            just_bind:
            glBindBufferBase(GL_UNIFORM_BUFFER, block.binding_index, block.uniform_buffer->buffer);
        }
    }
}
