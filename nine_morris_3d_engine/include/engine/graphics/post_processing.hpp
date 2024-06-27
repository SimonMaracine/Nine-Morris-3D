#pragma once

#include <memory>
#include <vector>
#include <utility>

#include "engine/graphics/opengl/framebuffer.hpp"
#include "engine/graphics/opengl/shader.hpp"

namespace sm {
    class Renderer;
    struct PostProcessingContext;

    class PostProcessingStep {
    public:
        PostProcessingStep(std::shared_ptr<GlFramebuffer> framebuffer, std::unique_ptr<GlShader>&& shader)
            : framebuffer(framebuffer), shader(std::move(shader)) {}
        virtual ~PostProcessingStep() = default;

        PostProcessingStep(const PostProcessingStep&) = default;
        PostProcessingStep& operator=(const PostProcessingStep&) = default;
        PostProcessingStep(PostProcessingStep&&) noexcept = default;
        PostProcessingStep& operator=(PostProcessingStep&&) noexcept = default;

        virtual void setup(const PostProcessingContext& context) const = 0;
    protected:
        std::shared_ptr<GlFramebuffer> framebuffer;
        std::unique_ptr<GlShader> shader;

        friend class Renderer;
    };

    struct PostProcessingContext {
        std::vector<std::unique_ptr<PostProcessingStep>> steps;
        std::vector<unsigned int> textures;  // All textures in order
        unsigned int last_texture {};  // Last texture at any moment in the processing pipeline
        unsigned int original_texture {};
    };
}
