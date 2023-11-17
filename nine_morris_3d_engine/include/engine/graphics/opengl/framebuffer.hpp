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

        AttachmentFormat format = AttachmentFormat::None;
        AttachmentType type = AttachmentType::None;
    };

    struct FramebufferSpecification {
        // Must be specified
        int width {0};  // TODO reorder members
        int height {0};

        // At least one of these two must be specified
        std::vector<Attachment> color_attachments;
        Attachment depth_attachment;

        int samples {1};
        bool resizable {true};
        unsigned int resize_divisor {1};
        bool white_border_for_depth_texture {false};

        // Color attachment clearing stuff
        int clear_drawbuffer {0};  // TODO put in a struct
        float color_clear_value[4] {};
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

        FramebufferSpecification specification;

        unsigned int framebuffer {0};

        // These can be texture or renderbuffer handles
        std::vector<unsigned int> color_attachments;
        unsigned int depth_attachment {0};
    };
}
