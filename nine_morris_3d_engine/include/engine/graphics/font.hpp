#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <string>
#include <cstddef>
#include <unordered_map>

struct stbtt_fontinfo;

namespace sm {
    class GlVertexArray;
    class GlVertexBuffer;
    class GlTexture;

    class Font {
    public:
        Font(
            const std::string& file_path,
            float size,
            int padding,
            unsigned char on_edge_value,
            int pixel_dist_scale,
            int bitmap_size
        );

        ~Font();

        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;
        Font(Font&&) = delete;
        Font& operator=(Font&&) = delete;

        void update_data(const float* data, std::size_t size);

        unsigned int get_bitmap_size() const { return static_cast<unsigned int>(bitmap_size); }

        const GlVertexArray* get_vertex_array() const { return vertex_array.get(); }
        const GlTexture* get_bitmap() const { return bitmap_image.get(); }
        int get_vertex_count() const { return vertex_count; }

        // Baking API
        void begin_baking();
        void end_baking();
        void bake_characters(int begin_codepoint, int end_codepoint);
        void bake_characters(const char* string);
        void bake_character(int codepoint);
        void bake_ascii();

        // Call render to get the buffer of data used in the end by OpenGL
        void render(const std::string& string, std::vector<float>& buffer) const;

        // Get width and height of a line of text
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
        void write_bitmap_to_file();

        struct BakeContext {
            int x {0};
            int y {0};
            int max_row_height {0};
            unsigned char* bitmap {nullptr};
        } bake_context;

        std::unordered_map<char32_t, Glyph> glyphs;

        stbtt_fontinfo* font_info {nullptr};
        const unsigned char* font_info_buffer {nullptr};
        int bitmap_size {0};
        int padding {0};  // Between glyphs
        unsigned char on_edge_value {0};
        int pixel_dist_scale {0};
        float sf {0.0f};  // Scale factor

        std::string name;

        // Fonts own vertex arrays and bitmap textures
        std::shared_ptr<GlVertexArray> vertex_array;
        std::shared_ptr<GlTexture> bitmap_image;

        std::weak_ptr<GlVertexBuffer> buffer;

        int vertex_count {0};
    };
}
