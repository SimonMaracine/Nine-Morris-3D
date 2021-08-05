#pragma once

#include <memory>
#include <functional>

#include <entt/entt.hpp>

#include "application/events.h"
#include "application/window.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/buffer.h"
#include "opengl/renderer/framebuffer.h"
#include "opengl/renderer/renderer.h"
#include "other/model.h"
#include "other/loader.h"
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
    void draw_loading_screen();

    void start();
    void start_after_load();
    void restart();
    void end();
    float update_frame_counter();

    void imgui_start();
    void imgui_update(float dt);
    void imgui_end();

    bool running = true;
    std::unique_ptr<Window> window = nullptr;
    ApplicationData data;
    double fps = 0.0;
    bool game_ready = false;

    bool on_window_closed(events::WindowClosedEvent& event);
    bool on_window_resized(events::WindowResizedEvent& event);
    bool on_mouse_scrolled(events::MouseScrolledEvent& event);
    bool on_mouse_moved(events::MouseMovedEvent& event);
    bool on_mouse_button_pressed(events::MouseButtonPressedEvent& event);
    bool on_mouse_button_released(events::MouseButtonReleasedEvent& event);

    std::shared_ptr<Buffer> create_ids_buffer(unsigned int vertices_size,
                                              entt::entity entity);
    std::shared_ptr<VertexArray> create_entity_vertex_array(model::Mesh mesh,
                                                            entt::entity entity);

    void build_board(const model::Mesh& mesh);
    void build_camera();
    void build_skybox();
    void build_piece(Piece type, const model::Mesh& mesh, std::shared_ptr<Texture> diffuse_texture,
                     const glm::vec3& position);
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
    std::shared_ptr<Assets> assets = nullptr;
    std::unique_ptr<Loader> loader = nullptr;

    entt::entity board = entt::null;
    entt::entity camera = entt::null;
    entt::entity nodes[24];

    entt::entity hovered_entity = entt::null;

    std::shared_ptr<Shader> debug = nullptr;
};
