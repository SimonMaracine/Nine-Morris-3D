#pragma once

#include "application/layer.h"
#include "application/events.h"
#include "graphics/renderer/framebuffer_reader.h"
#include "graphics/renderer/opengl/vertex_array.h"
#include "graphics/renderer/opengl/buffer.h"
#include "nine_morris_3d/piece.h"
#include "nine_morris_3d/board.h"
#include "nine_morris_3d/options.h"
#include "nine_morris_3d/keyboard_controls.h"
#include "other/mesh.h"
#include "other/loader.h"

using namespace mesh;

class ImGuiLayer;
class GuiLayer;

class GameLayer : public Layer {
public:
    GameLayer(const std::string& id, Application* app)
        : Layer(id, app) {}
    virtual ~GameLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_awake() override;
    virtual void on_update(float dt) override;
    virtual void on_fixed_update() override;
    virtual void on_event(events::Event& event) override;

    bool on_mouse_scrolled(events::MouseScrolledEvent& event);
    bool on_mouse_moved(events::MouseMovedEvent& event);
    bool on_mouse_button_pressed(events::MouseButtonPressedEvent& event);
    bool on_mouse_button_released(events::MouseButtonReleasedEvent& event);
    bool on_key_pressed(events::KeyPressedEvent& event);
    bool on_key_released(events::KeyReleasedEvent& event);

    std::shared_ptr<Buffer> create_ids_buffer(unsigned int vertices_size, hoverable::Id id);

    void prepare_board();
    void prepare_board_paint();
    void prepare_pieces();
    void prepare_piece(unsigned int index, Piece::Type type, std::shared_ptr<Mesh<VPTNT>> mesh,
            std::shared_ptr<Texture> diffuse_texture);

    void prepare_board_no_normal();
    void prepare_board_paint_no_normal();
    void prepare_pieces_no_normal();
    void prepare_piece_no_normal(unsigned int index, Piece::Type type, std::shared_ptr<Mesh<VPTN>> mesh,
            std::shared_ptr<Texture> diffuse_texture);

    void prepare_nodes();
    void prepare_node(unsigned int index, const glm::vec3& position);

    void resetup_textures();

    void setup_board();
    void setup_board_paint();
    void setup_pieces();
    void setup_piece(unsigned int index, Piece::Type type, std::shared_ptr<Mesh<VPTNT>> mesh);

    void setup_board_no_normal();
    void setup_board_paint_no_normal();
    void setup_pieces_no_normal();
    void setup_piece_no_normal(unsigned int index, Piece::Type type, std::shared_ptr<Mesh<VPTN>> mesh);

    void setup_nodes();
    void setup_node(unsigned int index, const glm::vec3& position);

    void setup_skybox();
    void setup_light();
    void setup_camera();

    void set_scene_framebuffer(int samples);
    void set_texture_quality(std::string_view quality);
    void set_skybox(std::string_view skybox);
    void set_labeled_board_texture(bool labeled_board);
    void set_normal_mapping(bool normal_mapping);

    void actually_change_texture_quality();
    void actually_change_skybox();
    void actually_change_labeled_board_texture();
    void actually_change_normal_mapping();

    void load_game();

    Board board;
    StateHistory state_history;
    KeyboardControls keyboard;

    float mouse_wheel = 0.0f;
    float dx = 0.0f;
    float dy = 0.0f;
    float last_mouse_x = 0.0f;
    float last_mouse_y = 0.0f;

    bool first_move = false;

    glm::vec3 default_camera_position = glm::vec3(0.0f);

    std::unique_ptr<Loader<AssetsData, options::Options>> loader;

    bool changed_skybox = false;
    bool changed_texture_quality = false;
    bool changed_labeled_board_texture = false;
    bool changed_normal_mapping = false;

    ImGuiLayer* imgui_layer = nullptr;
    GuiLayer* gui_layer = nullptr;
};
