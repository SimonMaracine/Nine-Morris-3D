#pragma once

#include <memory>
#include <array>

#include <glad/glad.h>

class Framebuffer {
public:
    enum class Type {
        Scene, Intermediate, DepthMap
    };

    Framebuffer(Type type, int width, int height, int samples, int color_attachment_count,
            bool resizable);
    ~Framebuffer();

    static std::shared_ptr<Framebuffer> create(Type type, int width, int height, int samples,
                                               int color_attachment_count, bool resizable);

    void bind() const;
    static void bind_default();

    GLuint get_color_attachment(unsigned int index) const;
    GLuint get_depth_attachment() const;
    GLuint get_id() const { return framebuffer; }

    void resize(int width, int height);
    int read_pixel(unsigned int attachment_index, int x, int y) const;
    void clear_red_integer_attachment(int index, int value) const;

    static void resolve_framebuffer(GLuint read_framebuffer, GLuint draw_framebuffer,
                                    int width, int height);

    bool resizable = false;  // TODO move to private
    // FIXME refactor framebuffers
private:
    void build();

    Type type;
    int width, height, samples;
    int color_attachment_count;

    GLuint framebuffer = 0;

    std::array<GLuint, 4> color_attachments = { 0, 0, 0, 0 };
    GLuint depth_attachment = 0;
};
