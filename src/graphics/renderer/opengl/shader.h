#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cppblowfish.h>

#include "graphics/renderer/opengl/buffer.h"
#include "other/encryption.h"

struct UniformBlockSpecification {
    std::string block_name;
    unsigned int field_count;
    const char** field_names;
    std::shared_ptr<UniformBuffer> uniform_buffer;
    GLuint binding_index;
};

class Shader {
public:
    Shader(
        GLuint program,
        GLuint vertex_shader,
        GLuint fragment_shader,
        std::string_view name,
        const std::vector<std::string>& uniforms,
        std::string_view vertex_source_path,
        std::string_view fragment_source_path);
    ~Shader();

    static std::shared_ptr<Shader> create(
            std::string_view vertex_source_path,
            std::string_view fragment_source_path,
            const std::vector<std::string>& uniforms);
    static std::shared_ptr<Shader> create(
            std::string_view vertex_source_path,
            std::string_view fragment_source_path,
            const std::vector<std::string>& uniforms,
            const std::vector<UniformBlockSpecification>& uniform_blocks);
    static std::shared_ptr<Shader> create(
            const encryption::EncryptedFile& vertex_source_path,
            const encryption::EncryptedFile& fragment_source_path,
            const std::vector<std::string>& uniforms);
    static std::shared_ptr<Shader> create(
            const encryption::EncryptedFile& vertex_source_path,
            const encryption::EncryptedFile& fragment_source_path,
            const std::vector<std::string>& uniforms,
            const std::vector<UniformBlockSpecification>& uniform_blocks);

    void bind();
    static void unbind();

    void upload_uniform_mat4(std::string_view name, const glm::mat4& matrix);
    void upload_uniform_int(std::string_view name, int value);
    void upload_uniform_float(std::string_view name, float value);
    void upload_uniform_vec2(std::string_view name, glm::vec2 vector);
    void upload_uniform_vec3(std::string_view name, const glm::vec3& vector);
    void upload_uniform_vec4(std::string_view name, const glm::vec4& vector);

    // Make sure to reupload any uniforms that need to after calling this function
    void recompile();

    std::string_view get_name() { return name; }
    std::vector<std::string>& get_uniforms() { return uniforms; }
private:
    GLint get_uniform_location(std::string_view name);

    static GLuint compile_shader(std::string_view source_path, GLenum type, std::string_view name) noexcept(false);
    static GLuint compile_shader(const cppblowfish::Buffer& source_buffer, GLenum type, std::string_view name) noexcept(false);
    static bool check_compilation(GLuint shader, GLenum type, std::string_view name) noexcept(false);
    static bool check_linking(GLuint program, std::string_view name);
    static std::string get_name(std::string_view vertex_source, std::string_view fragment_source);

    GLuint program = 0;
    GLuint vertex_shader = 0;
    GLuint fragment_shader = 0;

    std::string name;

    // Uniforms cache
    std::unordered_map<std::string, GLint> cache;

    // Keep these for hot-reloading functionality
    std::string vertex_source_path;
    std::string fragment_source_path;
    std::vector<std::string> uniforms;
};
