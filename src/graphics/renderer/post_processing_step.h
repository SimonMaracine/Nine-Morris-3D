#pragma once

#include "graphics/renderer/renderer.h"
#include "graphics/renderer/opengl/framebuffer.h"
#include "graphics/renderer/opengl/shader.h"

class PostProcessingStep {
public:
    PostProcessingStep(std::shared_ptr<Framebuffer> framebuffer, std::shared_ptr<Shader> shader)
        : framebuffer(framebuffer), shader(shader) {}
    virtual ~PostProcessingStep() = default;

    virtual void render(const PostProcessingContext& context) const = 0;
    virtual void prepare(const PostProcessingContext& context) const = 0;

    bool enabled = true;
protected:
    std::shared_ptr<Framebuffer> framebuffer;
    std::shared_ptr<Shader> shader;

    friend class Renderer;
};
