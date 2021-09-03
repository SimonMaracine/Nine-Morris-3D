#pragma once

#include <entt/entt.hpp>

#include "application/application.h"
#include "application/layer.h"
#include "application/events.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/buffer.h"
#include "opengl/renderer/renderer.h"
#include "other/model.h"
#include "other/loader.h"
#include "ecs_and_game/game.h"

#define LIGHT_POSITION glm::vec3(-11.0f, 13.0f, -15.0f)

class ImGuiLayer;

class GameLayer : public Layer {
public:
    GameLayer(unsigned int id, Application* application)
        : Layer(id, application) {};
    virtual ~GameLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_bind_layers() override;
    virtual void on_update(float dt) override;
    virtual void on_draw() override;
    virtual void on_event(events::Event& event) override;

    bool on_mouse_scrolled(events::MouseScrolledEvent& event);
    bool on_mouse_moved(events::MouseMovedEvent& event);
    bool on_mouse_button_pressed(events::MouseButtonPressedEvent& event);
    bool on_mouse_button_released(events::MouseButtonReleasedEvent& event);
    bool on_window_resized(events::WindowResizedEvent& event);

    enum class TextureQuality {
        High, Normal
    };

    void start();
    void start_after_load();
    void restart();
    void end();
    void set_textures_quality(TextureQuality quality);

    static Rc<Buffer> create_ids_buffer(unsigned int vertices_size, entt::entity entity);
    static Rc<VertexArray> create_entity_vertex_array(Rc<model::Mesh<FullVertex>> mesh, entt::entity entity);

    void build_board();
    void build_board_paint();
    void build_camera();
    void build_skybox();
    void build_piece(int id, Piece type, Rc<model::Mesh<FullVertex>> mesh,
                     Rc<Texture> diffuse_texture, const glm::vec3& position);
    void build_directional_light();
    void build_origin();
    void build_node(int index, const glm::vec3& position);
    void build_turn_indicator();

    float mouse_wheel = 0.0f;
    float dx = 0.0f;
    float dy = 0.0f;
    float last_mouse_x = 0.0f;
    float last_mouse_y = 0.0f;

    TextureQuality texture_quality = TextureQuality::High;
    renderer::Storage* storage = nullptr;
    std::shared_ptr<Assets> assets = nullptr;
    Loader loader;

    entt::registry registry;
    entt::entity board = entt::null;
    entt::entity camera = entt::null;
    entt::entity nodes[24];

    entt::entity hovered_entity = entt::null;

    ImGuiLayer* imgui_layer;
};
