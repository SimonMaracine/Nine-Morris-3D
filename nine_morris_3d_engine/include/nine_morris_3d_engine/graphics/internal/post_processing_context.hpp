#pragma once

#include <vector>

namespace sm::internal {
    struct PostProcessingContext {
        std::vector<unsigned int> textures;  // All textures in order
        unsigned int last_texture {};  // Last texture at any moment in the processing pipeline
        unsigned int original_texture {};
    };
}
