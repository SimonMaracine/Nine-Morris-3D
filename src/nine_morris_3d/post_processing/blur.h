#pragma once

#include "graphics/renderer/renderer.h"
#include "graphics/renderer/post_processing_step.h"
#include "graphics/renderer/opengl/framebuffer.h"
#include "graphics/renderer/opengl/shader.h"

class Blur : public PostProcessingStep {
public:
    Blur(std::string_view id, std::shared_ptr<Framebuffer> framebuffer, std::shared_ptr<Shader> shader)
        : PostProcessingStep(id, framebuffer, shader) {}
    virtual ~Blur() = default;

    virtual void render(const PostProcessingContext& context) const override;
    virtual void prepare(const PostProcessingContext& context) const override;
};
