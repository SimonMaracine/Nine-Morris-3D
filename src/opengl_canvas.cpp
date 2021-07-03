#include <iostream>
#include <cassert>
#include <chrono>
#include <memory>

#include "opengl_canvas.h"
#include "logging.h"
#include "opengl/debug_opengl.h"
#include "opengl/renderer/renderer.h"

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
        start_program();
    }

    if (!valid()) {
        resize();
    }

    renderer::set_clear_color(0.5f, 0.0f, 0.5f);
    renderer::clear();

    shader->bind();
    array->bind();
    renderer::draw_indexed(array, 3);
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

void OpenGLCanvas::start_program() {
    logging::log_opengl_info(false);
    debug_opengl::maybe_init_debugging();
    auto [major, minor] = debug_opengl::get_version();
    assert(major >= 4 && minor >= 3);

    shader = Shader::create("data/shaders/minimum.vert",
                            "data/shaders/minimum.frag");
    buffer = VertexBuffer::create_with_data(positions, sizeof(positions));
    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 2);

    index_buffer = VertexBuffer::create_index(indices, sizeof(indices));

    array = VertexArray::create();
    index_buffer->bind();
    array->add_buffer(buffer, layout);
    VertexArray::unbind();
}

void OpenGLCanvas::resize() {
    int width = w();
    int height = h();
    renderer::set_viewport(width, height);
}

void OpenGLCanvas::reset() {
    std::cout << "Resetting!" << std::endl;
    start_program();
}

void OpenGLCanvas::end_program() {
    // Do ending stuff
}

static void update_fps_counter() {
    using namespace std::chrono;
    using clock = high_resolution_clock;

    static clock::time_point previous_seconds = clock::now();
    static int frame_count = 0;

    clock::time_point current_seconds = clock::now();
    duration<double> elapsed_seconds =
        duration_cast<duration<double>>(current_seconds - previous_seconds);

    if (elapsed_seconds.count() > 0.25) {
        previous_seconds = current_seconds;
        double fps = (double) frame_count / elapsed_seconds.count();
        SPDLOG_DEBUG("{}", fps);
        frame_count = 0;
    }
    frame_count++;
}

static void update_game(void* data) {
    OpenGLCanvas* canvas = (OpenGLCanvas*) data;

    update_fps_counter();

    // Update stuff

    canvas->redraw();
}
