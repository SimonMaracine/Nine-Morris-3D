#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "graphics/renderer/buffer.h"

class Shader {
public:
    Shader(GLuint program, GLuint vertex_shader, GLuint fragment_shader, const std::string& name,
            const std::vector<std::string>& uniforms, const std::string& vertex_source_path,
            const std::string& fragment_source_path);
    ~Shader();

    static std::shared_ptr<Shader> create(const std::string& vertex_source_path,
            const std::string& fragment_source_path, const std::vector<std::string>& uniforms);
    static std::shared_ptr<Shader> create(const std::string& vertex_source,
            const std::string& fragment_source, const std::vector<std::string>& uniforms,
            const char* block_name, int uniforms_count, std::shared_ptr<UniformBuffer> uniform_buffer);

    void bind() const;
    static void unbind();

    void set_uniform_matrix(const std::string& name, const glm::mat4& matrix) const;
    void set_uniform_int(const std::string& name, int value) const;
    void set_uniform_vec2(const std::string& name, const glm::vec2& vector) const;
    void set_uniform_vec3(const std::string& name, const glm::vec3& vector) const;
    void set_uniform_vec4(const std::string& name, const glm::vec4& vector) const;
    void set_uniform_float(const std::string& name, float value) const;

    void recompile();
private:
    GLint get_uniform_location(const std::string& name) const;

    static GLuint compile_shader(const std::string& source_path, GLenum type);
    static bool check_compilation(GLuint shader, GLenum type);
    static bool check_linking(GLuint program);
    static std::string get_name(const std::string& vertex_source, const std::string& fragment_source);

    GLuint program = 0;
    GLuint vertex_shader = 0;
    GLuint fragment_shader = 0;

    std::string name;

    mutable std::unordered_map<std::string, GLint> cache;

    // Keep these for hot-reloading functionality
    std::string vertex_source_path;
    std::string fragment_source_path;
};
