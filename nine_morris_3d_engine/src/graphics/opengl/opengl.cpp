#include "engine/graphics/opengl/opengl.hpp"

#include <glad/glad.h>

namespace sm {
    void OpenGl::initialize_default() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);
    }

    void OpenGl::clear(Buffers buffers) {
        unsigned int result {0};

        switch (buffers) {
            case Buffers::C:
                result = GL_COLOR_BUFFER_BIT;
                break;
            case Buffers::D:
                result = GL_DEPTH_BUFFER_BIT;
                break;
            case Buffers::S:
                result = GL_STENCIL_BUFFER_BIT;
                break;
            case Buffers::CD:
                result = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
                break;
            case Buffers::CS:
                result |= GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
                break;
            case Buffers::DS:
                result |= GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
                break;
            case Buffers::CDS:
                result |= GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
                break;
        }

        glClear(result);
    }

    void OpenGl::clear_color(float red, float green, float blue) {
        glClearColor(red, green, blue, 1.0f);
    }

    void OpenGl::viewport(int width, int height) {
        glViewport(0, 0, width, height);
    }

    void OpenGl::bind_texture_2d(unsigned int texture, int unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void OpenGl::draw_arrays(int count) {
        glDrawArrays(GL_TRIANGLES, 0, count);
    }

    void OpenGl::draw_arrays_lines(int count) {
        glDrawArrays(GL_LINES, 0, count);
    }

    void OpenGl::draw_elements(int count) {
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }

    void OpenGl::draw_elements_instanced(int count, int instance_count) {
        glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, instance_count);
    }

    void OpenGl::disable_depth_test() {
        glDisable(GL_DEPTH_TEST);
    }

    void OpenGl::enable_depth_test() {
        glEnable(GL_DEPTH_TEST);
    }

    void OpenGl::disable_blending() {
        glDisable(GL_BLEND);
    }

    void OpenGl::enable_blending() {
        glEnable(GL_BLEND);
    }

    void OpenGl::disable_back_face_culling() {
        glDisable(GL_CULL_FACE);
    }

    void OpenGl::enable_back_face_culling() {
        glEnable(GL_CULL_FACE);
    }

    void OpenGl::initialize_stencil() {
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    }

    void OpenGl::stencil_function(Function function, int ref, unsigned int mask) {
        unsigned int result {0};

        switch (function) {
            case Function::Always:
                result = GL_ALWAYS;
                break;
            case Function::NotEqual:
                result = GL_NOTEQUAL;
                break;
        }

        glStencilFunc(result, ref, mask);
    }

    void OpenGl::stencil_mask(unsigned int mask) {
        glStencilMask(mask);
    }
}
