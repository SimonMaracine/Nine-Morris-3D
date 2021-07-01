#include <iostream>

#include <glad/glad.h>

#include "opengl_canvas.h"
#include "logging.h"

unsigned int compile_shader(int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
        printf("Compilation error!\n");

    return shader;
}

unsigned int create_shader_program(const char* vertex_source, const char* fragment_source) {
    unsigned int vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    unsigned int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
        printf("Linking error!\n");

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

static void update_game(void* data);

OpenGLCanvas::OpenGLCanvas(int x, int y, int w, int h, const char* t)
        : Fl_Gl_Window(x, y, w, h, t) {
    mode(FL_OPENGL3 | FL_DOUBLE);
    Fl::add_idle(update_game, this);

    logging::init();
}

void OpenGLCanvas::draw() {
    if (!context_valid()) {
        make_current();
        gladLoadGL();
        start();
    }

    if (!valid()) {
        resize();
    }

    glClearColor(0.5f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);
    glBindVertexArray(array);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int OpenGLCanvas::handle(int event) {
    switch(event) {
        case FL_PUSH:
            std::cout << "Mouse down { " << Fl::event_x() << ", " << Fl::event_y() << " }" << std::endl;
            return 1;
        case FL_DRAG:
            // ... mouse moved while down event ...
            return 1;
        case FL_RELEASE:
            // ... mouse up event ...
            return 1;
        case FL_FOCUS:
        case FL_UNFOCUS:
            // ... Return 1 if you want keyboard events, 0 otherwise
            return 1;
        case FL_KEYBOARD: {
            // ... keypress, key is in Fl::event_key(), ascii in Fl::event_text()
            // ... Return 1 if you understand/use the keyboard event, 0 otherwise...
            int key = Fl::event_key();
            char keycode = (char) key;
            if (keycode == ' ') {
                std::cout << "Pressed space" << std::endl;
            }
            return 1;
        }
        case FL_SHORTCUT:
            // ... shortcut, key is in Fl::event_key(), ascii in Fl::event_text()
            // ... Return 1 if you understand/use the shortcut event, 0 otherwise...
            return 1;
        default:
            // pass other events to the base class...
            return Fl_Gl_Window::handle(event);
    }
}

void OpenGLCanvas::start() {
    std::cout << glGetString(GL_VERSION) << std::endl;
    logging::log_opengl_info(false);

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    glGenVertexArrays(1, &array);
    glBindVertexArray(array);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*) 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    shader = create_shader_program(vertex_source, fragment_source);
}

void OpenGLCanvas::resize() {
    int width = w();
    int height = h();
    std::cout << width << ", " << height << std::endl;
    glViewport(0, 0, width, height);
}

void OpenGLCanvas::reset() {
    glDeleteProgram(shader);
    glDeleteVertexArrays(1, &array);
    start();
    std::cout << "Reset!" << std::endl;
}

void OpenGLCanvas::end_program() {
    // Do ending stuff
}

static void update_game(void* data) {
    OpenGLCanvas* canvas = (OpenGLCanvas*) data;

    // Update stuff

    canvas->redraw();
}
