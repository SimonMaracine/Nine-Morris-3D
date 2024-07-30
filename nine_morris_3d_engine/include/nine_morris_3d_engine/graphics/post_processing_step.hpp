#pragma once

#include <memory>

#include "nine_morris_3d_engine/graphics/internal/post_processing_context.hpp"
#include "nine_morris_3d_engine/graphics/opengl/framebuffer.hpp"
#include "nine_morris_3d_engine/graphics/opengl/shader.hpp"

namespace sm {
    namespace internal {
        class Renderer;
    }

    using PostProcessingCtx = internal::PostProcessingContext;

    // Extend this class for every step of the post processing pipeline
    // Pointers of post processing steps should be retained as a resource
    class PostProcessingStep {
    public:
        PostProcessingStep(std::shared_ptr<GlFramebuffer> framebuffer, std::shared_ptr<GlShader> shader) noexcept
            : m_framebuffer(framebuffer), m_shader(shader) {}
        virtual ~PostProcessingStep() = default;

        PostProcessingStep(const PostProcessingStep&) noexcept = default;
        PostProcessingStep& operator=(const PostProcessingStep&) noexcept = default;
        PostProcessingStep(PostProcessingStep&&) noexcept = default;
        PostProcessingStep& operator=(PostProcessingStep&&) noexcept = default;

        virtual void setup(const PostProcessingCtx& ctx) const = 0;

        static void bind_texture(unsigned int texture, int unit) noexcept;
    protected:
        std::shared_ptr<GlFramebuffer> m_framebuffer;
        std::shared_ptr<GlShader> m_shader;

        friend class internal::Renderer;
    };
}
