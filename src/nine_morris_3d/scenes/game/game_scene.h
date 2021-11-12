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
#include "opengl/renderer/light.h"
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

    Rc<Buffer> create_ids_buffer(unsigned int vertices_size, hoverable::Id id);  // TODO rework buffers
    Rc<VertexArray> create_entity_vertex_array(Rc<model::Mesh<model::Vertex>> mesh, hoverable::Id id);

    void build_board();
    void build_board_paint();
    void build_piece(unsigned int index, Piece::Type type, Rc<model::Mesh<model::Vertex>> mesh,
            Rc<Texture> texture, const glm::vec3& position);
    void build_node(unsigned int index, const glm::vec3& position);
    void build_camera();
    void build_skybox();
    void build_light();

    options::Options options;

    Board board;
    Camera camera;
    Skybox skybox;
    Light light;

    hoverable::Id hovered_id = HOVERABLE_NULL;
};
