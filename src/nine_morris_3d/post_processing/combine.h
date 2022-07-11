#pragma once

#include "graphics/renderer/renderer.h"
#include "graphics/renderer/post_processing_step.h"
#include "graphics/renderer/opengl/framebuffer.h"
#include "graphics/renderer/opengl/shader.h"

class Combine : public PostProcessingStep {
public:
    Combine(std::shared_ptr<Framebuffer> framebuffer, std::shared_ptr<Shader> shader, unsigned int screen_texture)
        : PostProcessingStep(framebuffer, shader), screen_texture(screen_texture) {}
    virtual ~Combine() = default;

    virtual void render(const PostProcessingContext& context) const override;
    virtual void prepare(const PostProcessingContext& context) const override;
private:
    unsigned int screen_texture = 0;
};
