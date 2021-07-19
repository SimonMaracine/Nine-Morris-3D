#pragma once

#include <memory>
#include <functional>

#include <entt/entt.hpp>

#include "application/events.h"

#include "opengl/renderer/shader.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/framebuffer.h"
#include "opengl/renderer/renderer.h"
#include "opengl/renderer/vertex_buffer.h"
#include <other/model.h>

struct GLFWwindow;
class Application;

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
    float update_fps_counter();

    bool running = true;
    GLFWwindow* window;
    ApplicationData data;

    std::shared_ptr<VertexBuffer> create_ids_buffer(unsigned int vertices_size,
                                                    entt::entity entity);
    std::shared_ptr<VertexArray> create_entity_vertex_buffer(model::Mesh mesh,
                                                             entt::entity entity);

    bool on_window_closed(events::WindowClosedEvent& event);
    bool on_window_resized(events::WindowResizedEvent& event);
    bool on_mouse_scrolled(events::MouseScrolledEvent& event);
    bool on_mouse_moved(events::MouseMovedEvent& event);

    void build_board(const model::Mesh& mesh);
    void build_camera();
    void build_skybox();
    void build_piece(const model::Mesh& mesh, std::shared_ptr<Texture> diffuse_texture,
                     const glm::vec3& position);
    void build_directional_light();
    void build_origin();

    // const float mouse_sensitivity = 0.13f;
    // const float scroll_sensitivity = 1.2f;
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
    entt::entity piece = entt::null;
    entt::entity directional_light = entt::null;
    entt::entity origin = entt::null;

    entt::entity hovered_entity = entt::null;
};

