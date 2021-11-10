#include <glm/glm.hpp>

#include "nine_morris_3d/piece.h"
#include "nine_morris_3d/hoverable.h"

Piece::Piece(hoverable::Id id, Piece::Type type)
    : id(id), type(type) {

}
