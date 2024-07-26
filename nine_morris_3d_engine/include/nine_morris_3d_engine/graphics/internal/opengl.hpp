#pragma once

namespace sm::internal {
    namespace opengl {
        // Color, depth, stencil
        enum class Buffers {
            C,
            D,
            S,
            CD,
            CS,
            DS,
            CDS
        };

        enum class Function {
            Always,
            NotEqual
        };

        void initialize_default() noexcept;

        void clear(Buffers buffers) noexcept;
        void clear_color(float red, float green, float blue) noexcept;

        void viewport(int width, int height) noexcept;

        void bind_texture_2d(unsigned int texture, int unit) noexcept;

        void draw_arrays(int count) noexcept;
        void draw_arrays_lines(int count) noexcept;
        void draw_elements(int count) noexcept;
        void draw_elements_instanced(int count, int instance_count) noexcept;

        void disable_depth_test() noexcept;
        void enable_depth_test() noexcept;

        void disable_blending() noexcept;
        void enable_blending() noexcept;

        void disable_back_face_culling() noexcept;
        void enable_back_face_culling() noexcept;

        void enable_framebuffer_srgb() noexcept;
        void disable_framebuffer_srgb() noexcept;

        void enable_polygon_offset_fill() noexcept;
        void disable_polygon_offset_fill() noexcept;

        void initialize_stencil() noexcept;
        void stencil_function(Function function, int ref, unsigned int mask) noexcept;
        void stencil_mask(unsigned int mask) noexcept;

        void initialize_polygon_offset() noexcept;
    }
}
