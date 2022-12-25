#pragma once

#include <glad/glad.h>

#include "engine/graphics/opengl/framebuffer.h"
#include "engine/graphics/opengl/shader.h"

struct PostProcessingContext;

class PostProcessingStep {
public:
    PostProcessingStep(std::string_view name, std::shared_ptr<gl::Framebuffer> framebuffer, std::shared_ptr<gl::Shader> shader)
        : name(name), framebuffer(framebuffer), shader(shader) {}
    virtual ~PostProcessingStep() = default;

    PostProcessingStep(const PostProcessingStep&) = delete;
    PostProcessingStep& operator=(const PostProcessingStep&) = delete;
    PostProcessingStep(PostProcessingStep&&) = delete;
    PostProcessingStep& operator=(PostProcessingStep&&) = delete;

    virtual void render(const PostProcessingContext& context) const = 0;
    virtual void prepare(const PostProcessingContext& context) const = 0;

    std::string_view get_name() const { return name; }
protected:
    std::string name;

    // Store references to shaders and framebuffers
    std::shared_ptr<gl::Framebuffer> framebuffer;
    std::shared_ptr<gl::Shader> shader;

    friend class Renderer;
};

struct PostProcessingContext {
    std::vector<std::unique_ptr<PostProcessingStep>> steps;
    GLuint last_texture = 0;  // Last texture at any moment in the processing pipeline
    std::vector<GLuint> textures;  // All textures in order
    GLuint original_texture = 0;
};
