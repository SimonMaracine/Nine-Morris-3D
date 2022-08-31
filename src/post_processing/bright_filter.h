#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

// #include "graphics/renderer/renderer.h"
// #include "graphics/renderer/post_processing_step.h"
// #include "graphics/renderer/opengl/framebuffer.h"
// #include "graphics/renderer/opengl/shader.h"

class BrightFilter : public PostProcessingStep {
public:
    BrightFilter(std::string_view id, std::shared_ptr<Framebuffer> framebuffer, std::shared_ptr<Shader> shader)
        : PostProcessingStep(id, framebuffer, shader) {}
    virtual ~BrightFilter() = default;

    virtual void render(const PostProcessingContext& context) const override;
    virtual void prepare(const PostProcessingContext& context) const override;
};
