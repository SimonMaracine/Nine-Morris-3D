#pragma once

#include <engine/public/graphics.h>

class BrightFilter : public sm::PostProcessingStep {
public:
    BrightFilter(std::string_view name, std::shared_ptr<sm::gl::Framebuffer> framebuffer, std::shared_ptr<sm::gl::Shader> shader)
        : PostProcessingStep(name, framebuffer, shader) {}
    virtual ~BrightFilter() = default;

    virtual void render(const sm::PostProcessingContext& context) const override;
    virtual void prepare(const sm::PostProcessingContext& context) const override;
};
