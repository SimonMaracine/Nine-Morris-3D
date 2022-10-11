#include <glad/glad.h>

#include "nine_morris_3d_engine/graphics/debug_opengl.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/framebuffer.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"

static GLenum target(bool multisampled) {
    return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

static bool depth_attachment_present(const FramebufferSpecification& specification) {
    return (
        specification.depth_attachment.format != AttachmentFormat::None
        && specification.depth_attachment.type != AttachmentType::None
    );
}

static void attach_color_texture(GLuint texture, int samples, GLenum internal_format,
        GLenum format, int width, int height, unsigned int index) {
    const bool multisampled = samples > 1;

    if (multisampled) {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format,
                width, height, GL_TRUE);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,
                GL_UNSIGNED_BYTE, nullptr);
        LOG_ALLOCATION(width * height * 4)
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
            target(multisampled), texture, 0);
}

static void attach_depth_texture(GLuint texture, int samples, GLenum internal_format,
        GLenum format, GLenum attachment, int width, int height, bool white_border) {
    const bool multisampled = samples > 1;

    if (multisampled) {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format,
                width, height, GL_TRUE);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        if (white_border) {
            const float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,
                GL_UNSIGNED_BYTE, nullptr);
        LOG_ALLOCATION(width * height * 4)
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target(multisampled), texture, 0);
}

static void attach_color_renderbuffer(GLuint renderbuffer, int samples, GLenum internal_format,
        int width, int height, unsigned int index) {
    const bool multisampled = samples > 1;

    if (multisampled) {
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internal_format, width, height);
    } else {
        glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);
        LOG_ALLOCATION(width * height * 4)
    }

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER,
            renderbuffer);
}

static void attach_depth_renderbuffer(GLuint renderbuffer, int samples, GLenum internal_format,
        GLenum attachment, int width, int height) {
    const bool multisampled = samples > 1;

    if (multisampled) {
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internal_format, width, height);
    } else {
        glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);
        LOG_ALLOCATION(width * height * 4)
    }

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer);
}

Framebuffer::Framebuffer(const FramebufferSpecification& specification)
    : specification(specification) {
    ASSERT(
        specification.samples == 1 || specification.samples == 2 || specification.samples == 4,
        "Invalid sample size"
    );

    if (specification.white_border_for_depth_texture) {
        ASSERT(specification.depth_attachment.format != AttachmentFormat::None, "Invalid configuration");
        ASSERT(specification.depth_attachment.type == AttachmentType::Texture, "Invalid configuration");
    }

    ASSERT(specification.width > 0 && specification.height > 0, "Invalid size");

    ASSERT(specification.clear_drawbuffer >= 0, "Invalid drawbuffer to clear");

    if (!specification.color_attachments.empty()) {
        ASSERT(
            static_cast<size_t>(specification.clear_drawbuffer) < specification.color_attachments.size(),
            "Invalid drawbuffer to clear"
        );
    }

    build();

    DEB_DEBUG("Created framebuffer {}", framebuffer);
}

Framebuffer::~Framebuffer() {
    for (size_t i = 0; i < specification.color_attachments.size(); i++) {
        switch (specification.color_attachments[i].type) {
            case AttachmentType::None:
                ASSERT(false, "Attachment type 'None' is invalid");
                break;
            case AttachmentType::Texture:
                glDeleteTextures(1, &color_attachments[i]);
                break;
            case AttachmentType::Renderbuffer:
                glDeleteRenderbuffers(1, &color_attachments[i]);
                break;
        }
    }

    if (depth_attachment_present(specification)) {
        switch (specification.depth_attachment.type) {
            case AttachmentType::None:
                ASSERT(false, "Attachment type 'None' is invalid");
                break;
            case AttachmentType::Texture:
                glDeleteTextures(1, &depth_attachment);
                break;
            case AttachmentType::Renderbuffer:
                glDeleteRenderbuffers(1, &depth_attachment);
                break;
        }
    }

    glDeleteFramebuffers(1, &framebuffer);

    DEB_DEBUG("Deleted framebuffer {}", framebuffer);
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void Framebuffer::bind_default() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Framebuffer::get_color_attachment(unsigned int index) {
    ASSERT(index < color_attachments.size(), "Invalid color attachment");

    return color_attachments[index];
}

GLuint Framebuffer::get_depth_attachment() {
    return depth_attachment;
}

void Framebuffer::resize(int width, int height) {
    if (width < 1 || height < 1 || width > 8192 || height > 8192) {
        DEB_ERROR("Attempted to resize framebuffer to [{}, {}]", width, height);
        return;
    }

    specification.width = width / specification.resize_divisor;
    specification.height = height / specification.resize_divisor;

    build();
}

int Framebuffer::read_pixel_red_integer(unsigned int attachment_index, int x, int y) {
    ASSERT(attachment_index < color_attachments.size(), "Invalid color attachment");

    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
    int pixel;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);

    return pixel;
}

void Framebuffer::read_pixel_red_integer_pbo(unsigned int attachment_index, int x, int y) {
    ASSERT(attachment_index < color_attachments.size(), "Invalid color attachment");

    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, nullptr);
}

void Framebuffer::clear_integer_color_attachment() {
    glClearBufferiv(GL_COLOR, specification.clear_drawbuffer, specification.clear_value);
}

void Framebuffer::resolve_framebuffer(GLuint draw_framebuffer, int width, int height) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_framebuffer);

    for (size_t i = 0; i < color_attachments.size(); i++) {
        glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}

void Framebuffer::build() {
    // Delete old framebuffer first
    if (framebuffer != 0) {
        for (size_t i = 0; i < specification.color_attachments.size(); i++) {
            switch (specification.color_attachments[i].type) {
                case AttachmentType::None:
                    ASSERT(false, "Attachment type 'None' is invalid");
                    break;
                case AttachmentType::Texture:
                    glDeleteTextures(1, &color_attachments[i]);
                    break;
                case AttachmentType::Renderbuffer:
                    glDeleteRenderbuffers(1, &color_attachments[i]);
                    break;
            }
        }

        if (depth_attachment_present(specification)) {
            switch (specification.depth_attachment.type) {
                case AttachmentType::None:
                    ASSERT(false, "Attachment type 'None' is invalid");
                    break;
                case AttachmentType::Texture:
                    glDeleteTextures(1, &depth_attachment);
                    break;
                case AttachmentType::Renderbuffer:
                    glDeleteRenderbuffers(1, &depth_attachment);
                    break;
            }
        }

        glDeleteFramebuffers(1, &framebuffer);

        color_attachments.clear();
        depth_attachment = 0;
    }

    // Then create a new framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    const bool multisampled = specification.samples > 1;

    color_attachments.resize(specification.color_attachments.size());

    for (size_t i = 0; i < specification.color_attachments.size(); i++) {
        switch (specification.color_attachments[i].type) {
            case AttachmentType::None:
                ASSERT(false, "Attachment type 'None' is invalid");
                break;
            case AttachmentType::Texture: {
                GLuint texture;
                glGenTextures(1, &texture);
                glBindTexture(target(multisampled), texture);

                switch (specification.color_attachments[i].format) {
                    case AttachmentFormat::None:
                        ASSERT(false, "Attachment format 'None' is invalid");
                        break;
                    case AttachmentFormat::RGBA8:
                        attach_color_texture(texture, specification.samples, GL_RGBA8, GL_BGRA,
                                specification.width, specification.height, i);
                        break;
                    case AttachmentFormat::RED_I:
                        attach_color_texture(texture, specification.samples, GL_R32I, GL_RED_INTEGER,
                                specification.width, specification.height, i);
                        break;
                    default:
                        REL_CRITICAL("Wrong attachment format, exiting...");
                        exit(1);
                }

                color_attachments[i] = texture;
                glBindTexture(target(multisampled), 0);

                break;
            }
            case AttachmentType::Renderbuffer: {
                GLuint renderbuffer;
                glGenRenderbuffers(1, &renderbuffer);
                glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);

                switch (specification.color_attachments[i].format) {
                    case AttachmentFormat::None:
                        ASSERT(false, "Attachment format 'None' is invalid");
                        break;
                    case AttachmentFormat::RGBA8:
                        attach_color_renderbuffer(renderbuffer, specification.samples, GL_RGBA8,
                                specification.width, specification.height, i);
                        break;
                    case AttachmentFormat::RED_I:
                        attach_color_renderbuffer(renderbuffer, specification.samples, GL_R32I,
                                specification.width, specification.height, i);
                        break;
                    default:
                        REL_CRITICAL("Wrong attachment format, exiting...");
                        exit(1);
                }

                color_attachments[i] = renderbuffer;
                glBindRenderbuffer(GL_RENDERBUFFER, 0);

                break;
            }   
        }
    }

    if (depth_attachment_present(specification)) {
        switch (specification.depth_attachment.type) {
            case AttachmentType::None:
                ASSERT(false, "Attachment type 'None' is invalid");
                break;
            case AttachmentType::Texture: {
                GLuint texture;
                glGenTextures(1, &texture);
                glBindTexture(target(multisampled), texture);

                switch (specification.depth_attachment.format) {
                    case AttachmentFormat::None:
                        ASSERT(false, "Attachment format 'None' is invalid");
                        break;
                    case AttachmentFormat::DEPTH24_STENCIL8:
                        attach_depth_texture(texture, specification.samples, GL_DEPTH24_STENCIL8,
                                GL_DEPTH_STENCIL, GL_DEPTH_STENCIL_ATTACHMENT, specification.width,
                                specification.height, specification.white_border_for_depth_texture);
                        break;
                    case AttachmentFormat::DEPTH32:
                        attach_depth_texture(texture, specification.samples, GL_DEPTH_COMPONENT32,
                                GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, specification.width,
                                specification.height, specification.white_border_for_depth_texture);
                        break;
                    default:
                        REL_CRITICAL("Wrong attachment format, exiting...");
                        exit(1);
                }

                depth_attachment = texture;
                glBindTexture(target(multisampled), 0);

                break;
            }
            case AttachmentType::Renderbuffer: {
                GLuint renderbuffer;
                glGenRenderbuffers(1, &renderbuffer);
                glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);

                switch (specification.depth_attachment.format) {
                    case AttachmentFormat::None:
                        ASSERT(false, "Attachment format 'None' is invalid");
                        break;
                    case AttachmentFormat::DEPTH24_STENCIL8:
                        attach_depth_renderbuffer(renderbuffer, specification.samples,
                                GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT,
                                specification.width, specification.height);
                        break;
                    case AttachmentFormat::DEPTH32:
                        attach_depth_renderbuffer(renderbuffer, specification.samples,
                                GL_DEPTH_COMPONENT32, GL_DEPTH_ATTACHMENT,
                                specification.width, specification.height);
                        break;
                    default:
                        REL_CRITICAL("Wrong attachment format, exiting...");
                        exit(1);
                }

                depth_attachment = renderbuffer;
                glBindRenderbuffer(GL_RENDERBUFFER, 0);

                break;
            }   
        }
    }

    if (color_attachments.size() > 1) {
        ASSERT(color_attachments.size() <= 4, "Currently there can be maximum 4 color attachments");

        constexpr GLenum attachments[4] = {
            GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
        };
        glDrawBuffers(color_attachments.size(), attachments);
    } else if (color_attachments.empty()) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        REL_CRITICAL("Framebuffer {} is incomplete, exiting...", framebuffer);
        exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);   
}
