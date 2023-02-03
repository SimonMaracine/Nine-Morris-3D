#include <glad/glad.h>

#include "engine/graphics/opengl/framebuffer.h"

namespace render_helpers {
    void clear(int buffers) {
        glClear(buffers);
    }

    void clear_color(float red, float green, float blue) {
        glClearColor(red, green, blue, 1.0f);
    }

    void viewport(int width, int height) {
        glViewport(0, 0, width, height);
    }

    void viewport(const gl::FramebufferSpecification& specification) {
        glViewport(0, 0, specification.width, specification.height);
    }

    void bind_texture_2d(GLuint texture, int unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void draw_arrays(int count) {
        glDrawArrays(GL_TRIANGLES, 0, count);
    }

    void draw_arrays_lines(int count) {
        glDrawArrays(GL_LINES, 0, count);
    }

    void draw_elements(int count) {
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }

    void draw_elements_instanced(int count, int instance_count) {
        glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, instance_count);
    }

    void disable_depth_test() {
        glDisable(GL_DEPTH_TEST);
    }

    void enable_depth_test() {
        glEnable(GL_DEPTH_TEST);
    }

    void disable_blending() {
        glDisable(GL_BLEND);
    }

    void enable_blending() {
        glEnable(GL_BLEND);
    }
}
