#pragma once

#include <FL/Fl.H>
#include <FL/gl.h>
#include <FL/Fl_Gl_Window.H>

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

    const char* vertex_source =
        "#version 430 core\n"
        ""
        "layout(location = 0) in vec2 position;"
        ""
        "void main() {"
        "   gl_Position = vec4(position, 0.0, 1.0);"
        "}";

    const char* fragment_source =
        "#version 430 core\n"
        ""
        "out vec4 fragment_color;"
        ""
        "void main() {"
        "   fragment_color = vec4(1.0, 0.0, 0.0, 1.0);"
        "}";

    unsigned int array;
    unsigned int shader;
};
