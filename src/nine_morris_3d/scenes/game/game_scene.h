#pragma once

#include <array>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "application/scene.h"
#include "application/application.h"
#include "graphics/renderer/camera.h"
#include "graphics/renderer/buffer.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/light.h"
#include "other/model.h"
#include "nine_morris_3d/options.h"
#include "nine_morris_3d/board.h"
#include "nine_morris_3d/hoverable.h"
#include "nine_morris_3d/turn_indicator.h"
#include "nine_morris_3d/timer.h"

class GameScene : public Scene {
public:
    GameScene(unsigned int id)
        : Scene(id) {}
    virtual ~GameScene() = default;

    virtual void on_enter() override;
    virtual void on_exit() override;

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

    Board board;
    Camera camera;
    Light light;
    TurnIndicator turn_indicator;
    std::shared_ptr<std::vector<Board>> board_state_history;
    Timer timer;

    hoverable::Id hovered_id = hoverable::null;
};
