#pragma once

#include <memory>
#include <array>
#include <cstddef>

#include "nine_morris_3d_engine/graphics/opengl/buffer.hpp"
#include "nine_morris_3d_engine/graphics/opengl/framebuffer.hpp"

namespace sm {
    // Object used to manage screen raeding
    template<std::size_t BufferCount>
    class FramebufferReader {
    public:
        static_assert(BufferCount > 0);

        FramebufferReader() = default;
        FramebufferReader(
            const std::array<std::shared_ptr<GlPixelBuffer>, BufferCount>& buffers,
            std::shared_ptr<GlFramebuffer> framebuffer
        )
            : m_buffers(buffers), m_framebuffer(framebuffer) {}

        // Call this after the framebuffer is bound to begin reading the framebuffer
        void read(int attachment_index, int x, int y) {
            m_buffer_index = (m_buffer_index + 1) % BufferCount;
            m_next_buffer_index = (m_buffer_index + 1) % BufferCount;

            m_buffers[m_buffer_index]->bind();
            m_framebuffer->read_pixel_float_pbo(attachment_index, x, y);

            GlPixelBuffer::unbind();
        }

        // Call this as far as possible from read() to actually get the data
        template<typename T>
        void get(T** data) {
            m_buffers[m_next_buffer_index]->bind();

            m_buffers[m_next_buffer_index]->map_data();
            m_buffers[m_next_buffer_index]->template get_data<T>(data);
            m_buffers[m_next_buffer_index]->unmap_data();

            GlPixelBuffer::unbind();
        }
    private:
        // Store references to pixel buffers and framebuffers
        std::array<std::shared_ptr<GlPixelBuffer>, BufferCount> m_buffers;
        std::shared_ptr<GlFramebuffer> m_framebuffer;
        std::size_t m_buffer_index {};
        std::size_t m_next_buffer_index {};
    };
}
