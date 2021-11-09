#include <glm/glm.hpp>

#include "nine_morris_3d/node.h"
#include "nine_morris_3d/hoverable.h"

Node::Node(hoverable::Id id) : id(id) {

}

float Node::scale = 1.0f;
int Node::index_count = 0;
