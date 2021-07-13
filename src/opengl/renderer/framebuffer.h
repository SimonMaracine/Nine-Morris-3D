#pragma once

#include <memory>
#include <vector>

#include <glad/glad.h>

enum class TextureFormat {
    None,
    RGB8,
    RGBA8,
    DEPTH24_STENCIL8
};

struct Specification {
    unsigned int width, height;
    int samples = 1;

    std::vector<TextureFormat> attachments;
};

class Framebuffer {
public:
    Framebuffer(const Specification& specification);
    ~Framebuffer();

    static std::shared_ptr<Framebuffer> create(const Specification& specification);

    void bind() const;
    static void unbind();

    Specification& get_specification() { return specification; }

    void resize(unsigned int width, unsigned int height);
private:
    void build();

    GLuint framebuffer = 0;
    
    std::vector<TextureFormat> color_attachment_formats;
    TextureFormat depth_attachment_format = TextureFormat::None;

    std::vector<GLuint> color_attachments;
    GLuint depth_attachment = 0;

    Specification specification;
};
