#pragma once

#include "graphics/renderer/renderer.h"
#include "graphics/renderer/opengl/framebuffer.h"
#include "graphics/renderer/opengl/shader.h"

class PostProcessingStep {
public:
    PostProcessingStep(std::string_view id, std::shared_ptr<Framebuffer> framebuffer, std::shared_ptr<Shader> shader)
        : id(id), framebuffer(framebuffer), shader(shader) {}
    virtual ~PostProcessingStep() = default;

    virtual void render(const PostProcessingContext& context) const = 0;
    virtual void prepare(const PostProcessingContext& context) const = 0;

    std::string_view get_id() const { return id; }

    bool enabled = true;
protected:
    std::string id;

    std::shared_ptr<Framebuffer> framebuffer;
    std::shared_ptr<Shader> shader;

    friend class Renderer;
};
