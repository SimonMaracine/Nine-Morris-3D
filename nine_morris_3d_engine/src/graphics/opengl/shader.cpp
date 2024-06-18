#include "engine/graphics/opengl/shader.hpp"

#include <stdexcept>
#include <algorithm>
#include <cassert>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "engine/application_base/platform.hpp"
#include "engine/application_base/panic.hpp"
#include "engine/other/logging.hpp"

namespace sm {
    GlShader::GlShader(const std::string& source_vertex, const std::string& source_fragment) {
        // name = Utils::get_file_name(source_vertex) + " & " + Utils::get_file_name(source_fragment);  // FIXME

        vertex_shader = compile_shader(source_vertex, GL_VERTEX_SHADER);
        fragment_shader = compile_shader(source_fragment, GL_FRAGMENT_SHADER);
        program = create_program();

        if (!check_linking(program)) {
            LOG_DIST_CRITICAL("Could not link shader program {}", program);
            throw ResourceLoadingError;
        }

        delete_intermediates();
        introspect_program();
        check_and_cache_uniforms();

        LOG_DEBUG("Created GL shader {} `{}`", program, name);
    }

    // GlShader::GlShader(const EncrFile& source_vertex, const EncrFile& source_fragment) {
    //     name = Utils::get_file_name(source_vertex) + " & " + Utils::get_file_name(source_fragment);

    //     const auto buffer_vertex {Encrypt::load_file(source_vertex)};
    //     const auto buffer_fragment {Encrypt::load_file(source_fragment)};

    //     vertex_shader = compile_shader(buffer_vertex, GL_VERTEX_SHADER);
    //     fragment_shader = compile_shader(buffer_fragment, GL_FRAGMENT_SHADER);
    //     program = create_program();

    //     if (!check_linking(program)) {
    //         LOG_DIST_CRITICAL("Could not link shader program {}", program);
    //         throw ResourceLoadingError;
    //     }

    //     delete_intermediates();
    //     introspect_program();
    //     check_and_cache_uniforms();

    //     LOG_DEBUG("Created GL shader {} `{}`", program, name);
    // }

    GlShader::~GlShader() {
        glDeleteProgram(program);

        LOG_DEBUG("Deleted GL shader {} `{}`", program, name);
    }

    void GlShader::bind() const {
        glUseProgram(program);
    }

    void GlShader::unbind() {
        glUseProgram(0);
    }

    void GlShader::upload_uniform_mat4(Id name, const glm::mat4& matrix) const {
        const int location {get_uniform_location(name)};
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void GlShader::upload_uniform_int(Id name, int value) const {
        const int location {get_uniform_location(name)};
        glUniform1i(location, value);
    }

    void GlShader::upload_uniform_float(Id name, float value) const {
        const int location {get_uniform_location(name)};
        glUniform1f(location, value);
    }

    void GlShader::upload_uniform_vec2(Id name, glm::vec2 vector) const {
        const int location {get_uniform_location(name)};
        glUniform2f(location, vector.x, vector.y);
    }

    void GlShader::upload_uniform_vec3(Id name, const glm::vec3& vector) const {
        const int location {get_uniform_location(name)};
        glUniform3f(location, vector.x, vector.y, vector.z);
    }

    void GlShader::upload_uniform_vec4(Id name, const glm::vec4& vector) const {
        const int location {get_uniform_location(name)};
        glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
    }

    void GlShader::add_uniform_buffer(std::shared_ptr<GlUniformBuffer> uniform_buffer) {
        uniform_buffer->bind();
        uniform_buffer->configure(program);  // No problem, if it's already configured
        GlUniformBuffer::unbind();

        uniform_buffers.push_back(uniform_buffer);
    }

    int GlShader::get_uniform_location(Id name) const {
#ifdef SM_BUILD_DEBUG
        try {
            return cache.at(name);
        } catch (const std::out_of_range&) {
            LOG_ERROR("Cannot get hashed uniform variable `{}` from program `{}`", name, this->name);
            return -1;
        }
#else
        return cache.at(name);
#endif
    }

    void GlShader::check_and_cache_uniforms() {
        for (const auto& uniform : uniforms) {
            int location {};

            // Don't touch uniforms that are in blocks
            for (const auto& block : uniform_blocks) {
                if (std::find(block.uniforms.cbegin(), block.uniforms.cend(), uniform) != block.uniforms.cend()) {
                    goto skip_uniform;
                }
            }

            location = glGetUniformLocation(program, uniform.c_str());

            if (location == -1) {
                LOG_ERROR("Uniform variable `{}` in program `{}` not found", uniform, name);
                continue;
            }

            cache[Id(uniform)] = location;

            skip_uniform:
            continue;
        }
    }

    void GlShader::introspect_program() {
        // Uniforms stuff
        int uniform_count;
        glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniform_count);

        uniforms.reserve(uniform_count);

        for (unsigned int i {0}; i < static_cast<unsigned int>(uniform_count); i++) {
            char buffer[64] {};
            glGetProgramResourceName(program, GL_UNIFORM, i, 64, nullptr, buffer);

            uniforms.push_back(buffer);
        }

        // Uniform blocks stuff
        int uniform_block_count;
        glGetProgramInterfaceiv(program, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &uniform_block_count);

        uniform_blocks.reserve(uniform_block_count);

        for (unsigned int i {0}; i < static_cast<unsigned int>(uniform_block_count); i++) {
            UniformBlockSpecification block;
            std::size_t block_active_uniforms_count {0};

            {
                char buffer[64] {};
                glGetProgramResourceName(program, GL_UNIFORM_BLOCK, i, 64, nullptr, buffer);

                block.block_name = buffer;
            }

            {
                int buffer[2] {};
                const GLenum properties[] { GL_BUFFER_BINDING, GL_NUM_ACTIVE_VARIABLES };
                glGetProgramResourceiv(program, GL_UNIFORM_BLOCK, i, 2, properties, 2, nullptr, buffer);

                block.binding_index = static_cast<unsigned int>(buffer[0]);
                block_active_uniforms_count = static_cast<std::size_t>(buffer[1]);
                block.uniforms.reserve(block_active_uniforms_count);
            }

            {
                int* buffer_uniforms {new int[block_active_uniforms_count]};
                const GLenum properties[] { GL_ACTIVE_VARIABLES };
                glGetProgramResourceiv(
                    program,
                    GL_UNIFORM_BLOCK,
                    i,
                    1,
                    properties,
                    static_cast<int>(block_active_uniforms_count),
                    nullptr,
                    buffer_uniforms
                );

                for (unsigned int j {0}; j < static_cast<unsigned int>(block_active_uniforms_count); j++) {
                    char buffer[64] {};
                    glGetProgramResourceName(program, GL_UNIFORM, buffer_uniforms[j], 64, nullptr, buffer);

                    block.uniforms.push_back(buffer);
                }

                delete[] buffer_uniforms;
            }

            uniform_blocks.push_back(block);
        }
    }

    unsigned int GlShader::create_program() const {
        assert(vertex_shader != 0);
        assert(fragment_shader != 0);

        const unsigned int program {glCreateProgram()};
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
        glValidateProgram(program);

        return program;
    }

    void GlShader::delete_intermediates() {
        assert(program != 0);
        assert(vertex_shader != 0);
        assert(fragment_shader != 0);

        glDetachShader(program, vertex_shader);
        glDetachShader(program, fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        vertex_shader = 0;
        fragment_shader = 0;
    }

    unsigned int GlShader::compile_shader(const std::string& source, unsigned int type) const {
        const unsigned int shader {glCreateShader(type)};

        const char* const source_data {source.c_str()};
        const int source_length {static_cast<int>(source.size())};

        glShaderSource(shader, 1, &source_data, &source_length);
        glCompileShader(shader);

        if (!check_compilation(shader, type)) {
            LOG_DIST_CRITICAL("Could not compile shader {}", shader);
            throw ResourceLoadingError;
        }

        return shader;
    }

    // unsigned int GlShader::compile_shader(const std::pair<unsigned char*, std::size_t>& source, unsigned int type) const {
    //     const unsigned int shader {glCreateShader(type)};

    //     const char* buffer {reinterpret_cast<const char*>(source_buffer.first)};
    //     const char* const source {buffer};
    //     const int source_length {static_cast<int>(source_buffer.second)};

    //     glShaderSource(shader, 1, &source, &source_length);
    //     glCompileShader(shader);

    //     if (!check_compilation(shader, type)) {
    //         LOG_DIST_CRITICAL("Could not compile shader {}", shader);
    //         throw ResourceLoadingError;
    //     }

    //     return shader;
    // }

    bool GlShader::check_compilation(unsigned int shader, unsigned int type) const {
        int compile_status {};
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

        if (compile_status == GL_FALSE) {
            int log_length {};
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

            const char* type_name {nullptr};
            switch (type) {
                case GL_VERTEX_SHADER:
                    type_name = "Vertex";
                    break;
                case GL_FRAGMENT_SHADER:
                    type_name = "Fragment";
                    break;
                default:
                    type_name = "Unknown";
                    break;
            }

            if (log_length == 0) {
                LOG_DIST_CRITICAL("{} shader compilation error with no message in program `{}`", type_name, name);
            } else {
                char* log_message {new char[log_length]};
                glGetShaderInfoLog(shader, log_length, nullptr, log_message);

                LOG_DIST_CRITICAL("{} shader compilation error in program `{}`\n{}", type_name, name, log_message);
                delete[] log_message;
            }

            return false;
        }

        return true;
    }

    bool GlShader::check_linking(unsigned int program) const {
        int link_status {};
        glGetProgramiv(program, GL_LINK_STATUS, &link_status);

        if (link_status == GL_FALSE) {
            int log_length {};
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

            if (log_length == 0) {
                LOG_DIST_CRITICAL("Linking error with no message in program `{}`", name);
            } else {
                char* log_message {new char[log_length]};
                glGetProgramInfoLog(program, log_length, nullptr, log_message);

                LOG_DIST_CRITICAL("Linking error in program `{}`\n{}", name, log_message);
                delete[] log_message;
            }

            return false;
        }

        return true;
    }
}
