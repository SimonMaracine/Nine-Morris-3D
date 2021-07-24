#pragma once

#include <memory>
#include <functional>

#include <entt/entt.hpp>

#include "application/events.h"
#include "application/window.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/framebuffer.h"
#include "opengl/renderer/renderer.h"
#include "opengl/renderer/vertex_buffer.h"
#include "other/model.h"
#include "ecs/game.h"

struct ApplicationData {
    int width, height;
    std::function<void(events::Event&)> event_function;
};

class Application {
public:
    Application(int width, int height);
    ~Application();

    void run();
    void on_event(events::Event& event);
    void update(float dt);
    void draw();

    void start();
    void end();
    float update_frame_counter();

    void imgui_start();
    void imgui_update(float dt);
    void imgui_end();

    bool running = true;
    std::unique_ptr<Window> window = nullptr;
    ApplicationData data;
    double fps = 0.0;

    bool on_window_closed(events::WindowClosedEvent& event);
    bool on_window_resized(events::WindowResizedEvent& event);
    bool on_mouse_scrolled(events::MouseScrolledEvent& event);
    bool on_mouse_moved(events::MouseMovedEvent& event);
    bool on_mouse_button_pressed(events::MouseButtonPressedEvent& event);
    bool on_mouse_button_released(events::MouseButtonReleasedEvent& event);

    std::shared_ptr<VertexBuffer> create_ids_buffer(unsigned int vertices_size,
                                                    entt::entity entity);
    std::shared_ptr<VertexArray> create_entity_vertex_buffer(model::Mesh mesh,
                                                             entt::entity entity);

    void build_board(const model::Mesh& mesh);
    void build_camera();
    void build_skybox();
    void build_piece(int index, Piece type, const model::Mesh& mesh,
                     std::shared_ptr<Texture> diffuse_texture, const glm::vec3& position);
    void build_directional_light();
    void build_origin();
    void build_node(int index, const model::Mesh& mesh, const glm::vec3& position);

    float mouse_wheel = 0.0f;
    float dx = 0.0f;
    float dy = 0.0f;
    float last_mouse_x = 0.0f;
    float last_mouse_y = 0.0f;

    entt::registry registry;
    const renderer::Storage* storage = nullptr;

    entt::entity board = entt::null;
    entt::entity camera = entt::null;
    entt::entity skybox = entt::null;
    entt::entity pieces[18];
    entt::entity directional_light = entt::null;
    entt::entity origin = entt::null;
    entt::entity nodes[24];

    entt::entity hovered_entity = entt::null;
};
