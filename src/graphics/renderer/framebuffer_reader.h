#pragma once

#include <array>
#include <memory>

#include "graphics/renderer/buffer.h"
#include "graphics/renderer/framebuffer.h"

/**
 * Use this to asynchronously read pixels from a framebuffer.
 * Pass the array of pixel buffers and the number of them.
 * Stack allocate objects of this class.
 */
template<unsigned int BufferCount>
class FramebufferReader {
public:
    FramebufferReader() = default;
    FramebufferReader(const std::array<std::shared_ptr<PixelBuffer>, BufferCount>& buffers,
            std::shared_ptr<Framebuffer> framebuffer)
        : buffers(buffers), framebuffer(framebuffer) {
        static_assert(BufferCount > 0);
    }
    ~FramebufferReader() = default;

    // Call this after the framebuffer is bound to begin reading the framebuffer
    void read(unsigned int attachment_index, int x, int y) {
        buffer_index = (buffer_index + 1) % BufferCount;
        next_buffer_index = (buffer_index + 1) % BufferCount;

        buffers[buffer_index]->bind();
        framebuffer->read_pixel_red_integer_pbo(attachment_index, x, y);
    }

    // Call this as far as possible from read() to actually get the data
    template<typename T>
    void get(T** data) {
        buffers[next_buffer_index]->bind();
        buffers[next_buffer_index]->map_data();
        buffers[next_buffer_index]->template get_data<T>(data);
        buffers[next_buffer_index]->unmap_data();

        PixelBuffer::unbind();
    }
private:
    std::array<std::shared_ptr<PixelBuffer>, BufferCount> buffers;
    std::shared_ptr<Framebuffer> framebuffer;
    unsigned int buffer_index = 0;
    unsigned int next_buffer_index = 0;
};
