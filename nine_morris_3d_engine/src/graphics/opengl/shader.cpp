#include "nine_morris_3d_engine/graphics/opengl/shader.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cassert>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm {
    GlShader::GlShader(const std::string& source_vertex, const std::string& source_fragment) {
        const unsigned int vertex_shader {compile_shader(source_vertex, GL_VERTEX_SHADER)};
        const unsigned int fragment_shader {compile_shader(source_fragment, GL_FRAGMENT_SHADER)};
        create_program(vertex_shader, fragment_shader);

        if (!check_linking(program)) {
            SM_THROW_ERROR(ResourceError, "Could not link shader program {}", program);
        }

        delete_intermediates(vertex_shader, fragment_shader);
        const auto uniforms {introspect_program()};
        check_and_cache_uniforms(uniforms);

        LOG_DEBUG("Created GL shader {}", program);
    }

    GlShader::~GlShader() {
        glDeleteProgram(program);

        LOG_DEBUG("Deleted GL shader {}", program);
    }

    void GlShader::bind() const {
        glUseProgram(program);
    }

    void GlShader::unbind() {
        glUseProgram(0);
    }

    void GlShader::upload_uniform_mat3(Id name, const glm::mat3& matrix) const {
        const int location {get_uniform_location(name)};
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void GlShader::upload_uniform_mat3_array(Id name, const std::vector<glm::mat3>& matrices) const {
        const int location {get_uniform_location(name)};
        glUniformMatrix3fv(location, static_cast<int>(matrices.size()), GL_FALSE, glm::value_ptr(matrices.front()));
    }

    void GlShader::upload_uniform_mat4(Id name, const glm::mat4& matrix) const {
        const int location {get_uniform_location(name)};
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void GlShader::upload_uniform_mat4_array(Id name, const std::vector<glm::mat4>& matrices) const {
        const int location {get_uniform_location(name)};
        glUniformMatrix4fv(location, static_cast<int>(matrices.size()), GL_FALSE, glm::value_ptr(matrices.front()));
    }

    void GlShader::upload_uniform_int(Id name, int value) const {
        const int location {get_uniform_location(name)};
        glUniform1i(location, value);
    }

    void GlShader::upload_uniform_int_array(Id name, const std::vector<int>& values) const {
        const int location {get_uniform_location(name)};
        glUniform1iv(location, static_cast<int>(values.size()), values.data());
    }

    void GlShader::upload_uniform_float(Id name, float value) const {
        const int location {get_uniform_location(name)};
        glUniform1f(location, value);
    }

    void GlShader::upload_uniform_vec2(Id name, glm::vec2 vector) const {
        const int location {get_uniform_location(name)};
        glUniform2f(location, vector.x, vector.y);
    }

    void GlShader::upload_uniform_vec3(Id name, glm::vec3 vector) const {
        const int location {get_uniform_location(name)};
        glUniform3f(location, vector.x, vector.y, vector.z);
    }

    void GlShader::upload_uniform_vec3_array(Id name, const std::vector<glm::vec3>& vectors) const {
        const int location {get_uniform_location(name)};
        glUniform3fv(location, static_cast<int>(vectors.size()), glm::value_ptr(vectors.front()));
    }

    void GlShader::upload_uniform_vec4(Id name, glm::vec4 vector) const {
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
#ifndef SM_BUILD_DISTRIBUTION
        if (const auto iter {cache.find(name)}; iter == cache.cend()) {
            LOG_ERROR("Cannot get hashed uniform variable {} from program {}", static_cast<std::uint64_t>(name), program);
            return -1;
        } else {
            return iter->second;
        }
#else
        return cache.at(name);
#endif
    }

    void GlShader::check_and_cache_uniforms(const std::vector<std::string>& uniforms) {
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
                LOG_ERROR("Uniform variable `{}` in program {} not found", uniform, program);
                continue;
            }

            cache[Id(uniform)] = location;

            skip_uniform:
            continue;
        }
    }

    std::vector<std::string> GlShader::introspect_program() {
        std::vector<std::string> uniforms;

        // Uniforms stuff
        int uniform_count {};
        glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniform_count);

        uniforms.reserve(uniform_count);

        for (int i {0}; i < uniform_count; i++) {
            char buffer[64] {};
            glGetProgramResourceName(program, GL_UNIFORM, static_cast<unsigned int>(i), 64, nullptr, buffer);

            uniforms.push_back(buffer);
        }

        // Uniform blocks stuff
        int uniform_block_count {};
        glGetProgramInterfaceiv(program, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &uniform_block_count);

        uniform_blocks.reserve(uniform_block_count);

        for (int i {0}; i < uniform_block_count; i++) {
            UniformBlockSpecification block;
            std::size_t block_active_uniforms_count {};

            {
                char buffer[64] {};
                glGetProgramResourceName(program, GL_UNIFORM_BLOCK, static_cast<unsigned int>(i), 64, nullptr, buffer);

                block.block_name = buffer;
            }

            {
                int buffer[2] {};
                const GLenum properties[] { GL_BUFFER_BINDING, GL_NUM_ACTIVE_VARIABLES };
                glGetProgramResourceiv(program, GL_UNIFORM_BLOCK, static_cast<unsigned int>(i), 2, properties, 2, nullptr, buffer);

                block.binding_index = static_cast<unsigned int>(buffer[0]);
                block_active_uniforms_count = static_cast<std::size_t>(buffer[1]);
                block.uniforms.reserve(block_active_uniforms_count);
            }

            {
                const std::unique_ptr<int[]> buffer_uniforms {std::make_unique<int[]>(block_active_uniforms_count)};

                const GLenum properties[] { GL_ACTIVE_VARIABLES };
                glGetProgramResourceiv(
                    program,
                    GL_UNIFORM_BLOCK,
                    static_cast<unsigned int>(i),
                    1,
                    properties,
                    static_cast<int>(block_active_uniforms_count),
                    nullptr,
                    buffer_uniforms.get()
                );

                for (std::size_t j {0}; j < block_active_uniforms_count; j++) {
                    char buffer[64] {};
                    glGetProgramResourceName(program, GL_UNIFORM, buffer_uniforms[j], 64, nullptr, buffer);

                    block.uniforms.push_back(buffer);
                }
            }

            uniform_blocks.push_back(block);
        }

        return uniforms;
    }

    void GlShader::create_program(unsigned int vertex_shader, unsigned int fragment_shader) {
        assert(vertex_shader != 0);
        assert(fragment_shader != 0);

        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
        glValidateProgram(program);
    }

    void GlShader::delete_intermediates(unsigned int vertex_shader, unsigned int fragment_shader) {
        assert(program != 0);
        assert(vertex_shader != 0);
        assert(fragment_shader != 0);

        glDetachShader(program, vertex_shader);
        glDetachShader(program, fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    unsigned int GlShader::compile_shader(const std::string& source, unsigned int type) {
        const unsigned int shader {glCreateShader(type)};

        const char* const source_data {source.c_str()};
        const int source_length {static_cast<int>(source.size())};

        glShaderSource(shader, 1, &source_data, &source_length);
        glCompileShader(shader);

        if (!check_compilation(shader, type)) {
            SM_THROW_ERROR(ResourceError, "Could not compile shader {}", shader);
        }

        return shader;
    }

    bool GlShader::check_compilation(unsigned int shader, unsigned int type) {
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
                LOG_DIST_ERROR("{} shader compilation error with no message", type_name);
            } else {
                std::string log_message;
                log_message.resize(log_length);
                glGetShaderInfoLog(shader, log_length, nullptr, log_message.data());

                LOG_DIST_ERROR("{} shader compilation error\n{}", type_name, log_message);
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
                LOG_DIST_ERROR("Linking error with no message in program {}", program);
            } else {
                std::string log_message;
                log_message.resize(log_length);
                glGetProgramInfoLog(program, log_length, nullptr, log_message.data());

                LOG_DIST_ERROR("Linking error in program {}\n{}", program, log_message);
            }

            return false;
        }

        return true;
    }
}
