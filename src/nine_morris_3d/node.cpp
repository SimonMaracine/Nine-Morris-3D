#include <glm/glm.hpp>

#include "nine_morris_3d/node.h"
#include "graphics/renderer/hoverable.h"

Node::Node(hoverable::Id id, unsigned int index)
    : id(id), index(index) {

}
