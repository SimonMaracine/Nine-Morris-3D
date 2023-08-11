#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <memory>
#include <initializer_list>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <stdexcept>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/application_base/platform.hpp"
#include "engine/application_base/panic.hpp"
#include "engine/graphics/opengl/shader.hpp"
#include "engine/graphics/opengl/buffer.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/assert.hpp"
#include "engine/other/encrypt.hpp"

#define CASE(enum_, count, type) case enum_: size = (count) * sizeof(type); break;

namespace sm {
    static std::size_t type_size(GLenum type) {
        std::size_t size = 0;

        switch (type) {
            CASE(GL_FLOAT_VEC3, 3, GLfloat)
            CASE(GL_FLOAT_MAT4, 16, GLfloat)
            default:
                LOG_DIST_CRITICAL("Unknown type `{}`", type);
                panic();
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
                LOG_DIST_CRITICAL("{} shader compilation error with no message in shader `{}`", t, name);
            } else {
                char* log_message = new char[log_length];
                glGetShaderInfoLog(shader, log_length, nullptr, log_message);

                LOG_DIST_CRITICAL("{} shader compilation error in shader `{}`\n{}", t, name, log_message);
                delete[] log_message;
            }

            return false;
        }

        return true;
    }

    static GLuint compile_shader(std::string_view source_path, GLenum type, std::string_view name) noexcept(false) {
        std::ifstream file {std::string(source_path), std::ios::binary};

        if (!file.is_open()) {
            LOG_DIST_CRITICAL("Could not open file `{}` for reading", source_path);
            panic();
        }

        file.seekg(0, file.end);
        const std::size_t length = file.tellg();
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
            throw std::runtime_error("Shader compilation error");  // FIXME put these in class and don't use exceptons
        }

        return shader;
    }

    static GLuint compile_shader(const std::pair<unsigned char*, std::size_t>& source_buffer, GLenum type, std::string_view name) noexcept(false) {
        GLuint shader = glCreateShader(type);

        const char* buffer = reinterpret_cast<const char*>(source_buffer.first);  // It is safe
        const char* const source = buffer;
        const int source_length = source_buffer.second;

        glShaderSource(shader, 1, &source, &source_length);
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
                LOG_DIST_CRITICAL("Shader linking error with no message in shader `{}`", name);
            } else {
                char* log_message = new char[log_length];
                glGetProgramInfoLog(program, log_length, nullptr, log_message);

                LOG_DIST_CRITICAL("Shader linking error in shader `{}`\n{}", name, log_message);
                delete[] log_message;
            }

            return false;
        }

        return true;
    }

    static std::string get_name_sources(std::string_view vertex_source, std::string_view fragment_source) {
        std::size_t last_slash_v = vertex_source.find_last_of("/");
        SM_ASSERT(last_slash_v != std::string::npos, "Could not find slash");
        const std::string vertex = std::string(vertex_source.substr(last_slash_v + 1));

        std::size_t last_slash_f = fragment_source.find_last_of("/");
        SM_ASSERT(last_slash_f != std::string::npos, "Could not find slash");
        const std::string fragment = std::string(fragment_source.substr(last_slash_f + 1));

        return vertex + " & " + fragment;
    }

    GlShader::GlShader(std::string_view vertex_source, std::string_view fragment_source,
            Uniforms uniforms, UniformBlocks uniform_blocks)
        : vertex_source_path(vertex_source), fragment_source_path(fragment_source) {
        name = get_name_sources(vertex_source_path, fragment_source_path);

        try {
            vertex_shader = compile_shader(vertex_source_path, GL_VERTEX_SHADER, name);
            fragment_shader = compile_shader(fragment_source_path, GL_FRAGMENT_SHADER, name);
        } catch (const std::runtime_error& e) {
            LOG_DIST_CRITICAL("Could not compile shaders: {}", e.what());
            panic();
        }

        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
        glValidateProgram(program);

        if (!check_linking(program, name)) {
            LOG_DIST_CRITICAL("Exiting...");
            panic();
        }

        check_and_cache_uniforms(uniforms);

        if (uniform_blocks.size() > 0) {
            configure_uniform_blocks(program, uniform_blocks);
        }

        LOG_DEBUG("Created GL shader {} ({})", program, name);
    }

    GlShader::GlShader(Encrypt::EncryptedFile vertex_source, Encrypt::EncryptedFile fragment_source,
            Uniforms uniforms, UniformBlocks uniform_blocks)
        : vertex_source_path(vertex_source), fragment_source_path(fragment_source) {
        name = get_name_sources(vertex_source_path, fragment_source_path);

        const auto buffer_vertex = Encrypt::load_file(vertex_source);
        const auto buffer_fragment = Encrypt::load_file(fragment_source);

        try {
            vertex_shader = compile_shader(buffer_vertex, GL_VERTEX_SHADER, name);
            fragment_shader = compile_shader(buffer_fragment, GL_FRAGMENT_SHADER, name);
        } catch (const std::runtime_error& e) {
            LOG_DIST_CRITICAL("Could not compile shaders: {}", e.what());
            panic();
        }

        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
        glValidateProgram(program);

        if (!check_linking(program, name)) {
            LOG_DIST_CRITICAL("Exiting...");
            panic();
        }

        check_and_cache_uniforms(uniforms);

        if (uniform_blocks.size() > 0) {
            configure_uniform_blocks(program, uniform_blocks);
        }

        LOG_DEBUG("Created GL shader {} ({})", program, name);
    }

    GlShader::~GlShader() {
        delete_shader(program, vertex_shader, fragment_shader);

        LOG_DEBUG("Deleted GL shader {} ({})", program, name);
    }

    void GlShader::bind() {
        glUseProgram(program);
    }

    void GlShader::unbind() {
        glUseProgram(0);
    }

    void GlShader::upload_uniform_mat4(Key name, const glm::mat4& matrix) {
        const GLint location = get_uniform_location(name);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void GlShader::upload_uniform_int(Key name, int value) {
        const GLint location = get_uniform_location(name);
        glUniform1i(location, value);
    }

    void GlShader::upload_uniform_float(Key name, float value) {
        const GLint location = get_uniform_location(name);
        glUniform1f(location, value);
    }

    void GlShader::upload_uniform_vec2(Key name, glm::vec2 vector) {
        const GLint location = get_uniform_location(name);
        glUniform2f(location, vector.x, vector.y);
    }

    void GlShader::upload_uniform_vec3(Key name, const glm::vec3& vector) {
        const GLint location = get_uniform_location(name);
        glUniform3f(location, vector.x, vector.y, vector.z);
    }

    void GlShader::upload_uniform_vec4(Key name, const glm::vec4& vector) {
        const GLint location = get_uniform_location(name);
        glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
    }

#if 0
    void GlShader::recompile() {
        GLuint new_vertex_shader;
        GLuint new_fragment_shader;
        try {
            new_vertex_shader = compile_shader(vertex_source_path, GL_VERTEX_SHADER, name);
            new_fragment_shader = compile_shader(fragment_source_path, GL_FRAGMENT_SHADER, name);
        } catch (const std::runtime_error&) {
            LOG_ERROR("Abort recompiling");
            return;
        }

        GLuint new_program = glCreateProgram();
        glAttachShader(new_program, new_vertex_shader);
        glAttachShader(new_program, new_fragment_shader);
        glLinkProgram(new_program);
        glValidateProgram(new_program);

        if (!check_linking(new_program, name)) {
            LOG_ERROR("Abort recompiling");
            delete_shader(new_program, new_vertex_shader, new_fragment_shader);
            return;
        }

        cache.clear();
        check_and_cache_uniforms(uniforms);

        delete_shader(program, vertex_shader, fragment_shader);

        LOG_DEBUG("Recompiled old shader {} to new shader {} ({})", program, new_program, name);

        program = new_program;
        vertex_shader = new_vertex_shader;
        fragment_shader = new_fragment_shader;
    }
#endif

    int GlShader::get_uniform_location(Key name) const {
#ifdef SM_BUILD_DEBUG
        try {
            return cache.at(name);
        } catch (const std::out_of_range&) {
            LOG_ERROR("Cannot get hashed uniform variable `{}` from shader `{}`", name, this->name);
            return -1;
        }
#else
        return cache.at(name);
#endif
    }

    void GlShader::check_and_cache_uniforms(Uniforms uniforms) {
        for (const auto& uniform : uniforms) {
            this->uniforms.push_back(uniform);

            const GLint location = glGetUniformLocation(program, uniform.c_str());

            if (location == -1) {
                LOG_ERROR("Uniform variable `{}` in shader `{}` not found", uniform, name);
                continue;
            }

            cache[Key(uniform)] = location;
        }
    }

    void GlShader::configure_uniform_blocks(unsigned int program, const UniformBlocks& uniform_blocks) {
        if (uniform_blocks.size() == 0) {
            LOG_WARNING("Uniform blocks structure is empty; this function does nothing");
        }

        for (const UniformBlockSpecification& block : uniform_blocks) {
            const GLuint block_index = glGetUniformBlockIndex(program, block.block_name.c_str());

            if (block_index == GL_INVALID_INDEX) {
                LOG_DIST_CRITICAL("Invalid block index");
                panic();
            }

            // If it's already configured, skip everything else
            if (block.uniform_buffer->is_configured()) {
                continue;
            }

            // Get data block size
            GLint block_size;
            glGetActiveUniformBlockiv(program, block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &block_size);

            // Allocate memory on both CPU and GPU side
            block.uniform_buffer->bind();
            block.uniform_buffer->allocate_memory(block_size);
            GlUniformBuffer::unbind();

            // Link uniform buffer to binding index
            glBindBufferBase(GL_UNIFORM_BUFFER, block.binding_index, block.uniform_buffer->buffer);  // TODO move this to buffer class

            const std::size_t field_count = block.field_names.size();
            static constexpr std::size_t MAX_FIELD_COUNT = 8;

            SM_ASSERT(field_count <= MAX_FIELD_COUNT, "Maximum 8 fields for now");

            GLuint indices[MAX_FIELD_COUNT];
            GLint offsets[MAX_FIELD_COUNT];
            GLint sizes[MAX_FIELD_COUNT];
            GLint types[MAX_FIELD_COUNT];

            // Create the uniforms names list; the order of these names matters
            char* field_names[MAX_FIELD_COUNT];

            for (std::size_t i = 0; i < field_count; i++) {
                const std::string& name = block.field_names[i];
                const std::size_t size = name.size() + 1;

                field_names[i] = new char[size];
                std::strncpy(field_names[i], name.c_str(), size);
                field_names[i][size - 1] = '\0';  // Not really needed
            }

            // Get uniform indices just to later get offsets, sizes and types
            glGetUniformIndices(
                program,
                field_count,
                static_cast<const char* const*>(field_names),
                indices
            );

            for (std::size_t i = 0; i < field_count; i++) {
                delete[] field_names[i];
            }

            for (std::size_t i = 0; i < field_count; i++) {
                if (indices[i] == GL_INVALID_INDEX) {
                    LOG_DIST_CRITICAL("Invalid field index");
                    panic();
                }
            }

            glGetActiveUniformsiv(program, field_count, indices, GL_UNIFORM_OFFSET, offsets);
            glGetActiveUniformsiv(program, field_count, indices, GL_UNIFORM_SIZE, sizes);  // For arrays
            glGetActiveUniformsiv(program, field_count, indices, GL_UNIFORM_TYPE, types);

            // Finally setup the uniform block fields
            for (std::size_t i = 0; i < field_count; i++) {
                UniformBlockField field;
                field.offset = static_cast<std::size_t>(offsets[i]);
                field.size = static_cast<std::size_t>(sizes[i]) * type_size(types[i]);

                block.uniform_buffer->add_field(i, field);
            }

            block.uniform_buffer->set_configured();
        }
    }
}
