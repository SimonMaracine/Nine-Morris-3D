#include "nine_morris_3d/board.h"

Board::Board(unsigned int id) : Hoverable(id) {

}

float Board::scale = 1.0f;
int Board::index_count = 0;

glm::vec3 Board::specular_color = glm::vec3(0.0f);
float Board::shininess = 0.0f;

int BoardPaint::index_count = 0;
