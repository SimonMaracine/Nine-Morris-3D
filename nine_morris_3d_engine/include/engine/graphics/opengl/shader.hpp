#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <optional>
#include <cstddef>
#include <utility>

#include <glm/glm.hpp>
#include <cppblowfish/cppblowfish.hpp>
#include <resmanager/resmanager.hpp>

#include "engine/graphics/opengl/buffer.hpp"
#include "engine/other/encrypt.hpp"

namespace sm {
    class GlShader {
    public:
        using Key = resmanager::HashedStr64;
        using KeyHash = resmanager::Hash<Key>;

        GlShader(std::string_view source_vertex, std::string_view source_fragment);
        GlShader(Encrypt::EncryptedFile source_vertex, Encrypt::EncryptedFile source_fragment);
        ~GlShader();

        GlShader(const GlShader&) = delete;
        GlShader& operator=(const GlShader&) = delete;
        GlShader(GlShader&&) = delete;
        GlShader& operator=(GlShader&&) = delete;

        void bind();
        static void unbind();

        void upload_uniform_mat4(Key name, const glm::mat4& matrix);
        void upload_uniform_int(Key name, int value);
        void upload_uniform_float(Key name, float value);
        void upload_uniform_vec2(Key name, glm::vec2 vector);
        void upload_uniform_vec3(Key name, const glm::vec3& vector);
        void upload_uniform_vec4(Key name, const glm::vec4& vector);

        std::string_view get_name() { return name; }
    private:
        int get_uniform_location(Key name) const;
        void check_and_cache_uniforms();

        void introspect_program();

        unsigned int create_program();
        void delete_intermediates();
        std::optional<unsigned int> compile_shader(std::string_view source_path, unsigned int type);
        std::optional<unsigned int> compile_shader(const std::pair<unsigned char*, std::size_t>& source_buffer, unsigned int type);
        bool check_compilation(unsigned int shader, unsigned int type);
        bool check_linking(unsigned int program);

        unsigned int program = 0;
        unsigned int vertex_shader = 0;
        unsigned int fragment_shader = 0;

        std::string name;

        // Uniforms cache
        std::unordered_map<Key, int, KeyHash> cache;

        // Data from introspection
        struct UniformBlock {
            std::string name;
            unsigned int binding_index {};
        };

        std::vector<std::string> uniforms;
        std::vector<UniformBlock> uniform_blocks;
        std::vector<std::string> uniforms_in_blocks;
    };
}
