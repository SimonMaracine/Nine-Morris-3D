#include <glm/glm.hpp>

#include "nine_morris_3d/piece.h"

Piece::Piece(unsigned int id, Piece::Type type)
    : Hoverable(id), type(type) {

}

float Piece::scale = 1.0f;
int Piece::index_count = 0;

glm::vec3 Piece::specular_color = glm::vec3(0.0f);
float Piece::shininess = 0.0f;

glm::vec3 Piece::select_color = glm::vec3(0.0f);
glm::vec3 Piece::hover_color = glm::vec3(0.0f);
