#pragma once

#include <memory>
#include <vector>

#include "engine/graphics/opengl/framebuffer.hpp"
#include "engine/graphics/opengl/shader.hpp"

namespace sm {
    struct PostProcessingContext;

    class PostProcessingStep {
    public:
        PostProcessingStep(std::shared_ptr<GlFramebuffer> framebuffer, std::shared_ptr<gl::Shader> shader)
            : framebuffer(framebuffer), shader(shader) {}
        virtual ~PostProcessingStep() = default;

        PostProcessingStep(const PostProcessingStep&) = delete;
        PostProcessingStep& operator=(const PostProcessingStep&) = delete;
        PostProcessingStep(PostProcessingStep&&) = delete;
        PostProcessingStep& operator=(PostProcessingStep&&) = delete;

        virtual void render(const PostProcessingContext& context) const = 0;
        virtual void prepare(const PostProcessingContext& context) const = 0;
    protected:
        // Store references to shaders and framebuffers
        std::shared_ptr<GlFramebuffer> framebuffer;
        std::shared_ptr<gl::Shader> shader;

        friend class Renderer;
    };

    struct PostProcessingContext {
        std::vector<std::unique_ptr<PostProcessingStep>> steps;
        unsigned int last_texture = 0;  // Last texture at any moment in the processing pipeline
        std::vector<unsigned int> textures;  // All textures in order
        unsigned int original_texture = 0;
    };
}
