#pragma once

#include <vector>

namespace sm {
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

        AttachmentFormat format {AttachmentFormat::None};
        AttachmentType type {AttachmentType::None};
    };

    struct FramebufferSpecification {
        // At least one of these two must be specified
        std::vector<Attachment> color_attachments;
        Attachment depth_attachment;

        // Must be specified
        int width {0};
        int height {0};

        // Color attachment clearing stuff
        struct {
            int drawbuffer {0};
            float value[4] {};
        } color_clear;

        int samples {1};
        unsigned int resize_divisor {1};
        bool resizable {true};
        bool white_border_for_depth_texture {false};
    };

    class GlFramebuffer {
    public:
        GlFramebuffer(const FramebufferSpecification& specification);
        ~GlFramebuffer();

        GlFramebuffer(const GlFramebuffer&) = delete;
        GlFramebuffer& operator=(const GlFramebuffer&) = delete;
        GlFramebuffer(GlFramebuffer&&) = delete;
        GlFramebuffer& operator=(GlFramebuffer&&) = delete;

        void bind() const;
        static void bind_default();

        unsigned int get_color_attachment(int attachment_index) const;
        unsigned int get_depth_attachment() const;
        const FramebufferSpecification& get_specification() const { return specification; }

        // Usually called by application
        void resize(int width, int height);

        // Read pixels from some buffer
        float read_pixel_float(int attachment_index, int x, int y) const;  // TODO read value is float; should be generic
        void read_pixel_float_pbo(int attachment_index, int x, int y) const;

        // Clear some buffer
        void clear_color_attachment_float() const;  // TODO should be generic

        // Resolve this to draw_framebuffer
        void blit(const GlFramebuffer* draw_framebuffer, int width, int height) const;
    private:
        void build();

        unsigned int framebuffer {0};

        // These can be texture or renderbuffer handles
        unsigned int depth_attachment {0};
        std::vector<unsigned int> color_attachments;

        FramebufferSpecification specification;
    };
}
