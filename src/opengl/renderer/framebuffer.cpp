#include <memory>
#include <cassert>

#include <glad/glad.h>

#include "opengl/renderer/framebuffer.h"
#include "other/logging.h"

Framebuffer::Framebuffer(const std::vector<TextureFormat>& color_attachment_formats,
                         TextureFormat depth_attachment_format,
                         const Specification& specification)
        : color_attachment_formats(color_attachment_formats),
          depth_attachment_format(depth_attachment_format), specification(specification) {
    build();
    
    SPDLOG_DEBUG("Created framebuffer {}", framebuffer);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(color_attachments.size(), color_attachments.data());
    glDeleteTextures(1, &depth_attachment);

    SPDLOG_DEBUG("Deleted framebuffer {}", framebuffer);
}

std::shared_ptr<Framebuffer> Framebuffer::create(const Specification& specification) {
    std::vector<TextureFormat> color_attachment_formats;
    TextureFormat depth_attachment_format = TextureFormat::None;

    for (TextureFormat format : specification.attachments) {
        if (format == TextureFormat::Depth24Stencil8) {
            depth_attachment_format = format;
        } else if (format == TextureFormat::DepthForShadow) {
            depth_attachment_format = format;
        } else {
            color_attachment_formats.push_back(format);
        }
    }

    return std::make_shared<Framebuffer>(color_attachment_formats, depth_attachment_format,
                                         specification);
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void Framebuffer::bind_default() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Framebuffer::get_color_attachment(unsigned int index) const {
    assert(index < color_attachments.size());
    return color_attachments[index];
}

GLuint Framebuffer::get_depth_attachment() const {
    return depth_attachment;
}

void Framebuffer::resize(int width, int height) {
    if (width < 1 || height < 1 || width > 8192 || height > 8192) {
        SPDLOG_ERROR("Attempted to rezize framebuffer to [ {}, {} ]", width, height);
        return;
    }
    specification.width = width;
    specification.height = height;

    build();
}

int Framebuffer::read_pixel(unsigned int attachment_index, int x, int y) const {
    assert(attachment_index < color_attachments.size());

    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
    int pixel;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);

    return pixel;
}

void Framebuffer::clear_red_integer_attachment(int index, int value) const {
    assert(index < (int) color_attachments.size());

    glClearBufferiv(GL_COLOR, index, &value);
}

static void attach_color_texture(GLuint texture, GLenum internal_format, GLenum format,
                                 int width, int height, unsigned int index) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexStorage2D(GL_TEXTURE_2D, 1, internal_format, width, height);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D,
                           texture, 0);
}

static void attach_depth_texture(GLuint texture, GLenum format, GLenum attachment_type,
                                 int width, int height) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_DEPTH_STENCIL,
                 GL_UNSIGNED_INT_24_8, nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, GL_TEXTURE_2D, texture, 0);
}

static void attach_depth_shadow_texture(GLuint texture, int width, int height) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
}

void Framebuffer::build() {
    if (framebuffer) {
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(color_attachments.size(), color_attachments.data());
        glDeleteTextures(1, &depth_attachment);

        color_attachments.clear();
        depth_attachment = 0;
    }

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    if (!color_attachment_formats.empty()) {
        color_attachments.resize(color_attachment_formats.size());

        glGenTextures(color_attachments.size(), color_attachments.data());

        for (unsigned int i = 0; i < color_attachments.size(); i++) {
            glBindTexture(GL_TEXTURE_2D, color_attachments[i]);

            switch (color_attachment_formats[i]) {
                case TextureFormat::RGB8:
                    attach_color_texture(color_attachments[i], GL_RGB8, GL_RGB,
                                         specification.width, specification.height, i);
                    break;
                case TextureFormat::RGBA8:
                    attach_color_texture(color_attachments[i], GL_RGBA8, GL_RGBA,
                                         specification.width, specification.height, i);
                    break;
                case TextureFormat::RedInteger:
                    attach_color_texture(color_attachments[i], GL_R32I, GL_RED_INTEGER,
                                         specification.width, specification.height, i);
                    break;
                default:
                    spdlog::critical("Texture format unrecognized");
                    std::exit(1);
            }
        }
    }

    if (depth_attachment_format != TextureFormat::None) {
        glGenTextures(1, &depth_attachment);
        glBindTexture(GL_TEXTURE_2D, depth_attachment);

        switch (depth_attachment_format) {
            case TextureFormat::Depth24Stencil8:
                attach_depth_texture(depth_attachment, GL_DEPTH24_STENCIL8,
                                     GL_DEPTH_STENCIL_ATTACHMENT, specification.width,
                                     specification.height);
                break;
            case TextureFormat::DepthForShadow:
                attach_depth_shadow_texture(depth_attachment, specification.width,
                                            specification.height);
                break;
            default:
                spdlog::critical("Texture format unrecognized");
                std::exit(1);
        }
    }

    if (color_attachments.size() > 1) {
        assert(color_attachments.size() <= 4);

        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                              GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(color_attachments.size(), buffers);
    } else if (color_attachments.empty()) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        spdlog::critical("Framebuffer {} is incomplete", framebuffer);
        std::exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
