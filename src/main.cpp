#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Item.H>

#include "opengl_canvas.h"

static void new_game(Fl_Widget* _w, void* data) {
    OpenGLCanvas* canvas = (OpenGLCanvas*) data;

    canvas->reset();
}

static void exit_game(Fl_Widget* _w, void* data) {
    OpenGLCanvas* canvas = (OpenGLCanvas*) data;

    canvas->end_program();
    std::exit(0);
}

int main() {
    Fl_Window* window = new Fl_Window(1024 + 20, 576 + 50, "Nine Morris 3D");
    OpenGLCanvas* canvas = new OpenGLCanvas(10, 40, 1024, 576, "Nine Morris 3D");

    Fl_Menu_Item menuitems[] = {
        { "Game", 0, 0, 0, FL_SUBMENU },
            { "New", 0, (Fl_Callback*) new_game, canvas },
            { "Exit", 0, (Fl_Callback*) exit_game, canvas },
            { 0 },
        { "Help", 0, 0, 0, FL_SUBMENU },
            { "About", 0, (Fl_Callback*) nullptr, nullptr },
            { 0 },
        { 0 }
    };

    Fl_Menu_Bar* bar_menu = new Fl_Menu_Bar(0, 0, 1024 + 20, 30);
    bar_menu->copy(menuitems);

    window->end();
    window->resizable(canvas);
    window->show();
    return Fl::run();
}
