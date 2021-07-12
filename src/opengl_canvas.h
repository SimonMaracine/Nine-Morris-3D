#pragma once

#include <memory>

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <entt/entt.hpp>

#include "opengl/renderer/shader.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/vertex_buffer.h"

class OpenGLCanvas : public Fl_Gl_Window {
public:
    OpenGLCanvas(int x, int y, int w, int h, const char* t);

    virtual void draw();
    virtual int handle(int event);

    void reset();
    void end_program();
    void resize();
    void start_program();

    void build_board();
    void build_camera();
    void build_skybox();
    void build_box();
    void build_piece();

    int mouse_x = 0;
    int mouse_y = 0;
    int mouse_wheel = 0;
    bool left_mouse_pressed = false;
    bool right_mouse_pressed = false;
    float mouse_dt_x = 0;
    float mouse_dt_y = 0;

    float mouse_sensitivity = 0.13f;

    entt::registry registry;

    std::shared_ptr<Shader> basic_shader = nullptr;

    entt::entity board = entt::null;
    entt::entity camera = entt::null;
    entt::entity skybox = entt::null;
    entt::entity box = entt::null;
    entt::entity piece = entt::null;
};
