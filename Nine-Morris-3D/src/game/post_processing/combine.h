#pragma once

#include <engine/public/graphics.h>

class Combine : public PostProcessingStep {
public:
    Combine(std::string_view name, std::shared_ptr<gl::Framebuffer> framebuffer, std::shared_ptr<gl::Shader> shader)
        : PostProcessingStep(name, framebuffer, shader) {}
    virtual ~Combine() = default;

    virtual void render(const PostProcessingContext& context) const override;
    virtual void prepare(const PostProcessingContext& context) const override;

    float strength = 0.0f;
};
