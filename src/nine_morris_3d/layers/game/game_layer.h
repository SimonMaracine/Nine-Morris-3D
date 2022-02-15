#pragma once

#include <string>
#include <utility>

#include "application/layer.h"
#include "application/events.h"
#include "graphics/renderer/renderer.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/buffer.h"
#include "graphics/renderer/camera.h"
#include "graphics/renderer/light.h"
#include "other/model.h"
#include "other/loader.h"

class ImGuiLayer;
class GuiLayer;

class GameLayer : public Layer {
public:
    GameLayer(std::string&& id, Application* app)
        : Layer(std::move(id), app) {}
    virtual ~GameLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_bind_layers() override;
    virtual void on_update(float dt) override;
    virtual void on_fixed_update() override;
    virtual void on_draw() override;
    virtual void on_event(events::Event& event) override;

    bool on_mouse_scrolled(events::MouseScrolledEvent& event);
    bool on_mouse_moved(events::MouseMovedEvent& event);
    bool on_mouse_button_pressed(events::MouseButtonPressedEvent& event);
    bool on_mouse_button_released(events::MouseButtonReleasedEvent& event);
    bool on_key_released(events::KeyReleasedEvent& event);
    bool on_window_resized(events::WindowResizedEvent& event);

    std::shared_ptr<Buffer> create_ids_buffer(unsigned int vertices_size, hoverable::Id id);
    std::shared_ptr<VertexArray> create_entity_vertex_array(std::shared_ptr<model::Mesh<model::Vertex>> mesh,
            hoverable::Id id);

    void build_board();
    void build_board_paint();
    void build_piece(unsigned int index, Piece::Type type, std::shared_ptr<model::Mesh<model::Vertex>> mesh,
            std::shared_ptr<Texture> texture, const glm::vec3& position);
    void build_node(unsigned int index, const glm::vec3& position);
    void build_camera();
    void build_skybox();
    void build_light();
    void build_turn_indicator();

    void render_skybox();
    void setup_light();
    void setup_camera();
    void setup_board();
    void setup_board_paint();
    void setup_pieces();
    void render_pieces();
    void render_nodes();
    void render_to_depth();
    void setup_shadows();
    void setup_quad3d_projection_view();

    void set_scene_framebuffer(int samples);
    void set_textures_quality(const std::string& quality);
    void set_skybox(const std::string& skybox);
    void load_game();

    Board board;
    Camera camera;
    Light light;
    std::shared_ptr<std::vector<Board>> board_state_history;

    hoverable::Id hovered_id = hoverable::null;

    float mouse_wheel = 0.0f;
    float dx = 0.0f;
    float dy = 0.0f;
    float last_mouse_x = 0.0f;
    float last_mouse_y = 0.0f;

    bool first_move = false;

    glm::vec3 default_camera_position = glm::vec3(0.0f);

    ImGuiLayer* imgui_layer = nullptr;
    GuiLayer* gui_layer = nullptr;
};
