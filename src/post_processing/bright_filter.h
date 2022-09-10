#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

class BrightFilter : public PostProcessingStep {
public:
    BrightFilter(std::string_view id, entt::resource_handle<Framebuffer> framebuffer, entt::resource_handle<Shader> shader)
        : PostProcessingStep(id, framebuffer, shader) {}
    virtual ~BrightFilter() = default;

    virtual void render(const PostProcessingContext& context) const override;
    virtual void prepare(const PostProcessingContext& context) const override;
};
