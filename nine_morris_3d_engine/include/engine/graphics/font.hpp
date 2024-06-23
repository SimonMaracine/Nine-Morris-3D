#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <string>
#include <unordered_map>

#include "engine/graphics/opengl/vertex_array.hpp"
#include "engine/graphics/opengl/texture.hpp"

struct stbtt_fontinfo;

namespace sm {
    class GlVertexBuffer;

    struct FontSpecification {
        float size_height {16.0f};
        int bitmap_size {256};
        int padding {4};  // Between glyphs
        unsigned char on_edge_value {180};
        float pixel_dist_scale {45.0f};
    };

    class Font {
    public:
        Font(const std::string& buffer, const FontSpecification& specification);
        ~Font();

        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;
        Font(Font&&) = delete;
        Font& operator=(Font&&) = delete;

        int get_bitmap_size() const { return bitmap_size; }
        const GlTexture* get_bitmap() const { return bitmap_image.get(); }

        // Baking API
        void begin_baking();
        void end_baking(const char* name = "");
        void bake_characters(int begin_codepoint, int end_codepoint);
        void bake_characters(const char* string);
        void bake_character(int codepoint);
        void bake_ascii();

        // Get the buffer of data used in the end by OpenGL
        void render(const std::string& string, int index, std::vector<unsigned char>& buffer) const;

        // Get the width and height of a piece of text
        std::pair<int, int> get_string_size(const std::string& string, float scale) const;
    private:
        struct Glyph {
            struct {
                float s0, t0, s1, t1;
            } bitmap;

            float width, height;
            float xoff, yoff, xadvance;
        };

        void try_bake_character(int codepoint);
        const Glyph& get_character_glyph(char32_t character) const;
        std::tuple<int, int, int> get_vertical_metrics() const;
        void write_bitmap_to_file(const char* name) const;

        struct {
            int x {};
            int y {};
            int max_row_height {};
            std::unique_ptr<unsigned char[]> bitmap;
        } bake_context;

        std::unique_ptr<GlTexture> bitmap_image;
        std::unordered_map<char32_t, Glyph> glyphs;

        std::string font_buffer;
        stbtt_fontinfo* font_info {nullptr};

        int bitmap_size {};
        // int padding {};
        // unsigned char on_edge_value {};
        // float pixel_dist_scale {};

        float sf {};  // Scale factor
        float baseline {};
    };
}
