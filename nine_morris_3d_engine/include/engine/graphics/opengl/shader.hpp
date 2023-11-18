#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <cstddef>
#include <utility>
#include <memory>

#include <glm/glm.hpp>
#include <resmanager/resmanager.hpp>

#include "engine/graphics/opengl/buffer.hpp"
#include "engine/other/encrypt.hpp"

namespace sm {
    class Renderer;

    class GlShader {
    public:
        using Key = resmanager::HashedStr64;
        using KeyHash = resmanager::Hash<Key>;

        GlShader(const std::string& source_vertex, const std::string& source_fragment);
        GlShader(const EncrFile& source_vertex, const EncrFile& source_fragment);
        ~GlShader();

        GlShader(const GlShader&) = delete;
        GlShader& operator=(const GlShader&) = delete;
        GlShader(GlShader&&) = delete;
        GlShader& operator=(GlShader&&) = delete;

        void bind() const;
        static void unbind();

        void upload_uniform_mat4(Key name, const glm::mat4& matrix) const;
        void upload_uniform_int(Key name, int value) const;
        void upload_uniform_float(Key name, float value) const;
        void upload_uniform_vec2(Key name, glm::vec2 vector) const;
        void upload_uniform_vec3(Key name, const glm::vec3& vector) const;
        void upload_uniform_vec4(Key name, const glm::vec4& vector) const;

        unsigned int get_id() const { return program; }
        const std::string& get_name() const { return name; }

        void add_uniform_buffer(std::shared_ptr<GlUniformBuffer> uniform_buffer);
    private:
        int get_uniform_location(Key name) const;
        void check_and_cache_uniforms();

        void introspect_program();

        unsigned int create_program() const;
        void delete_intermediates();
        std::optional<unsigned int> compile_shader(const std::string& source_path, unsigned int type) const;
        std::optional<unsigned int> compile_shader(const std::pair<unsigned char*, std::size_t>& source_buffer, unsigned int type) const;
        bool check_compilation(unsigned int shader, unsigned int type) const;
        bool check_linking(unsigned int program) const;

        unsigned int program {0};
        unsigned int vertex_shader {0};
        unsigned int fragment_shader {0};

        std::string name;

        // Uniforms cache
        std::unordered_map<Key, int, KeyHash> cache;

        // Data from introspection
        std::vector<std::string> uniforms;
        std::vector<UniformBlockSpecification> uniform_blocks;

        // Shaders own uniform buffers
        std::vector<std::shared_ptr<GlUniformBuffer>> uniform_buffers;

        friend class Renderer;
    };
}
