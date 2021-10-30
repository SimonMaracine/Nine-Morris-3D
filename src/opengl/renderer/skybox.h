#pragma once

#include "opengl/renderer/texture.h"
#include "opengl/renderer/vertex_array.h"

class Skybox {
public:
    Skybox() = default;
    ~Skybox() = default;

    Rc<Texture3D> texture;
    Rc<VertexArray> vertex_array;
};
