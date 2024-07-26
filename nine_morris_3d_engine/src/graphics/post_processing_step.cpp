#include "nine_morris_3d_engine/graphics/post_processing_step.hpp"

#include "nine_morris_3d_engine/graphics/internal/opengl.hpp"

namespace sm {
    void PostProcessingStep::bind_texture(unsigned int texture, int unit) noexcept {
        internal::opengl::bind_texture_2d(texture, unit);
    }
}
