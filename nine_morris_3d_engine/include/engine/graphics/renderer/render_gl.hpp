#pragma once

namespace sm {
    struct RenderGl {
        /*
            Color
            Depth
            Stencil
        */
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

        static void initialize_default();

        static void clear(Buffers buffers);
        static void clear_color(float red, float green, float blue);

        static void viewport(int width, int height);

        static void bind_texture_2d(unsigned int texture, int unit);

        static void draw_arrays(int count);
        static void draw_arrays_lines(int count);
        static void draw_elements(int count);
        static void draw_elements_instanced(int count, int instance_count);

        static void disable_depth_test();
        static void enable_depth_test();

        static void disable_blending();
        static void enable_blending();

        static void disable_back_face_culling();
        static void enable_back_face_culling();

        static void initialize_stencil();
        static void stencil_function(Function function, int ref, unsigned int mask);
        static void stencil_mask(unsigned int mask);
    };
}
