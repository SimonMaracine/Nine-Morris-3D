#pragma once

#include <vector>

#include "nine_morris_3d_engine/graphics/post_processing_step.hpp"

namespace sm {
    namespace internal {
        struct PostProcessingContext {
            std::vector<unsigned int> textures;  // All textures in order
            unsigned int last_texture {};  // Last texture at any moment in the processing pipeline
            unsigned int original_texture {};
        };
    }
}
