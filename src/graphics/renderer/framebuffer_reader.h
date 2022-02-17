#pragma once

#include <array>
#include <memory>

#include "graphics/renderer/buffer.h"
#include "graphics/renderer/framebuffer.h"

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

    void read(unsigned int attachment_index, int x, int y) {
        buffer_index = (buffer_index + 1) % BufferCount;
        next_buffer_index = (buffer_index + 1) % BufferCount;

        buffers[buffer_index]->bind();
        framebuffer->read_pixel_red_integer_pbo(attachment_index, x, y);
    }

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
