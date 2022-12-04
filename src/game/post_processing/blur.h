#pragma once

#include <nine_morris_3d_engine/engine_graphics.h>

class Blur : public PostProcessingStep {
public:
    Blur(std::string_view name, std::shared_ptr<gl::Framebuffer> framebuffer, std::shared_ptr<gl::Shader> shader)
        : PostProcessingStep(name, framebuffer, shader) {}
    virtual ~Blur() = default;

    virtual void render(const PostProcessingContext& context) const override;
    virtual void prepare(const PostProcessingContext& context) const override;
};
