#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/id.hpp"
#include "nine_morris_3d_engine/graphics/opengl/buffer.hpp"

// setting uniforms https://www.khronos.org/opengl/wiki/Program_Introspection#Naming

namespace sm {
    namespace internal {
        class Renderer;
    }

    // OpenGL resource representing a shader program
    class GlShader {
    public:
        GlShader(const std::string& source_vertex, const std::string& source_fragment);
        GlShader(const std::string& source_vertex, const std::string& source_geometry, const std::string& source_fragment);
        ~GlShader();

        GlShader(const GlShader&) = delete;
        GlShader& operator=(const GlShader&) = delete;
        GlShader(GlShader&&) = delete;
        GlShader& operator=(GlShader&&) = delete;

        void bind() const;
        static void unbind();

        void upload_uniform_mat3(Id name, const glm::mat3& matrix) const;
        void upload_uniform_mat3_array(Id name, const std::vector<glm::mat3>& matrices) const;
        void upload_uniform_mat4(Id name, const glm::mat4& matrix) const;
        void upload_uniform_mat4_array(Id name, const std::vector<glm::mat4>& matrices) const;
        void upload_uniform_int(Id name, int value) const;
        void upload_uniform_int_array(Id name, const std::vector<int>& values) const;
        void upload_uniform_float(Id name, float value) const;
        void upload_uniform_vec2(Id name, glm::vec2 vector) const;
        void upload_uniform_vec3(Id name, glm::vec3 vector) const;
        void upload_uniform_vec3_array(Id name, const std::vector<glm::vec3>& vectors) const;
        void upload_uniform_vec4(Id name, glm::vec4 vector) const;

        unsigned int get_id() const;

        void add_uniform_buffer(std::shared_ptr<GlUniformBuffer> uniform_buffer);
    private:
        int get_uniform_location(Id name) const;
        void check_and_cache_uniforms(const std::vector<std::string>& uniforms);

        std::vector<std::string> introspect_program();

        void create_program(unsigned int vertex_shader, unsigned int fragment_shader);
        void create_program(unsigned int vertex_shader, unsigned int geometry_shader, unsigned int fragment_shader);
        void delete_intermediates(unsigned int vertex_shader, unsigned int fragment_shader);
        void delete_intermediates(unsigned int vertex_shader, unsigned int geometry_shader, unsigned int fragment_shader);
        static unsigned int compile_shader(const std::string& source, unsigned int type);
        static bool check_compilation(unsigned int shader, unsigned int type);
        bool check_linking(unsigned int program) const;

        unsigned int m_program {};

        // Uniforms cache
        std::unordered_map<Id, int, Hash> m_cache;

        // Data from introspection; needed by renderer
        std::vector<UniformBlockSpecification> m_uniform_blocks;

        std::vector<std::shared_ptr<GlUniformBuffer>> m_uniform_buffers;

        friend class internal::Renderer;
    };
}
