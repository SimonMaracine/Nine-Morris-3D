#pragma once

#include <glad/glad.h>

namespace gl {
    struct FramebufferSpecification;
}

namespace render_helpers {
    enum {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT
    };

    void clear(int buffers);
    void clear_color(float red, float green, float blue);

    void viewport(int width, int height);
    void viewport(const gl::FramebufferSpecification& specification);

    void bind_texture_2d(GLuint texture, int unit);

    void draw_arrays(int count);
    void draw_arrays_lines(int count);
    void draw_elements(int count);
    void draw_elements_instanced(int count, int instance_count);

    void disable_depth_test();
    void enable_depth_test();

    void disable_blending();
    void enable_blending();
}
