#include "game/piece.hpp"

#include <utility>

Piece::Piece(unsigned int index, glm::vec3 position, sm::Renderable&& renderable)
    : renderable(std::move(renderable)) {}
