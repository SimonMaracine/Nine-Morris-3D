#include <memory>
#include <cassert>

#include <glad/glad.h>

#include "opengl/renderer/framebuffer.h"
#include "opengl/debug_opengl.h"
#include "other/logging.h"

Framebuffer::Framebuffer(Type type, int width, int height, int samples, int color_attachment_count)
        : type(type), width(width), height(height), samples(samples),
          color_attachment_count(color_attachment_count) {
    build();

    SPDLOG_DEBUG("Created framebuffer {}", framebuffer);
}

Framebuffer::~Framebuffer() {
    glDeleteTextures(color_attachments.size(), color_attachments.data());
    switch (type) {
        case Type::Scene:
        case Type::Intermediate:
            glDeleteRenderbuffers(1, &depth_attachment);
            break;
        case Type::DepthMap:
            glDeleteTextures(1, &depth_attachment);
            break;
    }
    glDeleteFramebuffers(1, &framebuffer);

    SPDLOG_DEBUG("Deleted framebuffer {}", framebuffer);
}

std::shared_ptr<Framebuffer> Framebuffer::create(Type type, int width, int height, int samples,
                                                 int color_attachments_count) {
    return std::make_shared<Framebuffer>(type, width, height, samples, color_attachments_count);
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
    this->width = width;
    this->height = height;

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

void Framebuffer::resolve_framebuffer(GLuint read_framebuffer, GLuint draw_framebuffer,
                                      int width, int height) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_framebuffer);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glDrawBuffer(GL_COLOR_ATTACHMENT1);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

static GLenum target(bool multisampled) {
    return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

static void attach_color_texture(GLuint texture, int samples, GLenum internal_format, GLenum format,
                                 int width, int height, unsigned int index) {
    bool multisampled = samples > 1;

    glBindTexture(target(multisampled), texture);

    if (multisampled) {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, width, height, GL_TRUE);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
        LOG_ALLOCATION(width * height * 4);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, target(multisampled),
                           texture, 0);
}

static void attach_depth_renderbuffer(GLuint renderbuffer, int samples, GLenum internal_format,
                                      GLenum attachment_type, int width, int height) {
    bool multisampled = samples > 1;

    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);

    if (multisampled) {
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internal_format, width, height);
    } else {
        glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);
        LOG_ALLOCATION(width * height * 4);
    }

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment_type, GL_RENDERBUFFER, renderbuffer);
}

static void attach_depth_shadow_texture(GLuint texture, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, nullptr);
    LOG_ALLOCATION(width * height * 4);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
}

void Framebuffer::build() {
    if (framebuffer) {
        glDeleteTextures(color_attachment_count, color_attachments.data());
        switch (type) {
            case Type::Scene:
            case Type::Intermediate:
                glDeleteRenderbuffers(1, &depth_attachment);
                break;
            case Type::DepthMap:
                glDeleteTextures(1, &depth_attachment);
                break;
        }
        glDeleteFramebuffers(1, &framebuffer);

        color_attachments.fill(0);
        depth_attachment = 0;
    }

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    switch (type) {
        case Type::Scene:
        case Type::Intermediate: {
            glGenTextures(color_attachment_count, color_attachments.data());
            glGenRenderbuffers(1, &depth_attachment);

            attach_color_texture(color_attachments[0], samples, GL_RGBA8, GL_BGRA, width, height, 0);
            attach_color_texture(color_attachments[1], samples, GL_R32I, GL_RED_INTEGER, width, height, 1);
            attach_depth_renderbuffer(depth_attachment, samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, width, height);

            GLenum attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                      GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(color_attachment_count, attachments);

            break;
        }
        case Type::DepthMap: {
            glGenTextures(1, &depth_attachment);

            attach_depth_shadow_texture(depth_attachment, width, height);

            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            break;
        }
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        spdlog::critical("Framebuffer {} is incomplete", framebuffer);
        std::exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
