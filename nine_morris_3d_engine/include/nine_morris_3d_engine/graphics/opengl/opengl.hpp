#pragma once

namespace sm {
    namespace opengl {
        // Color, Depth, Stencil
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

        void initialize_default();

        void clear(Buffers buffers);
        void clear_color(float red, float green, float blue);

        void viewport(int width, int height);

        void  bind_texture_2d(unsigned int texture, int unit);

        void draw_arrays(int count);
        void draw_arrays_lines(int count);
        void draw_elements(int count);
        void draw_elements_instanced(int count, int instance_count);

        void disable_depth_test();
        void enable_depth_test();

        void disable_blending();
        void enable_blending();

        void disable_back_face_culling();
        void enable_back_face_culling();

        void initialize_stencil();
        void stencil_function(Function function, int ref, unsigned int mask);
        void stencil_mask(unsigned int mask);
    }
}
