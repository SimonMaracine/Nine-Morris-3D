#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cppblowfish/cppblowfish.h>
#include <resmanager/resmanager.h>

#include "engine/graphics/opengl/buffer.h"
#include "engine/other/encrypt.h"

namespace gl {
    struct UniformBlockSpecification {
        std::string block_name;
        std::vector<std::string> field_names;  // Must be defined in the order that they appear in shader
        std::shared_ptr<UniformBuffer> uniform_buffer;
        GLuint binding_index = 0;
    };

    using UniformBlocks = std::initializer_list<UniformBlockSpecification>;

    class Shader {
    private:
        using Key = resmanager::HashedStr64;
        using HashFunction = resmanager::Hash<Key>;
    public:
        Shader(std::string_view vertex_source, std::string_view fragment_source,
            const std::vector<std::string>& uniforms, UniformBlocks uniform_blocks = {});
        Shader(encrypt::EncryptedFile vertex_source, encrypt::EncryptedFile fragment_source,
            const std::vector<std::string>& uniforms, UniformBlocks uniform_blocks = {});
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        Shader(Shader&&) = delete;
        Shader& operator=(Shader&&) = delete;

        void bind();
        static void unbind();

        void upload_uniform_mat4(Key name, const glm::mat4& matrix);
        void upload_uniform_int(Key name, int value);
        void upload_uniform_float(Key name, float value);
        void upload_uniform_vec2(Key name, glm::vec2 vector);
        void upload_uniform_vec3(Key name, const glm::vec3& vector);
        void upload_uniform_vec4(Key name, const glm::vec4& vector);

        // Make sure to reupload any uniforms that need to after calling this function
        void recompile();

        std::string_view get_name() { return name; }
        const std::vector<std::string>& get_uniforms() { return uniforms; }
    private:
        GLint get_uniform_location(Key name);
        void check_and_cache_uniforms(const std::vector<std::string>& uniforms);

        static void configure_uniform_blocks(GLuint program, const UniformBlocks& uniform_blocks);

        GLuint program = 0;
        GLuint vertex_shader = 0;
        GLuint fragment_shader = 0;

        std::string name;

        // Uniforms cache
        std::unordered_map<Key, GLint, HashFunction> cache;

        // Keep these for hot-reloading functionality
        std::string vertex_source_path;
        std::string fragment_source_path;
        std::vector<std::string> uniforms;
    };
}
