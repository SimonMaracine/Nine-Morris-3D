#include "nine_morris_3d_engine/graphics/opengl/framebuffer.hpp"

#include <cassert>

#include <glad/glad.h>

#include "nine_morris_3d_engine/application/internal/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm {
    static const unsigned int COLOR_ATTACHMENTS[] {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3
    };

    static unsigned int target(bool multisampled) {
        return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }

    static bool depth_attachment_present(const FramebufferSpecification& m_specification) {
        return (
            m_specification.depth_attachment.format != AttachmentFormat::None &&
            m_specification.depth_attachment.type != AttachmentType::None
        );
    }

    static void attach_color_texture(
        unsigned int texture,
        int samples,
        unsigned int internal_format,
        int width,
        int height,
        unsigned int index
    ) {
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

            const float border_color[4] { 0.0f, 0.0f, 0.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

            glTexStorage2D(GL_TEXTURE_2D, 1, internal_format, width, height);
        }

        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, target(multisampled), texture, 0
        );
    }

    static void attach_depth_texture(
        unsigned int texture,
        int samples,
        unsigned int internal_format,
        unsigned int attachment,
        int width,
        int height,
        bool white_border,
        bool comparison_mode
    ) {
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

            if (comparison_mode) {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            }

            glTexStorage2D(GL_TEXTURE_2D, 1, internal_format, width, height);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target(multisampled), texture, 0);
    }

    static void attach_color_renderbuffer(
        unsigned int renderbuffer,
        int samples,
        unsigned int internal_format,
        int width,
        int height,
        unsigned int index
    ) {
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

    static void attach_depth_renderbuffer(
        unsigned int renderbuffer,
        int samples,
        unsigned int internal_format,
        unsigned int attachment,
        int width,
        int height
    ) {
        const bool multisampled {samples > 1};

        if (multisampled) {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internal_format, width, height);
        } else {
            glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);
        }

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer);
    }

    static const char* framebuffer_status_message(GLenum status) {
        const char* message {};

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
                assert(false);
                break;
            default:
                break;
        }

        return message;
    }

    GlFramebuffer::GlFramebuffer(const FramebufferSpecification& m_specification)
        : m_specification(m_specification) {
        assert(m_specification.samples == 1 || m_specification.samples == 2 || m_specification.samples == 4);

        if (m_specification.white_border_depth_texture) {
            assert(m_specification.depth_attachment.format != AttachmentFormat::None);
            assert(m_specification.depth_attachment.type == AttachmentType::Texture);
            assert(m_specification.samples == 1);
        }

        if (m_specification.comparison_mode_depth_texture) {
            assert(m_specification.depth_attachment.format != AttachmentFormat::None);
            assert(m_specification.depth_attachment.type == AttachmentType::Texture);
            assert(m_specification.samples == 1);
        }

        assert(m_specification.width > 0 && m_specification.height > 0);

        build();

        LOG_DEBUG("Created GL framebuffer {}", m_framebuffer);
    }

    GlFramebuffer::~GlFramebuffer() {
        for (std::size_t i {0}; i < m_specification.color_attachments.size(); i++) {
            switch (m_specification.color_attachments[i].type) {
                case AttachmentType::None:
                    assert(false);
                    break;
                case AttachmentType::Texture:
                    glDeleteTextures(1, &m_color_attachments[i]);
                    break;
                case AttachmentType::Renderbuffer:
                    glDeleteRenderbuffers(1, &m_color_attachments[i]);
                    break;
            }
        }

        if (depth_attachment_present(m_specification)) {
            switch (m_specification.depth_attachment.type) {
                case AttachmentType::None:
                    assert(false);
                    break;
                case AttachmentType::Texture:
                    glDeleteTextures(1, &m_depth_attachment);
                    break;
                case AttachmentType::Renderbuffer:
                    glDeleteRenderbuffers(1, &m_depth_attachment);
                    break;
            }
        }

        glDeleteFramebuffers(1, &m_framebuffer);

        LOG_DEBUG("Deleted GL framebuffer {}", m_framebuffer);
    }

    void GlFramebuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    }

    void GlFramebuffer::bind_default() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int GlFramebuffer::get_color_attachment(int attachment_index) const {
        assert(static_cast<std::size_t>(attachment_index) < m_color_attachments.size());

        return m_color_attachments[attachment_index];
    }

    unsigned int GlFramebuffer::get_depth_attachment() const {
        return m_depth_attachment;
    }

    const FramebufferSpecification& GlFramebuffer::get_specification() const {
        return m_specification;
    }

    void GlFramebuffer::resize(int width, int height) {
        if (width < 1 || height < 1 || width > 8192 || height > 8192) {
            LOG_DIST_ERROR("Attempted to resize framebuffer to [{}, {}]", width, height);
            return;
        }

        m_specification.width = width / m_specification.resize_divisor;
        m_specification.height = height / m_specification.resize_divisor;

        build();
    }

    float GlFramebuffer::read_pixel_float(int attachment_index, int x, int y) const {
        assert(static_cast<std::size_t>(attachment_index) < m_color_attachments.size());

        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
        float pixel;
        glReadPixels(x, y, 1, 1, GL_RED, GL_FLOAT, &pixel);

        return pixel;
    }

    void GlFramebuffer::read_pixel_float_pbo(int attachment_index, int x, int y) const {
        assert(static_cast<std::size_t>(attachment_index) < m_color_attachments.size());

        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
        glReadPixels(x, y, 1, 1, GL_RED, GL_FLOAT, nullptr);
    }

    void GlFramebuffer::blit(const GlFramebuffer* draw_framebuffer, int width, int height) const {
        assert(m_color_attachments.size() == draw_framebuffer->m_color_attachments.size());

        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_framebuffer->m_framebuffer);

        for (std::size_t i {0}; i < m_color_attachments.size(); i++) {
            glReadBuffer(GL_COLOR_ATTACHMENT0 + static_cast<unsigned int>(i));
            glDrawBuffer(GL_COLOR_ATTACHMENT0 + static_cast<unsigned int>(i));
            glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }

        glDrawBuffers(static_cast<int>(draw_framebuffer->m_color_attachments.size()), COLOR_ATTACHMENTS);
    }

    void GlFramebuffer::build() {
        // Delete old framebuffer first
        if (m_framebuffer != 0) {
            for (std::size_t i {0}; i < m_specification.color_attachments.size(); i++) {
                switch (m_specification.color_attachments[i].type) {
                    case AttachmentType::None:
                        assert(false);
                        break;
                    case AttachmentType::Texture:
                        glDeleteTextures(1, &m_color_attachments[i]);
                        break;
                    case AttachmentType::Renderbuffer:
                        glDeleteRenderbuffers(1, &m_color_attachments[i]);
                        break;
                }
            }

            if (depth_attachment_present(m_specification)) {
                switch (m_specification.depth_attachment.type) {
                    case AttachmentType::None:
                        assert(false);
                        break;
                    case AttachmentType::Texture:
                        glDeleteTextures(1, &m_depth_attachment);
                        break;
                    case AttachmentType::Renderbuffer:
                        glDeleteRenderbuffers(1, &m_depth_attachment);
                        break;
                }
            }

            glDeleteFramebuffers(1, &m_framebuffer);

            m_color_attachments.clear();
            m_depth_attachment = 0;
        }

        // Then create a new framebuffer
        glGenFramebuffers(1, &m_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

        const bool multisampled {m_specification.samples > 1};

        m_color_attachments.resize(m_specification.color_attachments.size());

        for (std::size_t i {0}; i < m_specification.color_attachments.size(); i++) {
            switch (m_specification.color_attachments[i].type) {
                case AttachmentType::None:
                    assert(false);

                    break;
                case AttachmentType::Texture: {
                    unsigned int texture;
                    glGenTextures(1, &texture);
                    glBindTexture(target(multisampled), texture);

                    switch (m_specification.color_attachments[i].format) {
                        case AttachmentFormat::None:
                            assert(false);
                            break;
                        case AttachmentFormat::Rgba8:
                            attach_color_texture(
                                texture,
                                m_specification.samples,
                                GL_RGBA8,
                                m_specification.width,
                                m_specification.height,
                                static_cast<unsigned int>(i)
                            );
                            break;
                        case AttachmentFormat::Rgba8Float:
                            attach_color_texture(
                                texture,
                                m_specification.samples,
                                GL_RGBA16F,
                                m_specification.width,
                                m_specification.height,
                                static_cast<unsigned int>(i)
                            );
                            break;
                        case AttachmentFormat::RedInt:
                            attach_color_texture(
                                texture,
                                m_specification.samples,
                                GL_R32I,
                                m_specification.width,
                                m_specification.height,
                                static_cast<unsigned int>(i)
                            );
                            break;
                        case AttachmentFormat::RedFloat:
                            attach_color_texture(
                                texture,
                                m_specification.samples,
                                GL_R32F,
                                m_specification.width,
                                m_specification.height,
                                static_cast<unsigned int>(i)
                            );
                            break;
                        default:
                            assert(false);
                            break;
                    }

                    m_color_attachments[i] = texture;
                    glBindTexture(target(multisampled), 0);

                    break;
                }
                case AttachmentType::Renderbuffer: {
                    unsigned int renderbuffer;
                    glGenRenderbuffers(1, &renderbuffer);
                    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);

                    switch (m_specification.color_attachments[i].format) {
                        case AttachmentFormat::None:
                            assert(false);
                            break;
                        case AttachmentFormat::Rgba8:
                            attach_color_renderbuffer(
                                renderbuffer,
                                m_specification.samples,
                                GL_RGBA8,
                                m_specification.width,
                                m_specification.height,
                                static_cast<unsigned int>(i)
                            );
                            break;
                        case AttachmentFormat::Rgba8Float:
                            attach_color_renderbuffer(
                                renderbuffer,
                                m_specification.samples,
                                GL_RGBA16F,
                                m_specification.width,
                                m_specification.height,
                                static_cast<unsigned int>(i)
                            );
                            break;
                        case AttachmentFormat::RedInt:
                            attach_color_renderbuffer(
                                renderbuffer,
                                m_specification.samples,
                                GL_R32I,
                                m_specification.width,
                                m_specification.height,
                                static_cast<unsigned int>(i)
                            );
                            break;
                        case AttachmentFormat::RedFloat:
                            attach_color_renderbuffer(
                                renderbuffer,
                                m_specification.samples,
                                GL_R32F,
                                m_specification.width,
                                m_specification.height,
                                static_cast<unsigned int>(i)
                            );
                            break;
                        default:
                            assert(false);
                            break;
                    }

                    m_color_attachments[i] = renderbuffer;
                    glBindRenderbuffer(GL_RENDERBUFFER, 0);

                    break;
                }
            }
        }

        if (depth_attachment_present(m_specification)) {
            switch (m_specification.depth_attachment.type) {
                case AttachmentType::None:
                    assert(false);

                    break;
                case AttachmentType::Texture: {
                    unsigned int texture;
                    glGenTextures(1, &texture);
                    glBindTexture(target(multisampled), texture);

                    switch (m_specification.depth_attachment.format) {
                        case AttachmentFormat::None:
                            assert(false);
                            break;
                        case AttachmentFormat::Depth24Stencil8:
                            attach_depth_texture(
                                texture, m_specification.samples,
                                GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_specification.width,
                                m_specification.height, m_specification.white_border_depth_texture,
                                m_specification.comparison_mode_depth_texture
                            );
                            break;
                        case AttachmentFormat::Depth32:
                            attach_depth_texture(
                                texture, m_specification.samples,
                                GL_DEPTH_COMPONENT32, GL_DEPTH_ATTACHMENT, m_specification.width,
                                m_specification.height, m_specification.white_border_depth_texture,
                                m_specification.comparison_mode_depth_texture
                            );
                            break;
                        default:
                            assert(false);
                            break;
                    }

                    m_depth_attachment = texture;
                    glBindTexture(target(multisampled), 0);

                    break;
                }
                case AttachmentType::Renderbuffer: {
                    unsigned int renderbuffer;
                    glGenRenderbuffers(1, &renderbuffer);
                    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);

                    switch (m_specification.depth_attachment.format) {
                        case AttachmentFormat::None:
                            assert(false);
                            break;
                        case AttachmentFormat::Depth24Stencil8:
                            attach_depth_renderbuffer(
                                renderbuffer, m_specification.samples,
                                GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT,
                                m_specification.width, m_specification.height
                            );
                            break;
                        case AttachmentFormat::Depth32:
                            attach_depth_renderbuffer(
                                renderbuffer, m_specification.samples,
                                GL_DEPTH_COMPONENT32, GL_DEPTH_ATTACHMENT,
                                m_specification.width, m_specification.height
                            );
                            break;
                        default:
                            assert(false);
                            break;
                    }

                    m_depth_attachment = renderbuffer;
                    glBindRenderbuffer(GL_RENDERBUFFER, 0);

                    break;
                }
            }
        }

        if (m_color_attachments.size() > 1) {
            assert(m_color_attachments.size() <= std::size(COLOR_ATTACHMENTS));

            glDrawBuffers(static_cast<int>(m_color_attachments.size()), COLOR_ATTACHMENTS);
        } else if (m_color_attachments.empty()) {
            glDrawBuffer(GL_NONE);  // TODO what is this?
        }

        const unsigned int status {glCheckFramebufferStatus(GL_FRAMEBUFFER)};

        if (status != GL_FRAMEBUFFER_COMPLETE) {
            SM_THROW_ERROR(
                internal::OtherError,
                "GL framebuffer {} is incomplete: {}",
                m_framebuffer,
                framebuffer_status_message(status)
            );
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
