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
    int width, height;

    // At least one of these two must be specified
    std::vector<Attachment> color_attachments;
    Attachment depth_attachment;

    int samples = 1;
    bool resizable = true;
    bool white_border_for_depth_texture = false;
};

class Framebuffer {
public:
    Framebuffer(const FramebufferSpecification& specification);
    ~Framebuffer();

    static std::shared_ptr<Framebuffer> create(const FramebufferSpecification& specification);

    void bind();
    static void bind_default();

    GLuint get_color_attachment(unsigned int index);
    GLuint get_depth_attachment();
    GLuint get_id() { return framebuffer; }
    const FramebufferSpecification& get_specification() { return specification; }

    // Usually called by application
    void resize(int width, int height);

    // Read pixels
    int read_pixel_red_integer(unsigned int attachment_index, int x, int y);
    void read_pixel_red_integer_pbo(unsigned int attachment_index, int x, int y);

    void clear_red_integer_attachment(int index, int value);

    // Resolve this to draw_framebuffer
    void resolve_framebuffer(GLuint draw_framebuffer, int width, int height);
private:
    void build();

    FramebufferSpecification specification;

    GLuint framebuffer = 0;

    // These can be texture or renderbuffer handles
    std::vector<GLuint> color_attachments;
    GLuint depth_attachment = 0;
};
