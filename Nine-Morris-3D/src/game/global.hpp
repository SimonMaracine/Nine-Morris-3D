#pragma once

#include <engine/nine_morris_3d.hpp>

struct Global {
    std::shared_ptr<sm::PostProcessingStep> blur_step;
};
