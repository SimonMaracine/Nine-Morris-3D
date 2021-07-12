#pragma once

#include <memory>

#include <glad/glad.h>

struct Specification {
    unsigned int width, height;
    unsigned int samples = 1;
};

class Framebuffer {
public:
    Framebuffer(GLuint framebuffer, GLuint color_attachment, GLuint depth_attachment,
                const Specification& specification);
    ~Framebuffer();

    static std::shared_ptr<Framebuffer> create(const Specification& specification);

    void bind() const;
    static void unbind();

    Specification& get_specification() { return specification; }
private:
    GLuint framebuffer;
    GLuint color_attachment, depth_attachment;

    Specification specification;
};
