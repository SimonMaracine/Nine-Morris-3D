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
    // Pointers of post processing steps should be treated as resources
    class PostProcessingStep {
    public:
        PostProcessingStep(std::shared_ptr<GlFramebuffer> framebuffer, std::shared_ptr<GlShader> shader)
            : m_framebuffer(framebuffer), m_shader(shader) {}
        virtual ~PostProcessingStep() = default;

        PostProcessingStep(const PostProcessingStep&) = default;
        PostProcessingStep& operator=(const PostProcessingStep&) = default;
        PostProcessingStep(PostProcessingStep&&) = default;
        PostProcessingStep& operator=(PostProcessingStep&&) = default;

        // Called before rendering the to the framebuffer
        virtual void setup(const PostProcessingCtx& ctx) const = 0;

        static void bind_texture(unsigned int texture, int unit);
    protected:
        std::shared_ptr<GlFramebuffer> m_framebuffer;
        std::shared_ptr<GlShader> m_shader;

        friend class internal::Renderer;
    };
}
