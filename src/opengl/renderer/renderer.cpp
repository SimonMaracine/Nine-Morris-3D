#include <memory>

#include <glad/glad.h>

#include "opengl/renderer/vertex_array.h"

namespace renderer {
    void init() {

    }

    void set_viewport(GLint width, GLint height) {
        glViewport(0, 0, width, height);
    }

    void set_clear_color(GLfloat red, GLfloat green, GLfloat blue) {
        glClearColor(red, green, blue, 1.0f);
    }

    void clear() {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void begin() {

    }

    void end() {

    }

    void draw_indexed(std::shared_ptr<VertexArray> array, GLsizei count) {  // TODO what?
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }
}
