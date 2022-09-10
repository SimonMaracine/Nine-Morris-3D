#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

class Blur : public PostProcessingStep {
public:
    Blur(std::string_view id, entt::resource_handle<Framebuffer> framebuffer, entt::resource_handle<Shader> shader)
        : PostProcessingStep(id, framebuffer, shader) {}
    virtual ~Blur() = default;

    virtual void render(const PostProcessingContext& context) const override;
    virtual void prepare(const PostProcessingContext& context) const override;
};
