#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

struct BlurStep : sm::PostProcessingStep {
    BlurStep(std::shared_ptr<sm::GlFramebuffer> framebuffer, std::unique_ptr<sm::GlShader>&& shader)
        : sm::PostProcessingStep(framebuffer, std::move(shader)) {}

    void setup(const sm::PostProcessingContext& context) const override;
};
