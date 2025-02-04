#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <string>

#include "nine_morris_3d_engine/graphics/opengl/texture.hpp"

struct stbtt_fontinfo;
struct stbtt_pack_context;

namespace sm {
    struct FontSpecification {
        float size_height {16.0f};
        int bitmap_size {128};
    };

    class Font {
    public:
        Font(const std::string& buffer, const FontSpecification& specification = {});
        ~Font();

        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;
        Font(Font&&) = delete;
        Font& operator=(Font&&) = delete;

        int get_bitmap_size() const;
        const GlTexture* get_bitmap() const;

        // Baking API
        void begin_baking();
        void end_baking(const char* name = nullptr);
        void bake_characters(int begin_codepoint, int end_codepoint);
        void bake_ascii();

        struct CharacterBuffer {
            float f0, f1, f2, f3;
            int i0;
            float f4, f5, f6, f7;
            int i1;
            float f8, f9, f10, f11;
            int i2;
            float f12, f13, f14, f15;
            int i3;
            float f16, f17, f18, f19;
            int i4;
            float f20, f21, f22, f23;
            int i5;
        };

        // Get the buffer of data used in the end by OpenGL
        void render(const std::string& string, int index, std::vector<CharacterBuffer>& buffer) const;

        // Get the width and height of a piece of text
        std::pair<int, int> get_string_size(const std::string& string, float scale = 1.0f) const;
    private:
        // The library author forgot to actually name the struct, so I made another one :P
        struct Quad {
            float x0, y0, s0, t0;
            float x1, y1, s1, t1;
        };

        void get_character_quad(int codepoint, float* x, float* y, Quad* quad) const;
        static void write_bitmap_to_file(const char* name, const unsigned char* bitmap, int size);

        struct PackRange {
            // The library author forgot to actually name the struct :P
            void* packed_characters {};
            int begin_codepoint {};
            int count {};
        };

        std::unique_ptr<unsigned char[]> m_bitmap;
        std::vector<PackRange> m_pack_ranges;
        stbtt_pack_context* m_pack_context {};

        std::unique_ptr<GlTexture> m_bitmap_texture;

        std::string m_font_buffer;
        stbtt_fontinfo* m_font_info {};

        float m_size_height {};
        int m_bitmap_size {};

        float m_sf {};  // Scale factor
        float m_baseline {};
        float m_vertical_advance {};
    };
}
