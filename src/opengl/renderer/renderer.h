#pragma once

#include <memory>

#include <glad/glad.h>

#include "opengl/renderer/vertex_array.h"

namespace renderer {
    void init();
    void set_viewport(GLint width, GLint height);
    void set_clear_color(GLfloat red, GLfloat green, GLfloat blue);
    void clear();
    void begin();
    void end();
    void draw_indexed(std::shared_ptr<VertexArray> array, GLsizei count);
}
