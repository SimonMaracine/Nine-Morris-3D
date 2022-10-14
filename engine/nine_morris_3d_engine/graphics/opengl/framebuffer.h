#pragma once

#include <glad/glad.h>

enum class AttachmentFormat {
    None,
    RGBA8,
    RED_I,
    DEPTH24_STENCIL8,
    DEPTH32
};

enum class AttachmentType {
    None,
    Texture,
    Renderbuffer
};

struct Attachment {
    Attachment() = default;
    Attachment(AttachmentFormat format, AttachmentType type)
        : format(format), type(type) {}
    AttachmentFormat format = AttachmentFormat::None;
    AttachmentType type = AttachmentType::None;
};

struct FramebufferSpecification {
    // Must be specified
    int width = 0, height = 0;

    // At least one of these two must be specified
    std::vector<Attachment> color_attachments;
    Attachment depth_attachment;

    int samples = 1;
    bool resizable = true;
    unsigned int resize_divisor = 1;
    bool white_border_for_depth_texture = false;

    // Integer color attachment clearing stuff
    GLint clear_drawbuffer = 0;
    const GLint* clear_value = nullptr;
};

class Framebuffer {
public:
    Framebuffer(const FramebufferSpecification& specification);
    ~Framebuffer();

    void bind();
    static void bind_default();

    GLuint get_color_attachment(GLint attachment_index);
    GLuint get_depth_attachment();
    GLuint get_id() { return framebuffer; }
    const FramebufferSpecification& get_specification() { return specification; }

    // Usually called by application
    void resize(int width, int height);

    // Read pixels
    int read_pixel_red_integer(GLint attachment_index, int x, int y);
    void read_pixel_red_integer_pbo(GLint attachment_index, int x, int y);

    // Clear buffer
    void clear_integer_color_attachment();

    // Resolve this to draw_framebuffer
    void blit(Framebuffer* draw_framebuffer, int width, int height);
private:
    void build();

    FramebufferSpecification specification;

    GLuint framebuffer = 0;

    // These can be texture or renderbuffer handles
    std::vector<GLuint> color_attachments;
    GLuint depth_attachment = 0;

    static constexpr GLenum COLOR_ATTACHMENTS[4] = {
        GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
    };
};
