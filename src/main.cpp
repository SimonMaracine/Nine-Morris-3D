#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Item.H>

#include <glad/glad.h>
#include <GL/glx.h>

#include <FL/gl.h>
#include <FL/Fl_Gl_Window.H>

void update_game(void* data);

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

class OpenGLWindow : public Fl_Gl_Window {
public:
    OpenGLWindow(int x, int y, int w, int h, const char* t);

    virtual void draw();
    virtual int handle(int event);

    void reset();
    void end_program();
    void hello();

    int foo = 18;
private:
    void resize();
    void start();

    float positions[6] = {
        0.0f, 0.5f,
        -0.5f, -0.5f,
        0.5f, -0.5f
    };

    const char* vertex_source =
        "#version 330 core\n"
        ""
        "layout(location = 0) in vec2 position;"
        ""
        "void main() {"
        "   gl_Position = vec4(position, 0.0, 1.0);"
        "}";

    const char* fragment_source =
        "#version 330 core\n"
        ""
        "out vec4 fragment_color;"
        ""
        "void main() {"
        "   fragment_color = vec4(1.0, 0.0, 0.0, 1.0);"
        "}";

    unsigned int array;
    unsigned int shader;
};

OpenGLWindow::OpenGLWindow(int x, int y, int w, int h, const char* t)
        : Fl_Gl_Window(x, y, w, h, t) {
    mode(FL_OPENGL3 | FL_DOUBLE);
    Fl::add_idle(update_game, this);
}

void OpenGLWindow::draw() {
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

int OpenGLWindow::handle(int event) {
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

void OpenGLWindow::start() {
    std::cout << glGetString(GL_VERSION) << std::endl;

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof (float), positions, GL_STATIC_DRAW);

    glGenVertexArrays(1, &array);
    glBindVertexArray(array);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof (float), (const void*) 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    shader = create_shader_program(vertex_source, fragment_source);
}

void OpenGLWindow::resize() {
    int width = w();
    int height = h();
    std::cout << width << ", " << height << std::endl;
    glViewport(0, 0, width, height);
}

void OpenGLWindow::reset() {
    glDeleteProgram(shader);
    glDeleteVertexArrays(1, &array);
    start();
    std::cout << "Reset!" << std::endl;
}

void OpenGLWindow::end_program() {
    // Do ending stuff
}

void update_game(void* data) {
    OpenGLWindow* window = (OpenGLWindow*) data;

    // Update stuff

    window->redraw();
}

void new_game(Fl_Widget* _w, void* data) {
    OpenGLWindow* canvas = (OpenGLWindow*) data;
    canvas->reset();
    std::cout << "New game" << std::endl;
    std::cout << canvas->foo << std::endl;
}

void exit_game(Fl_Widget *_w, void *data) {
    OpenGLWindow* canvas = (OpenGLWindow*) data;
    canvas->end_program();
    std::exit(0);
}

int main() {
    Fl_Window* window = new Fl_Window(640, 480, "Test");
    OpenGLWindow* canvas = new OpenGLWindow(10, 40, 620, 430, "Test");

    Fl_Menu_Item menuitems[] = {
        { "Game", 0, 0, 0, FL_SUBMENU },
            { "New", 0, (Fl_Callback*) new_game, canvas },
            { "Exit", 0, (Fl_Callback*) exit_game, canvas },
            { 0 },
        { "Help", 0, 0, 0, FL_SUBMENU },
            { "About", 0, (Fl_Callback*) NULL, 0 },
            { 0 },
        { 0 }
    };

    Fl_Menu_Bar* bar_menu = new Fl_Menu_Bar(0, 0, 640, 30);
    bar_menu->copy(menuitems);

    window->end();
    window->resizable(canvas);
    window->show();
    return Fl::run();
}
