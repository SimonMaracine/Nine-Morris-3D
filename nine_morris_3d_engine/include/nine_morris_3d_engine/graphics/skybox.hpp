#pragma once

#include <memory>

#include "nine_morris_3d_engine/graphics/opengl/texture.hpp"

namespace sm {
    struct Skybox {
        std::shared_ptr<GlTextureCubemap> texture;
    };
}
