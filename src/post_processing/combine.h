#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

// #include "graphics/renderer/renderer.h"
// #include "graphics/renderer/post_processing_step.h"
// #include "graphics/renderer/opengl/framebuffer.h"
// #include "graphics/renderer/opengl/shader.h"

class Combine : public PostProcessingStep {
public:
    Combine(std::string_view id, std::shared_ptr<Framebuffer> framebuffer, std::shared_ptr<Shader> shader)
        : PostProcessingStep(id, framebuffer, shader) {}
    virtual ~Combine() = default;

    virtual void render(const PostProcessingContext& context) const override;
    virtual void prepare(const PostProcessingContext& context) const override;

    float strength = 0.0f;
};
