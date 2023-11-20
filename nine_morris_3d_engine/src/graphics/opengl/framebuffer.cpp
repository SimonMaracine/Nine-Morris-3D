#include <vector>

#include <glad/glad.h>

#include "engine/application_base/panic.hpp"
#include "engine/graphics/opengl/framebuffer.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/assert.hpp"

namespace sm {
    static const GLenum COLOR_ATTACHMENTS[4] {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3
    };

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
            int width, int height, unsigned int index) {
        const bool multisampled {samples > 1};

        if (multisampled) {
            glTexImage2DMultisample(
                GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, width, height, GL_TRUE
            );
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // These need to be linear
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            float border_color[4] { 0.0f, 0.0f, 0.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

            glTexStorage2D(GL_TEXTURE_2D, 1, internal_format, width, height);
        }

        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, target(multisampled), texture, 0
        );
    }

    static void attach_depth_texture(GLuint texture, int samples, GLenum internal_format,
            GLenum attachment, int width, int height, bool white_border) {
        const bool multisampled {samples > 1};

        if (multisampled) {
            glTexImage2DMultisample(
                GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, width, height, GL_TRUE
            );
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // These need to be linear
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            if (white_border) {
                const float border_color[4] { 1.0f, 1.0f, 1.0f, 1.0f };
                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
            }

            glTexStorage2D(GL_TEXTURE_2D, 1, internal_format, width, height);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target(multisampled), texture, 0);
    }

    static void attach_color_renderbuffer(GLuint renderbuffer, int samples, GLenum internal_format,
            int width, int height, unsigned int index) {
        const bool multisampled {samples > 1};

        if (multisampled) {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internal_format, width, height);
        } else {
            glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);
        }

        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, renderbuffer
        );
    }

    static void attach_depth_renderbuffer(GLuint renderbuffer, int samples, GLenum internal_format,
            GLenum attachment, int width, int height) {
        const bool multisampled {samples > 1};

        if (multisampled) {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internal_format, width, height);
        } else {
            glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);
        }

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer);
    }

    static const char* print_framebuffer_status_message(GLenum status) {
        const char* message {nullptr};

        switch (status) {
            case GL_FRAMEBUFFER_UNDEFINED:
                message = "GL_FRAMEBUFFER_UNDEFINED";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                message = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                message = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                message = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                message = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                message = "GL_FRAMEBUFFER_UNSUPPORTED";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                message = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                message = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
                break;
            case GL_FRAMEBUFFER_COMPLETE:
                SM_ASSERT(false, "Don't call this function, if the status is GL_FRAMEBUFFER_COMPLETE");
                break;
            default:
                break;
        }

        return message;
    }

    GlFramebuffer::GlFramebuffer(const FramebufferSpecification& specification)
        : specification(specification) {
        SM_ASSERT(
            specification.samples == 1 || specification.samples == 2 || specification.samples == 4,
            "Invalid sample size"
        );

        if (specification.white_border_for_depth_texture) {
            SM_ASSERT(specification.depth_attachment.format != AttachmentFormat::None, "Invalid configuration");
            SM_ASSERT(specification.depth_attachment.type == AttachmentType::Texture, "Invalid configuration");
        }

        SM_ASSERT(specification.width > 0 && specification.height > 0, "Invalid size");

        SM_ASSERT(specification.clear_drawbuffer >= 0, "Invalid drawbuffer to clear");

        if (!specification.color_attachments.empty()) {
            SM_ASSERT(
                static_cast<std::size_t>(specification.clear_drawbuffer) < specification.color_attachments.size(),
                "Invalid drawbuffer to clear"
            );
        }

        build();

        LOG_DEBUG("Created GL framebuffer {}", framebuffer);
    }

    GlFramebuffer::~GlFramebuffer() {
        for (std::size_t i {0}; i < specification.color_attachments.size(); i++) {
            switch (specification.color_attachments[i].type) {
                case AttachmentType::None:
                    SM_ASSERT(false, "Attachment type None is invalid");
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
                    SM_ASSERT(false, "Attachment type None is invalid");
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

        LOG_DEBUG("Deleted GL framebuffer {}", framebuffer);
    }

    void GlFramebuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    }

    void GlFramebuffer::bind_default() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int GlFramebuffer::get_color_attachment(int attachment_index) const {
        SM_ASSERT(static_cast<std::size_t>(attachment_index) < color_attachments.size(), "Invalid color attachment");

        return color_attachments[attachment_index];
    }

    unsigned int GlFramebuffer::get_depth_attachment() const {
        return depth_attachment;
    }

    void GlFramebuffer::resize(int width, int height) {
        if (width < 1 || height < 1 || width > 8192 || height > 8192) {
            LOG_DIST_ERROR("Attempted to resize framebuffer to [{}, {}]", width, height);
            return;
        }

        specification.width = width / specification.resize_divisor;
        specification.height = height / specification.resize_divisor;

        build();
    }

    float GlFramebuffer::read_pixel_float(int attachment_index, int x, int y) const {
        SM_ASSERT(static_cast<std::size_t>(attachment_index) < color_attachments.size(), "Invalid color attachment");

        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
        float pixel;
        glReadPixels(x, y, 1, 1, GL_RED, GL_FLOAT, &pixel);

        return pixel;
    }

    void GlFramebuffer::read_pixel_float_pbo(int attachment_index, int x, int y) const {
        SM_ASSERT(static_cast<std::size_t>(attachment_index) < color_attachments.size(), "Invalid color attachment");

        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
        glReadPixels(x, y, 1, 1, GL_RED, GL_FLOAT, nullptr);
    }

    void GlFramebuffer::clear_color_attachment_float() const {  // TODO right now not used
        glClearBufferfv(GL_COLOR, specification.clear_drawbuffer, specification.color_clear_value);
    }

    void GlFramebuffer::blit(const GlFramebuffer* draw_framebuffer, int width, int height) const {
        SM_ASSERT(
            color_attachments.size() == draw_framebuffer->color_attachments.size(),
            "Framebuffers must have the same attachments"
        );

        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_framebuffer->framebuffer);

        for (std::size_t i {0}; i < color_attachments.size(); i++) {
            glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
            glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
            glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }

        glDrawBuffers(draw_framebuffer->color_attachments.size(), COLOR_ATTACHMENTS);
    }

    void GlFramebuffer::build() {
        // Delete old framebuffer first
        if (framebuffer != 0) {
            for (std::size_t i {0}; i < specification.color_attachments.size(); i++) {
                switch (specification.color_attachments[i].type) {
                    case AttachmentType::None:
                        SM_ASSERT(false, "Attachment type None is invalid");
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
                        SM_ASSERT(false, "Attachment type None is invalid");
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

        const bool multisampled {specification.samples > 1};

        color_attachments.resize(specification.color_attachments.size());

        for (std::size_t i {0}; i < specification.color_attachments.size(); i++) {
            switch (specification.color_attachments[i].type) {
                case AttachmentType::None:
                    SM_ASSERT(false, "Attachment type None is invalid");

                    break;
                case AttachmentType::Texture: {
                    unsigned int texture;
                    glGenTextures(1, &texture);
                    glBindTexture(target(multisampled), texture);

                    switch (specification.color_attachments[i].format) {
                        case AttachmentFormat::None:
                            SM_ASSERT(false, "Attachment format None is invalid");
                            break;
                        case AttachmentFormat::Rgba8:
                            attach_color_texture(
                                texture, specification.samples, GL_RGBA8,
                                specification.width, specification.height, i
                            );
                            break;
                        case AttachmentFormat::RedInt:
                            attach_color_texture(
                                texture, specification.samples, GL_R32I,
                                specification.width, specification.height, i
                            );
                            break;
                        case AttachmentFormat::RedFloat:
                            attach_color_texture(
                                texture, specification.samples, GL_R32F,
                                specification.width, specification.height, i
                            );
                            break;
                        default:
                            SM_ASSERT(false, "Wrong attachment format");
                            break;
                    }

                    color_attachments[i] = texture;
                    glBindTexture(target(multisampled), 0);

                    break;
                }
                case AttachmentType::Renderbuffer: {
                    unsigned int renderbuffer;
                    glGenRenderbuffers(1, &renderbuffer);
                    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);

                    switch (specification.color_attachments[i].format) {
                        case AttachmentFormat::None:
                            SM_ASSERT(false, "Attachment format None is invalid");
                            break;
                        case AttachmentFormat::Rgba8:
                            attach_color_renderbuffer(
                                renderbuffer, specification.samples, GL_RGBA8,
                                specification.width, specification.height, i
                            );
                            break;
                        case AttachmentFormat::RedInt:
                            attach_color_renderbuffer(
                                renderbuffer, specification.samples, GL_R32I,
                                specification.width, specification.height, i
                            );
                            break;
                        case AttachmentFormat::RedFloat:
                            attach_color_renderbuffer(
                                renderbuffer, specification.samples, GL_R32F,
                                specification.width, specification.height, i
                            );
                            break;
                        default:
                            SM_ASSERT(false, "Wrong attachment format");
                            break;
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
                    SM_ASSERT(false, "Attachment type None is invalid");

                    break;
                case AttachmentType::Texture: {
                    unsigned int texture;
                    glGenTextures(1, &texture);
                    glBindTexture(target(multisampled), texture);

                    switch (specification.depth_attachment.format) {
                        case AttachmentFormat::None:
                            SM_ASSERT(false, "Attachment format None is invalid");
                            break;
                        case AttachmentFormat::Depth24Stencil8:
                            attach_depth_texture(
                                texture, specification.samples,
                                GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, specification.width,
                                specification.height, specification.white_border_for_depth_texture
                            );
                            break;
                        case AttachmentFormat::Depth32:
                            attach_depth_texture(
                                texture, specification.samples,
                                GL_DEPTH_COMPONENT32, GL_DEPTH_ATTACHMENT, specification.width,
                                specification.height, specification.white_border_for_depth_texture
                            );
                            break;
                        default:
                            SM_ASSERT(false, "Wrong attachment format");
                            break;
                    }

                    depth_attachment = texture;
                    glBindTexture(target(multisampled), 0);

                    break;
                }
                case AttachmentType::Renderbuffer: {
                    unsigned int renderbuffer;
                    glGenRenderbuffers(1, &renderbuffer);
                    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);

                    switch (specification.depth_attachment.format) {
                        case AttachmentFormat::None:
                            SM_ASSERT(false, "Attachment format None is invalid");
                            break;
                        case AttachmentFormat::Depth24Stencil8:
                            attach_depth_renderbuffer(
                                renderbuffer, specification.samples,
                                GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT,
                                specification.width, specification.height
                            );
                            break;
                        case AttachmentFormat::Depth32:
                            attach_depth_renderbuffer(
                                renderbuffer, specification.samples,
                                GL_DEPTH_COMPONENT32, GL_DEPTH_ATTACHMENT,
                                specification.width, specification.height
                            );
                            break;
                        default:
                            SM_ASSERT(false, "Wrong attachment format");
                            break;
                    }

                    depth_attachment = renderbuffer;
                    glBindRenderbuffer(GL_RENDERBUFFER, 0);

                    break;
                }
            }
        }

        if (color_attachments.size() > 1) {
            SM_ASSERT(color_attachments.size() <= 4, "Currently there can be maximum 4 color attachments");

            glDrawBuffers(color_attachments.size(), COLOR_ATTACHMENTS);
        } else if (color_attachments.empty()) {
            glDrawBuffer(GL_NONE);  // TODO what is this?
            glReadBuffer(GL_NONE);
        }

        const GLenum status {glCheckFramebufferStatus(GL_FRAMEBUFFER)};

        if (status != GL_FRAMEBUFFER_COMPLETE) {
            LOG_DIST_CRITICAL("GL framebuffer {} is incomplete", framebuffer);
            LOG_DIST_CRITICAL("Status: {}", print_framebuffer_status_message(status));
            throw OtherError;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
