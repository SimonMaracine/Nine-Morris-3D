#pragma once

#include <array>
#include <memory>

#include <glm/glm.hpp>

#include "application/scene.h"
#include "application/application.h"
#include "opengl/renderer/camera.h"
#include "opengl/renderer/buffer.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/skybox.h"
#include "other/model.h"
#include "nine_morris_3d/options.h"
#include "nine_morris_3d/board.h"
#include "nine_morris_3d/hoverable.h"

class GameScene : public Scene {
public:
    GameScene(unsigned int id, Application* application)
        : Scene(id, application) {}
    virtual ~GameScene() = default;

    virtual void on_enter() override;
    virtual void on_exit() override;

    Rc<Buffer> create_ids_buffer(unsigned int vertices_size, unsigned int id);  // TODO rework buffers
    Rc<VertexArray> create_entity_vertex_array(Rc<model::Mesh<model::Vertex>> mesh, unsigned int id);

    void build_board();
    void build_board_paint();
    void build_piece(unsigned int index, Piece::Type type, Rc<model::Mesh<model::Vertex>> mesh,
            Rc<Texture> texture, const glm::vec3& position);
    void build_node(unsigned int index, const glm::vec3& position);
    void build_camera();
    void build_skybox();

    options::Options options;

    // entt::entity board = entt::null;
    // entt::entity camera = entt::null;
    // entt::entity nodes[24];
    // entt::entity pieces[18];

    // entt::entity hovered_entity = entt::null;

    Board board;
    std::array<std::shared_ptr<Piece>, 18> pieces;
    std::array<Node, 24> nodes;
    Camera camera;
    Skybox skybox;

    Hoverable hovered_entity = HOVERABLE_NULL;
};
