#include <glad/glad.h>

#include "engine/graphics/opengl/framebuffer.h"

namespace render_helpers {
    void clear(int buffers) {
        glClear(buffers);
    }

    void viewport(int width, int height) {
        glViewport(0, 0, width, height);
    }

    void viewport(const gl::FramebufferSpecification& specification) {
        glViewport(
            0,
            0,
            specification.width,
            specification.height
        );
    }

    void clear_color(float red, float green, float blue) {
        glClearColor(red, green, blue, 1.0f);
    }

    void bind_texture_2d(GLuint texture, int unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void draw_arrays(int count) {
        glDrawArrays(GL_TRIANGLES, 0, count);
    }
}
