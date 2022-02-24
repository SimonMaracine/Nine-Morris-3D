#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "graphics/renderer/shader.h"

class Material {
public:
    Material(std::shared_ptr<Shader> shader, std::vector<std::string> uniforms);
    ~Material();


private:
    std::shared_ptr<Shader> shader;
    std::unordered_map<std::string, 
};
