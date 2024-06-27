#pragma once

#include <memory>
#include <array>
#include <cstddef>

#include "engine/graphics/opengl/buffer.hpp"
#include "engine/graphics/opengl/framebuffer.hpp"

namespace sm {
    template<std::size_t BufferCount>
    class FramebufferReader {
    public:
        static_assert(BufferCount > 0);

        FramebufferReader() = default;
        FramebufferReader(
            const std::array<std::shared_ptr<GlPixelBuffer>, BufferCount>& buffers,
            std::shared_ptr<GlFramebuffer> framebuffer
        )
            : buffers(buffers), framebuffer(framebuffer) {}

        // Call this after the framebuffer is bound to begin reading the framebuffer
        void read(int attachment_index, int x, int y) {
            buffer_index = (buffer_index + 1) % BufferCount;
            next_buffer_index = (buffer_index + 1) % BufferCount;

            buffers[buffer_index]->bind();
            framebuffer->read_pixel_float_pbo(attachment_index, x, y);

            GlPixelBuffer::unbind();
        }

        // Call this as far as possible from read() to actually get the data
        template<typename T>
        void get(T** data) {
            buffers[next_buffer_index]->bind();

            buffers[next_buffer_index]->map_data();
            buffers[next_buffer_index]->template get_data<T>(data);
            buffers[next_buffer_index]->unmap_data();

            GlPixelBuffer::unbind();
        }
    private:
        // Store references to pixel buffers and framebuffers
        std::array<std::shared_ptr<GlPixelBuffer>, BufferCount> buffers;
        std::shared_ptr<GlFramebuffer> framebuffer;
        std::size_t buffer_index {};
        std::size_t next_buffer_index {};
    };
}
