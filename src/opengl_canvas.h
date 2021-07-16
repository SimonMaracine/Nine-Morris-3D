#pragma once

#include <memory>

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <entt/entt.hpp>

#include "opengl/renderer/shader.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/framebuffer.h"
#include "opengl/renderer/renderer.h"
#include "opengl/renderer/vertex_buffer.h"
#include <other/model.h>

class OpenGLCanvas : public Fl_Gl_Window {
public:
    OpenGLCanvas(int x, int y, int w, int h, const char* t);

    virtual void draw();
    virtual int handle(int event);

    void start_program();
    void resize();
    void reset();
    void end_program();

    std::shared_ptr<VertexBuffer> create_ids_buffer(unsigned int vertices_size,
                                                    entt::entity entity);
    std::shared_ptr<VertexArray> create_entity_vertex_buffer(model::Mesh mesh,
                                                             entt::entity entity);

    void build_board(const model::Mesh& mesh);
    void build_camera();
    void build_skybox();
    void build_piece(const model::Mesh& mesh, std::shared_ptr<Texture> diffuse_texture,
                     const glm::vec3& position);
    void build_directional_light();
    void build_origin();

    int width = 0, height = 0;

    int mouse_x = 0;
    int mouse_y = 0;
    int mouse_wheel = 0;
    bool left_mouse_pressed = false;
    bool right_mouse_pressed = false;
    float mouse_dt_x = 0;
    float mouse_dt_y = 0;
    bool pressed_A = false;
    bool pressed_D = false;
    bool pressed_W = false;
    bool pressed_S = false;
    bool pressed_R = false;
    bool pressed_F = false;

    float mouse_sensitivity = 0.13f;
    float scroll_sensitivity = 1.2f;

    entt::registry registry;

    const renderer::Storage* storage = nullptr;

    entt::entity board = entt::null;
    entt::entity camera = entt::null;
    entt::entity skybox = entt::null;
    entt::entity piece = entt::null;
    entt::entity directional_light = entt::null;
    entt::entity origin = entt::null;

    entt::entity hovered_entity = entt::null;
};
