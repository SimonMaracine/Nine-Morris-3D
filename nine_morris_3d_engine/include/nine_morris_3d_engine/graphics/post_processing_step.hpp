#pragma once

#include <memory>

#include "nine_morris_3d_engine/graphics/opengl/framebuffer.hpp"
#include "nine_morris_3d_engine/graphics/opengl/shader.hpp"

namespace sm {
    namespace internal {
        class Renderer;
        class PostProcessingContext;
    }

    // Extend this class for every step of the post processing pipeline
    // Pointers of post processing steps should be retained as a resource
    class PostProcessingStep {
    public:
        PostProcessingStep(std::shared_ptr<GlFramebuffer> framebuffer, std::shared_ptr<GlShader> shader)
            : framebuffer(framebuffer), shader(shader) {}
        virtual ~PostProcessingStep() = default;

        PostProcessingStep(const PostProcessingStep&) = default;
        PostProcessingStep& operator=(const PostProcessingStep&) = default;
        PostProcessingStep(PostProcessingStep&&) noexcept = default;
        PostProcessingStep& operator=(PostProcessingStep&&) noexcept = default;

        virtual void setup(const internal::PostProcessingContext& context) const = 0;
    protected:
        std::shared_ptr<GlFramebuffer> framebuffer;
        std::shared_ptr<GlShader> shader;

        friend class internal::Renderer;
    };
}
