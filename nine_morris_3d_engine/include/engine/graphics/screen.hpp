#pragma once

#include <memory>
#include <vector>

#include "engine/graphics/opengl/framebuffer.hpp"

namespace sm {
    class Application;

    class Screen {
    public:
        Screen() = default;
        ~Screen() = default;

        Screen(const Screen&) = default;
        Screen& operator=(const Screen&) = default;
        Screen(Screen&&) noexcept = default;
        Screen& operator=(Screen&&) noexcept = default;

        void add_framebuffer(std::shared_ptr<GlFramebuffer> framebuffer);
    private:
        void resize(int width, int height);
        void clear_framebuffers();

        std::vector<std::weak_ptr<GlFramebuffer>> framebuffers;

        friend class Application;
    };
}
