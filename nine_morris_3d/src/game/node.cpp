#include "game/node.hpp"

#include <utility>

Node::Node(unsigned int index, glm::vec3 position, sm::Renderable&& renderable)
    : renderable(std::move(renderable)) {}

void Node::update(sm::Ctx& ctx) {
    if (highlighted) {
        ctx.add_renderable(renderable);
    }
}
