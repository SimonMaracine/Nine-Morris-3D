#include <glm/glm.hpp>

#include "nine_morris_3d/node.h"

Node::Node(unsigned int id) : Hoverable(id) {

}

float Node::scale = 1.0f;
int Node::index_count = 0;
