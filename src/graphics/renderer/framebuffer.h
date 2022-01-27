#pragma once

#include <memory>
#include <vector>

#include <glad/glad.h>

enum class AttachmentFormat {
    RGBA8,
    RED_I,
    DEPTH24_STENCIL8,
    DEPTH32
};

enum class AttachmentType {
    Texture,
    Renderbuffer
};

struct Attachment {
    Attachment() = default;
    Attachment(AttachmentFormat format, AttachmentType type)
        : format(format), type(type) {}
    AttachmentFormat format;
    AttachmentType type;
};

struct FramebufferSpecification {
    // Must be specified
    int width, height;

    // At least one of these must be specified
    std::vector<Attachment> color_attachments;
    Attachment depth_attachment;

    bool enable_depth_attachment = false;
    int samples = 1;
    bool resizable = true;
    bool white_border_for_depth_texture = false;
};

class Framebuffer {
public:
    Framebuffer(const FramebufferSpecification& specification);
    ~Framebuffer();

    static std::shared_ptr<Framebuffer> create(const FramebufferSpecification& specification);

    void bind() const;
    static void bind_default();

    GLuint get_color_attachment(unsigned int index) const;
    GLuint get_depth_attachment() const;
    GLuint get_id() const { return framebuffer; }
    const FramebufferSpecification& get_specification() const { return specification; }

    void resize(int width, int height);
    int read_pixel_red_integer(unsigned int attachment_index, int x, int y) const;
    void clear_red_integer_attachment(int index, int value) const;

    void resolve_framebuffer(GLuint draw_framebuffer, int width, int height);
private:
    void build();

    FramebufferSpecification specification;

    GLuint framebuffer = 0;

    // These can be texture or renderbuffer handles
    std::vector<GLuint> color_attachments;
    GLuint depth_attachment = 0;
};
