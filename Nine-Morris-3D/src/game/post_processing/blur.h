#pragma once

#include <engine/public/graphics.h>

class Blur : public sm::PostProcessingStep {
public:
    Blur(std::string_view name, std::shared_ptr<sm::gl::Framebuffer> framebuffer, std::shared_ptr<sm::gl::Shader> shader)
        : PostProcessingStep(name, framebuffer, shader) {}
    virtual ~Blur() = default;

    virtual void render(const sm::PostProcessingContext& context) const override;
    virtual void prepare(const sm::PostProcessingContext& context) const override;
};
