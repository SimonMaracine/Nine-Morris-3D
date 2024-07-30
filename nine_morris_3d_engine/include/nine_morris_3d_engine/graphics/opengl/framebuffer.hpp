#pragma once

#include <vector>

namespace sm {
    enum class AttachmentFormat {
        None,
        Rgba8,
        Rgba8Float,
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
        int width {};
        int height {};

        int samples {1};
        int resize_divisor {1};
        bool resizable {true};
        bool white_border_depth_texture {false};
        bool comparison_mode_depth_texture {false};
    };

    class GlFramebuffer {
    public:
        explicit GlFramebuffer(const FramebufferSpecification& specification);
        ~GlFramebuffer();

        GlFramebuffer(const GlFramebuffer&) = delete;
        GlFramebuffer& operator=(const GlFramebuffer&) = delete;
        GlFramebuffer(GlFramebuffer&&) = delete;
        GlFramebuffer& operator=(GlFramebuffer&&) = delete;

        void bind() const noexcept;
        static void bind_default() noexcept;

        unsigned int get_color_attachment(int attachment_index) const noexcept;
        unsigned int get_depth_attachment() const noexcept;
        const FramebufferSpecification& get_specification() const noexcept;

        // Usually called by application
        void resize(int width, int height);

        // Read pixels from some buffer
        float read_pixel_float(int attachment_index, int x, int y) const noexcept;  // TODO read value is float; should be generic
        void read_pixel_float_pbo(int attachment_index, int x, int y) const noexcept;

        // Resolve this to draw_framebuffer
        void blit(const GlFramebuffer* draw_framebuffer, int width, int height) const noexcept;
    private:
        void build();

        unsigned int m_framebuffer {};
        FramebufferSpecification m_specification;

        // These can be texture or renderbuffer handles
        unsigned int m_depth_attachment {};
        std::vector<unsigned int> m_color_attachments;
    };
}
