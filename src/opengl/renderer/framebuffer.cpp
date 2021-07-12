#include <memory>
#include <cassert>

#include <glad/glad.h>

#include "opengl/renderer/framebuffer.h"
#include "other/logging.h"

Framebuffer::Framebuffer(GLuint framebuffer, GLuint color_attachment,
                         GLuint depth_attachment, const Specification& specification)
        : framebuffer(framebuffer), color_attachment(color_attachment),
          depth_attachment(depth_attachment), specification(specification) {
    SPDLOG_DEBUG("Created framebuffer {}", framebuffer);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &framebuffer);

    SPDLOG_DEBUG("Deleted framebuffer {}", framebuffer);
}

std::shared_ptr<Framebuffer> Framebuffer::create(const Specification& specification) {
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    GLuint color_attachment;
    glGenTextures(1, &color_attachment);
    glBindTexture(GL_TEXTURE_2D, color_attachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, specification.width, specification.height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Attach the color texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           color_attachment, 0);

    GLuint depth_attachment;
    glGenTextures(1, &depth_attachment);
    glBindTexture(GL_TEXTURE_2D, depth_attachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, specification.width,
                 specification.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Attach the depth texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
                           depth_attachment, 0);

    if (!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)) {
        SPDLOG_CRITICAL("Framebuffer {} is incomplete", framebuffer);
        std::exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return std::make_shared<Framebuffer>(framebuffer, color_attachment, depth_attachment,
                                         specification);
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
