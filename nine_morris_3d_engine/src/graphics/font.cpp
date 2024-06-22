#include "engine/graphics/font.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstddef>
#include <cassert>

#include <stb_truetype.h>
#include <stb_image_write.h>
#include <glm/glm.hpp>
#include <utf8.h>

#include "engine/application_base/platform.hpp"
#include "engine/application_base/error.hpp"
#include "engine/application_base/logging.hpp"
#include "engine/graphics/opengl/buffer.hpp"
#include "engine/graphics/opengl/vertex_buffer_layout.hpp"

namespace sm {
    static constexpr char32_t ERROR_CHARACTER {127};

    static void blit_glyph(
        unsigned char* dest,
        int dest_width,
        int dest_height,
        int dest_x,
        int dest_y,
        unsigned char* glyph,
        int width,
        int height,
        float* s0,
        float* t0,
        float* s1,
        float* t1
    ) {
        for (int x {0}; x < width; x++) {
            for (int y {0}; y < height; y++) {
                const int index {(y + dest_y) * dest_width + x + dest_x};

                assert(index < dest_width * dest_height);

                dest[static_cast<std::size_t>(index)] = glyph[y * width + x];
            }
        }

        *s0 = static_cast<float>(dest_x) / static_cast<float>(dest_width);
        *t0 = static_cast<float>(dest_y) / static_cast<float>(dest_height);
        *s1 = static_cast<float>(dest_x + width) / static_cast<float>(dest_width);
        *t1 = static_cast<float>(dest_y + height) / static_cast<float>(dest_height);
    }

    Font::Font(const std::string& buffer, const FontSpecification& specification)
        : bitmap_size(specification.bitmap_size), padding(specification.padding),
        on_edge_value(specification.on_edge_value), pixel_dist_scale(specification.pixel_dist_scale) {
        assert(bitmap_size % 4 == 0);  // Needs 4 byte alignment

        font_buffer = buffer;
        font_info = new stbtt_fontinfo;

        if (!stbtt_InitFont(font_info, reinterpret_cast<const unsigned char*>(font_buffer.data()), 0)) {
            SM_CRITICAL_ERROR(RuntimeError::ResourceLoading, "Could not load font");
        }

        sf = stbtt_ScaleForPixelHeight(font_info, specification.size_height);

        int x0 {};
        int y0 {};
        int x1 {};
        int y1 {};
        stbtt_GetFontBoundingBox(font_info, &x0, &y0, &x1, &y1);

        baseline = static_cast<float>(-y0) * sf;

        LOG_DEBUG("Loaded font");
    }

    Font::~Font() {
        delete font_info;

        LOG_DEBUG("Freed font");
    }

    void Font::begin_baking() {
        LOG_DEBUG("Begin baking font");

        // Delete the previous data first
        bitmap_image.reset();
        glyphs.clear();

        bake_context = {};
        bake_context.bitmap = std::make_unique<unsigned char[]>(bitmap_size * bitmap_size);

        // This character should always be present
        bake_character(ERROR_CHARACTER);
    }

    void Font::end_baking([[maybe_unused]] const char* name) {
        TextureSpecification specification;
        specification.format = Format::R8;
        specification.border_color = std::make_optional<glm::vec4>(0.0f, 0.0f, 0.0f, 1.0f);

        bitmap_image = std::make_unique<GlTexture>(bitmap_size, bitmap_size, bake_context.bitmap.get(), specification);

#ifndef SM_BUILD_DISTRIBUTION
        write_bitmap_to_file(name);
#endif

        bake_context.bitmap.reset();

        LOG_DEBUG("End baking font");
    }

    void Font::bake_characters(int begin_codepoint, int end_codepoint) {
        const auto [ascent, descent, line_gap] {get_vertical_metrics()};  // TODO

        for (int codepoint {begin_codepoint}; codepoint <= end_codepoint; codepoint++) {
            try_bake_character(codepoint);
        }
    }

    void Font::bake_characters(const char* string) {
        const auto [ascent, descent, line_gap] {get_vertical_metrics()};

        const std::u32string utf32_string {utf8::utf8to32(std::string(string))};

        for (const char32_t character : utf32_string) {
            try_bake_character(character);
        }
    }

    void Font::bake_character(int codepoint) {
        const auto [ascent, descent, line_gap] {get_vertical_metrics()};

        try_bake_character(codepoint);
    }

    void Font::bake_ascii() {
        bake_characters(32, 126);
    }

#define SM_PUSH_BACK_ITEM(buffer, item_data, item_size) \
    static_assert(sizeof(item_data) == item_size); \
    buffer.emplace_back(); \
    buffer.emplace_back(); \
    buffer.emplace_back(); \
    buffer.emplace_back(); \
    std::memcpy(buffer.data() + buffer.size() - item_size, &item_data, item_size)

    void Font::render(const std::string& string, int index, std::vector<unsigned char>& buffer) const {
        const std::u32string utf32_string {utf8::utf8to32(string)};

        int x {0};  // TODO C++20

        for (const char32_t character : utf32_string) {
            const Glyph& glyph {get_character_glyph(character)};

            const float x0 {static_cast<float>(x + glyph.xoff)};
            const float y0 {static_cast<float>(baseline + glyph.yoff)};
            const float x1 {static_cast<float>(x + glyph.xoff + glyph.width)};
            const float y1 {static_cast<float>(baseline + glyph.yoff + glyph.height)};

            static constexpr std::size_t ITEM_SIZE {4};

            SM_PUSH_BACK_ITEM(buffer, x0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, y1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, glyph.bitmap.s0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, glyph.bitmap.t0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, index, ITEM_SIZE);

            SM_PUSH_BACK_ITEM(buffer, x0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, y0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, glyph.bitmap.s0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, glyph.bitmap.t1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, index, ITEM_SIZE);

            SM_PUSH_BACK_ITEM(buffer, x1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, y1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, glyph.bitmap.s1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, glyph.bitmap.t0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, index, ITEM_SIZE);

            SM_PUSH_BACK_ITEM(buffer, x1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, y1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, glyph.bitmap.s1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, glyph.bitmap.t0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, index, ITEM_SIZE);

            SM_PUSH_BACK_ITEM(buffer, x0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, y0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, glyph.bitmap.s0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, glyph.bitmap.t1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, index, ITEM_SIZE);

            SM_PUSH_BACK_ITEM(buffer, x1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, y0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, glyph.bitmap.s1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, glyph.bitmap.t1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, index, ITEM_SIZE);

            x += glyph.xadvance;
        }
    }

    std::pair<int, int> Font::get_string_size(const std::string& string, float scale) const {
        const std::u32string utf32_string {utf8::utf8to32(string)};

        int x {0};
        int height {0};

        for (const char32_t character : utf32_string) {
            const Glyph& glyph {get_character_glyph(character)};

            x += glyph.xadvance;

            height = std::max(height, static_cast<int>(std::roundf(static_cast<float>(glyph.yoff) * scale)));
        }

        // Take padding into consideration
        const int width {static_cast<int>(std::roundf((static_cast<float>(x + padding * 2) * scale)))};

        return std::make_pair(width, height);
    }

    void Font::try_bake_character(int codepoint) {
        if (glyphs.count(codepoint) > 0) {
            LOG_WARNING("Character with codepoint `{}` is already baked");
            return;
        }

        // Assume 0, because glyph can be null
        int width {0};
        int height {0};

        unsigned char* glyph {stbtt_GetCodepointSDF(
            font_info,
            sf,
            codepoint,
            padding,
            on_edge_value,
            static_cast<float>(pixel_dist_scale),
            &width,
            &height,
            nullptr,
            nullptr
        )};

        if (glyph == nullptr) {
            LOG_WARNING("Could not bake character with codepoint `{}`; still adding to map...", codepoint);
        }

        if (bake_context.x + width > bitmap_size) {
            bake_context.y += bake_context.max_row_height;
            bake_context.x = 0;
            bake_context.max_row_height = 0;
        }

        float s0 {};
        float t0 {};
        float s1 {};
        float t1 {};
        blit_glyph(
            bake_context.bitmap.get(),
            bitmap_size, bitmap_size,
            bake_context.x, bake_context.y,
            glyph,
            width, height,
            &s0, &t0, &s1, &t1
        );

        stbtt_FreeSDF(glyph, nullptr);

        bake_context.x += width;
        bake_context.max_row_height = std::max(bake_context.max_row_height, height);

        int advance_width {};
        int left_side_bearing {};
        stbtt_GetCodepointHMetrics(font_info, codepoint, &advance_width, &left_side_bearing);

        int x0 {};
        int y0 {};
        int x1 {};
        int y1 {};
        stbtt_GetCodepointBox(font_info, codepoint, &x0, &y0, &x1, &y1);

        Glyph gl;
        gl.bitmap.s0 = s0;
        gl.bitmap.t0 = t0;
        gl.bitmap.s1 = s1;
        gl.bitmap.t1 = t1;
        // gl.bitmap.width = width;
        // gl.bitmap.height = height;
        gl.width = static_cast<float>(x1 - x0) * sf;
        gl.height = static_cast<float>(y1 - y0) * sf;
        gl.xoff = static_cast<int>(std::roundf(static_cast<float>(left_side_bearing) * sf));
        gl.yoff = static_cast<int>(std::roundf(static_cast<float>(y0) * sf));
        gl.xadvance = static_cast<int>(std::roundf(static_cast<float>(advance_width) * sf));

        glyphs[static_cast<char32_t>(codepoint)] = gl;
    }

    const Font::Glyph& Font::get_character_glyph(char32_t character) const {
        if (auto glyph {glyphs.find(character)}; glyph != glyphs.cend()) {
            return glyph->second;
        } else {
            return glyphs.at(ERROR_CHARACTER);
        }
    }

    std::tuple<int, int, int> Font::get_vertical_metrics() const {
        int ascent {};
        int descent {};
        int line_gap {};
        stbtt_GetFontVMetrics(font_info, &ascent, &descent, &line_gap);

        return std::make_tuple(ascent, descent, line_gap);
    }

    void Font::write_bitmap_to_file(const char* name) const {
        const std::string file_name {"bitmap_" + std::string(name) + ".png"};

        if (!stbi_write_png(file_name.c_str(), bitmap_size, bitmap_size, 1, bake_context.bitmap.get(), 0)) {
            LOG_ERROR("Failed to create bitmap png file `{}`", file_name);
        }
    }
}
