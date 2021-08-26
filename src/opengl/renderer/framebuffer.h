#pragma once

#include <memory>
#include <vector>

#include <glad/glad.h>

enum class TextureFormat {
    None,
    RGB8,
    RGBA8,
    Depth24Stencil8,
    DepthForShadow,
    RedInteger
};

struct Specification {
    int width, height;
    int samples = 1;

    std::vector<TextureFormat> attachments;
};

class Framebuffer {
public:
    Framebuffer(const std::vector<TextureFormat>& color_attachment_formats,
                TextureFormat depth_attachment_format, const Specification& specification);
    ~Framebuffer();

    static std::shared_ptr<Framebuffer> create(const Specification& specification);

    void bind() const;
    static void bind_default();

    const Specification& get_specification() const { return specification; }
    GLuint get_color_attachment(unsigned int index) const;
    GLuint get_depth_attachment() const;

    void resize(int width, int height);
    int read_pixel(unsigned int attachment_index, int x, int y) const;
    void clear_red_integer_attachment(int index, int value) const;
private:
    void build();

    GLuint framebuffer = 0;

    std::vector<TextureFormat> color_attachment_formats;
    TextureFormat depth_attachment_format = TextureFormat::None;

    std::vector<GLuint> color_attachments;
    GLuint depth_attachment = 0;

    Specification specification;
};
