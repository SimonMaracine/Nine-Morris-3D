#include <glm/glm.hpp>

#include "nine_morris_3d/piece.h"
#include "graphics/renderer/hoverable.h"

Piece::Piece(hoverable::Id id, Piece::Type type)
    : id(id), type(type) {

}
