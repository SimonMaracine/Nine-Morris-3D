#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <string>
#include <cstddef>
#include <unordered_map>

#include "engine/graphics/opengl/vertex_array.hpp"
#include "engine/graphics/opengl/texture.hpp"

struct stbtt_fontinfo;

namespace sm {
    class GlVertexBuffer;

    class Font {
    public:
        Font(
            const std::string& buffer,
            float size,
            int padding,
            unsigned char on_edge_value,
            int pixel_dist_scale,
            int bitmap_size
        );

        ~Font() = default;

        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;
        Font(Font&&) = default;
        Font& operator=(Font&&) = default;

        void update_data(const float* data, std::size_t size);

        unsigned int get_bitmap_size() const { return static_cast<unsigned int>(bitmap_size); }

        const GlVertexArray* get_vertex_array() const { return vertex_array.get(); }
        const GlTexture* get_bitmap() const { return bitmap_image.get(); }
        int get_vertex_count() const { return vertex_count; }

        // Baking API
        void begin_baking();
        void end_baking(const char* name = "");
        void bake_characters(int begin_codepoint, int end_codepoint);
        void bake_characters(const char* string);
        void bake_character(int codepoint);
        void bake_ascii();

        // Get the buffer of data used in the end by OpenGL
        void render(const std::string& string, std::vector<float>& buffer) const;

        // Get the width and height of a piece of text
        std::pair<int, int> get_string_size(const std::string& string, float scale) const;
    private:
        struct Glyph {
            float s0, t0, s1, t1;
            int width, height;
            int xoff, yoff, xadvance;
        };

        void initialize();
        void try_bake_character(int codepoint, int descent);
        const Glyph& get_character_glyph(char32_t character) const;
        void write_bitmap_to_file(const char* name) const;

        struct {
            int x {};
            int y {};
            int max_row_height {};
            std::unique_ptr<unsigned char[]> bitmap;
        } bake_context;

        std::unordered_map<char32_t, Glyph> glyphs;

        std::unique_ptr<stbtt_fontinfo> font_info;
        int bitmap_size {};
        int padding {};  // Between glyphs
        unsigned char on_edge_value {};
        int pixel_dist_scale {};
        float sf {};  // Scale factor

        int vertex_count {};

        // Fonts own vertex arrays and bitmap textures
        std::unique_ptr<GlVertexArray> vertex_array;
        std::unique_ptr<GlTexture> bitmap_image;

        std::weak_ptr<GlVertexBuffer> wvertex_buffer;
    };
}
