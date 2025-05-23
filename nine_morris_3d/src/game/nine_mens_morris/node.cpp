#include "game/nine_mens_morris/node.hpp"

#include <nine_morris_3d_engine/external/resmanager.h++>

NodeObj::NodeObj(int id, glm::vec3 position, std::shared_ptr<sm::ModelNode> model)
    : HoverableObj(id), m_model(model) {
    m_model->position = position;
}

void NodeObj::update(sm::Ctx& ctx) {
    if (m_highlighted) {
        const auto board_model {ctx.render_3d()->find_node("board"_H)};
        board_model->add_node(m_model);
    }
}
