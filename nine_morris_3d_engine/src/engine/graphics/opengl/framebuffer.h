#pragma once

#include <glad/glad.h>

namespace sm {
    namespace gl {
        enum class AttachmentFormat {
            None,
            Rgba8,
            RedInt,
            RedFloat,
            Depth24Stencil8,
            Depth32
        };

        enum class AttachmentType {
            None,
            Texture,
            Renderbuffer
        };

        struct Attachment {
            Attachment() = default;
            Attachment(AttachmentFormat format, AttachmentType type)
                : format(format), type(type) {}

            AttachmentFormat format = AttachmentFormat::None;
            AttachmentType type = AttachmentType::None;
        };

        struct FramebufferSpecification {
            // Must be specified
            int width = 0;
            int height = 0;

            // At least one of these two must be specified
            std::vector<Attachment> color_attachments;
            Attachment depth_attachment;

            int samples = 1;
            bool resizable = true;
            unsigned int resize_divisor = 1;
            bool white_border_for_depth_texture = false;

            // Color attachment clearing stuff
            GLint clear_drawbuffer = 0;
            GLfloat color_clear_value[4];
        };

        class Framebuffer final {
        public:
            Framebuffer(const FramebufferSpecification& specification);
            ~Framebuffer();

            Framebuffer(const Framebuffer&) = delete;
            Framebuffer& operator=(const Framebuffer&) = delete;
            Framebuffer(Framebuffer&&) = delete;
            Framebuffer& operator=(Framebuffer&&) = delete;

            void bind();
            static void bind_default();

            GLuint get_color_attachment(GLint attachment_index);
            GLuint get_depth_attachment();
            const FramebufferSpecification& get_specification() { return specification; }

            // Usually called by application
            void resize(int width, int height);

            // Read pixels from some buffer
            float read_pixel_float(GLint attachment_index, int x, int y);  // TODO red value is float; should be generic
            void read_pixel_float_pbo(GLint attachment_index, int x, int y);

            // Clear some buffer
            void clear_color_attachment_float();  // TODO should be generic

            // Resolve this to draw_framebuffer
            void blit(Framebuffer* draw_framebuffer, int width, int height);
        private:
            void build();

            FramebufferSpecification specification;

            GLuint framebuffer = 0;

            // These can be texture or renderbuffer handles
            std::vector<GLuint> color_attachments;
            GLuint depth_attachment = 0;
        };
    }
}
