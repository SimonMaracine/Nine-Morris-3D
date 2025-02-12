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

        // Stencil function
        enum class Function {
            Always,
            NotEqual
        };

        // Setup default OpenGL state
        void initialize_default();

        // Clear the active framebuffer
        void clear(Buffers buffers);

        // Set the clear color
        void clear_color(float red, float green, float blue);

        // Set the viewport for the active framebuffer
        void viewport(int width, int height);

        // Bind a 2D texture
        void bind_texture_2d(unsigned int texture, int unit);

        // Draw call routines
        void draw_arrays(int count);
        void draw_arrays_lines(int count);
        void draw_elements(int count);
        void draw_elements_instanced(int count, int instance_count);
        void draw_elements_adjacency(int count);

        // Depth test state
        void disable_depth_test();
        void enable_depth_test();

        // Blending state
        void disable_blending();
        void enable_blending();

        // Back face culling state
        void disable_back_face_culling();
        void enable_back_face_culling();

        // sRGB state
        void enable_framebuffer_srgb();
        void disable_framebuffer_srgb();

        // Polygon offset state
        void initialize_polygon_offset();
        void enable_polygon_offset_fill();
        void disable_polygon_offset_fill();

        // Stencil test state
        void initialize_stencil();
        void stencil_function(Function function, int ref, unsigned int mask);
        void stencil_mask(unsigned int mask);
    }
}
