#pragma once

#include <engine/public/graphics.h>

class Combine : public sm::PostProcessingStep {
public:
    Combine(std::string_view name, std::shared_ptr<sm::gl::Framebuffer> framebuffer, std::shared_ptr<sm::gl::Shader> shader)
        : PostProcessingStep(name, framebuffer, shader) {}
    virtual ~Combine() = default;

    virtual void render(const sm::PostProcessingContext& context) const override;
    virtual void prepare(const sm::PostProcessingContext& context) const override;

    float strength = 0.0f;
};
