#pragma once

#include "nine_morris_3d_engine/graphics/renderer/renderer.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/framebuffer.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/shader.h"

class PostProcessingStep {
public:
    PostProcessingStep(std::string_view name, std::shared_ptr<Framebuffer> framebuffer, std::shared_ptr<Shader> shader)
        : name(name), framebuffer(framebuffer), shader(shader) {}
    virtual ~PostProcessingStep() = default;

    virtual void render(const PostProcessingContext& context) const = 0;
    virtual void prepare(const PostProcessingContext& context) const = 0;

    std::string_view get_name() const { return name; }

    bool enabled = true;
protected:
    std::string name;

    std::shared_ptr<Framebuffer> framebuffer;
    std::shared_ptr<Shader> shader;

    friend class Renderer;
};
