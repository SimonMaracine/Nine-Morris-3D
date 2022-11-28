#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

class Blur : public PostProcessingStep {
public:
    Blur(std::string_view name, std::shared_ptr<Framebuffer> framebuffer, std::shared_ptr<Shader> shader)
        : PostProcessingStep(name, framebuffer, shader) {}
    virtual ~Blur() = default;

    virtual void render(const PostProcessingContext& context) const override;
    virtual void prepare(const PostProcessingContext& context) const override;
};
