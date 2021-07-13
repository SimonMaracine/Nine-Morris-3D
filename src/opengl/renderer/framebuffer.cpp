#include <memory>
#include <cassert>

#include <glad/glad.h>

#include "opengl/renderer/framebuffer.h"
#include "other/logging.h"

Framebuffer::Framebuffer(const Specification& specification)
        : specification(specification) {
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
        if (format == TextureFormat::DEPTH24_STENCIL8) {
            depth_attachment_format = format;
        } else {
            color_attachment_formats.push_back(format);
        }
    }

    return std::make_shared<Framebuffer>(specification);
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(unsigned int width, unsigned int height) {
    if (width < 1 || height < 1 || width > 8192 || height > 8192) {
        SPDLOG_ERROR("Attempted to rezize framebuffer to [ {}, {} ]", width, height);
        return;
    }
    specification.width = width;
    specification.height = height;

    build();
}

static void attach_color_texture(GLuint texture, GLenum format, unsigned int width,
                                 unsigned int height, int index) {
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D,
                           texture, 0);
}

static void attach_depth_texture(GLuint texture, GLenum format, GLenum attachment_type,
                                 unsigned int width, unsigned int height) {
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, GL_TEXTURE_2D, texture, 0);
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
        color_attachments.reserve(color_attachment_formats.size());

        glGenTextures(color_attachment_formats.size(), color_attachments.data());

        for (int i = 0; i < color_attachment_formats.size(); i++) {
            glBindTexture(GL_TEXTURE_2D, color_attachments[i]);

            switch (color_attachment_formats[i]) {
                case TextureFormat::RGB8:
                    attach_color_texture(color_attachments[i], GL_RGB8,
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
            case TextureFormat::DEPTH24_STENCIL8:
                attach_depth_texture(depth_attachment, GL_DEPTH24_STENCIL8,
                                     GL_DEPTH_STENCIL_ATTACHMENT, specification.width,
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
    }

    if (!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)) {
        SPDLOG_CRITICAL("Framebuffer {} is incomplete", framebuffer);
        std::exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
