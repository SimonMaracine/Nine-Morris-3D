#pragma once

#include <glad/glad.h>

#include "engine/graphics/opengl/framebuffer.h"

namespace render_helpers {
    enum {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT
    };

    void clear(int buffers);
    void viewport(int width, int height);
    void viewport(const gl::FramebufferSpecification& specification);
    void clear_color(float red, float green, float blue);
    void bind_texture_2d(GLuint texture, int unit);
    void draw_arrays(int count);
}
