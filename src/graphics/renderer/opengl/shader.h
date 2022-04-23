#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "graphics/renderer/opengl/buffer.h"

struct UniformBlockSpecification {
    std::string block_name;
    unsigned int field_count;
    const char** field_names;
    std::shared_ptr<UniformBuffer> uniform_buffer;
    GLuint binding_index;
};

/**
 * This represents OpenGL shader programs.
 */
class Shader {
public:
    Shader(GLuint program, GLuint vertex_shader, GLuint fragment_shader, const std::string& name,
            const std::vector<std::string>& uniforms, const std::string& vertex_source_path,
            const std::string& fragment_source_path);
    ~Shader();

    static std::shared_ptr<Shader> create(const std::string& vertex_source_path,
            const std::string& fragment_source_path, const std::vector<std::string>& uniforms);
    static std::shared_ptr<Shader> create(const std::string& vertex_source_path,
            const std::string& fragment_source_path, const std::vector<std::string>& uniforms,
            const std::vector<UniformBlockSpecification>& uniform_blocks);

    void bind();
    static void unbind();

    void set_uniform_mat4(const std::string& name, const glm::mat4& matrix);
    void set_uniform_int(const std::string& name, int value);
    void set_uniform_float(const std::string& name, float value);
    void set_uniform_vec2(const std::string& name, const glm::vec2& vector);
    void set_uniform_vec3(const std::string& name, const glm::vec3& vector);
    void set_uniform_vec4(const std::string& name, const glm::vec4& vector);

    // Make sure to reupload any uniforms that need to after calling this function
    void recompile();  // FIXME this should update the uniform indices!!!

    const std::string& get_name() { return name; }
private:
    GLint get_uniform_location(const std::string& name);

    static GLuint compile_shader(const std::string& source_path, GLenum type) noexcept(false);
    static bool check_compilation(GLuint shader, GLenum type);
    static bool check_linking(GLuint program);
    static std::string get_name(const std::string& vertex_source, const std::string& fragment_source);

    GLuint program = 0;
    GLuint vertex_shader = 0;
    GLuint fragment_shader = 0;

    std::string name;

    // Uniforms cache
    std::unordered_map<std::string, GLint> cache;

    // Keep these for hot-reloading functionality
    std::string vertex_source_path;
    std::string fragment_source_path;
};
