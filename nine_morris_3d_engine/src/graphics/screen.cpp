#include <memory>
#include <vector>
#include <utility>

#include "engine/graphics/opengl/framebuffer.hpp"
#include "engine/graphics/renderer/render_gl.hpp"
#include "engine/graphics/screen.hpp"

namespace sm {
    void Screen::add_framebuffer(std::shared_ptr<GlFramebuffer> framebuffer) {
        framebuffers.push_back(framebuffer);
    }

    void Screen::resize(int width, int height) {
        if (width == 0 || height == 0) {
            return;
        }

        RenderGl::viewport(width, height);

        for (std::weak_ptr<GlFramebuffer> wframebuffer : framebuffers) {
            std::shared_ptr<GlFramebuffer> framebuffer = wframebuffer.lock();

            if (framebuffer == nullptr) {
                return;
            }

            if (!framebuffer->get_specification().resizable) {
                return;
            }

            framebuffer->resize(width, height);
        }
    }

    void Screen::clear_framebuffers() {
        std::vector<std::weak_ptr<GlFramebuffer>> valid_framebuffers;

        for (const std::weak_ptr<GlFramebuffer>& wframebufer : framebuffers) {
            if (wframebufer.expired()) {
                continue;
            }

            valid_framebuffers.push_back(wframebufer);
        }

        framebuffers = std::move(valid_framebuffers);
    }
}
