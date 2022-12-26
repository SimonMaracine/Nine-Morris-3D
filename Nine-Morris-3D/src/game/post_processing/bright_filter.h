#pragma once

#include <engine/engine_graphics.h>

class BrightFilter : public PostProcessingStep {
public:
    BrightFilter(std::string_view name, std::shared_ptr<gl::Framebuffer> framebuffer, std::shared_ptr<gl::Shader> shader)
        : PostProcessingStep(name, framebuffer, shader) {}
    virtual ~BrightFilter() = default;

    virtual void render(const PostProcessingContext& context) const override;
    virtual void prepare(const PostProcessingContext& context) const override;
};
