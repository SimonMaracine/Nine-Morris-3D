#pragma once

#include "nine_morris_3d_engine/graphics/renderer/renderer.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/framebuffer.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/shader.h"

class PostProcessingStep {
public:
    PostProcessingStep(std::string_view id, entt::resource_handle<Framebuffer> framebuffer, entt::resource_handle<Shader> shader)
        : id(id), framebuffer(framebuffer), shader(shader) {}
    virtual ~PostProcessingStep() = default;

    virtual void render(const PostProcessingContext& context) const = 0;
    virtual void prepare(const PostProcessingContext& context) const = 0;

    std::string_view get_id() const { return id; }

    bool enabled = true;
protected:
    std::string id;

    entt::resource_handle<Framebuffer> framebuffer;
    entt::resource_handle<Shader> shader;

    friend class Renderer;
};
