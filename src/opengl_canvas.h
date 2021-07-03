#pragma once

#include <memory>

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

#include "opengl/renderer/shader.h"
#include "opengl/renderer/vertex_array.h"

class OpenGLCanvas : public Fl_Gl_Window {
public:
    OpenGLCanvas(int x, int y, int w, int h, const char* t);

    virtual void draw();
    virtual int handle(int event);

    void reset();
    void end_program();
private:
    void resize();
    void start_program();

    float positions[6] = {
        0.0f, 0.5f,
        -0.5f, -0.5f,
        0.5f, -0.5f
    };

    unsigned int indices[3] = {
        0, 1, 2
    };

    std::shared_ptr<Shader> shader = nullptr;
    std::shared_ptr<VertexArray> array = nullptr;
    std::shared_ptr<VertexBuffer> buffer = nullptr;
    std::shared_ptr<VertexBuffer> index_buffer = nullptr;
};
