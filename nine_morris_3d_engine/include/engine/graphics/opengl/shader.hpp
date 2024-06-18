#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cstddef>
#include <memory>

#include <glm/glm.hpp>

#include "engine/application_base/id.hpp"
#include "engine/graphics/opengl/buffer.hpp"

namespace sm {
    class Renderer;

    class GlShader {
    public:
        GlShader(const std::string& source_vertex, const std::string& source_fragment);
        ~GlShader();

        GlShader(const GlShader&) = delete;
        GlShader& operator=(const GlShader&) = delete;
        GlShader(GlShader&&) = delete;
        GlShader& operator=(GlShader&&) = delete;

        void bind() const;
        static void unbind();

        void upload_uniform_mat4(Id name, const glm::mat4& matrix) const;
        void upload_uniform_int(Id name, int value) const;
        void upload_uniform_float(Id name, float value) const;
        void upload_uniform_vec2(Id name, glm::vec2 vector) const;
        void upload_uniform_vec3(Id name, const glm::vec3& vector) const;
        void upload_uniform_vec4(Id name, const glm::vec4& vector) const;

        unsigned int get_id() const { return program; }

        void add_uniform_buffer(std::shared_ptr<GlUniformBuffer> uniform_buffer);
    private:
        int get_uniform_location(Id name) const;
        void check_and_cache_uniforms();

        void introspect_program();

        unsigned int create_program() const;
        void delete_intermediates();
        unsigned int compile_shader(const std::string& source, unsigned int type) const;
        bool check_compilation(unsigned int shader, unsigned int type) const;
        bool check_linking(unsigned int program) const;

        unsigned int program {};
        unsigned int vertex_shader {};
        unsigned int fragment_shader {};

        // Uniforms cache
        std::unordered_map<Id, int, Hash> cache;

        // Data from introspection
        std::vector<std::string> uniforms;
        std::vector<UniformBlockSpecification> uniform_blocks;

        // Shaders own uniform buffers
        std::vector<std::shared_ptr<GlUniformBuffer>> uniform_buffers;

        friend class Renderer;
    };
}
