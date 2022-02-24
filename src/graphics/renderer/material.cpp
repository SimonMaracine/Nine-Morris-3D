#include <memory>

#include "graphics/renderer/material.h"
#include "graphics/renderer/shader.h"


Material::Material(std::shared_ptr<Shader> shader)
    : shader(shader) {

}

Material::~Material() {

}
