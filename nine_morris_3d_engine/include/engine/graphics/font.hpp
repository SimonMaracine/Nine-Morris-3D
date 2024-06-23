#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <string>

#include "engine/graphics/opengl/texture.hpp"

struct stbtt_fontinfo;
struct stbtt_pack_context;

namespace sm {
    struct FontSpecification {
        float size_height {16.0f};
        int bitmap_size {256};
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
        const GlTexture* get_bitmap() const { return bitmap_texture.get(); }

        // Baking API
        void begin_baking();
        void end_baking(const char* name = "");
        void bake_characters(int begin_codepoint, int end_codepoint);
        void bake_ascii();

        // Get the buffer of data used in the end by OpenGL
        void render(const std::string& string, int index, std::vector<unsigned char>& buffer) const;

        // Get the width and height of a piece of text
        std::pair<int, int> get_string_size(const std::string& string, float scale) const;
    private:
        std::tuple<int, int, int> get_vertical_metrics() const;
        static void write_bitmap_to_file(const char* name, const unsigned char* bitmap, int size);

        struct PackRange {
            // The library author forgot to actually name the struct :P
            void* packed_characters {nullptr};
            int begin_codepoint {};
            int count {};
        };

        std::unique_ptr<unsigned char[]> bitmap;
        std::vector<PackRange> pack_ranges;
        stbtt_pack_context* pack_context {nullptr};

        std::unique_ptr<GlTexture> bitmap_texture;

        std::string font_buffer;
        stbtt_fontinfo* font_info {nullptr};

        float size_height {};
        int bitmap_size {};

        float sf {};  // Scale factor
        float baseline {};
    };
}
