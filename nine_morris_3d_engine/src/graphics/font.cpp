#include "engine/graphics/font.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstddef>
#include <cassert>

#include <stb_truetype.h>
#include <stb_image_write.h>
#include <utf8.h>

#include "engine/application_base/platform.hpp"
#include "engine/application_base/error.hpp"
#include "engine/application_base/logging.hpp"

namespace sm {
    Font::Font(const std::string& buffer, const FontSpecification& specification)
        : font_buffer(buffer), size_height(specification.size_height), bitmap_size(specification.bitmap_size) {
        assert(bitmap_size % 4 == 0);  // Needs 4 byte alignment

        font_info = new stbtt_fontinfo;

        if (!stbtt_InitFont(font_info, reinterpret_cast<unsigned char*>(font_buffer.data()), 0)) {
            SM_CRITICAL_ERROR(RuntimeError::ResourceLoading, "Could not load font");
        }

        sf = stbtt_ScaleForPixelHeight(font_info, specification.size_height);

        int x0 {};
        int y0 {};
        int x1 {};
        int y1 {};
        stbtt_GetFontBoundingBox(font_info, &x0, &y0, &x1, &y1);

        baseline = static_cast<float>(y0) * sf;

        const auto [ascent, descent, line_gap] {get_vertical_metrics()};  // TODO

        LOG_DEBUG("Loaded font");
    }

    Font::~Font() {
        for (const PackRange& pack_range : pack_ranges) {
            delete[] static_cast<stbtt_packedchar*>(pack_range.packed_characters);
        }

        delete font_info;

        LOG_DEBUG("Freed font");
    }

    void Font::begin_baking() {
        LOG_DEBUG("Begin baking font");

        bitmap = std::make_unique<unsigned char[]>(bitmap_size * bitmap_size);
        pack_context = new stbtt_pack_context;

        if (!stbtt_PackBegin(pack_context, bitmap.get(), bitmap_size, bitmap_size, 0, 1, nullptr)) {
            SM_CRITICAL_ERROR(RuntimeError::ResourceLoading, "Could not begin packing");
        }

        bitmap_texture.reset();
    }

    void Font::end_baking([[maybe_unused]] const char* name) {
        stbtt_PackEnd(pack_context);
        delete pack_context;

        TextureSpecification specification;
        specification.format = Format::R8;
        specification.border_color = std::make_optional<glm::vec4>(0.0f, 0.0f, 0.0f, 1.0f);

        bitmap_texture = std::make_unique<GlTexture>(bitmap_size, bitmap_size, bitmap.get(), specification);

#ifndef SM_BUILD_DISTRIBUTION
        write_bitmap_to_file(name, bitmap.get(), bitmap_size);
#endif

        bitmap.reset();

        LOG_DEBUG("End baking font");
    }

    void Font::bake_characters(int begin_codepoint, int count) {
        PackRange& pack_range {pack_ranges.emplace_back()};

        pack_range.packed_characters = new stbtt_packedchar[count];

        if (!stbtt_PackFontRange(pack_context, reinterpret_cast<unsigned char*>(font_buffer.data()), 0, size_height, begin_codepoint, count, static_cast<stbtt_packedchar*>(pack_range.packed_characters))) {
            SM_CRITICAL_ERROR(RuntimeError::ResourceLoading, "Could not pack range [{}, {}]", begin_codepoint, begin_codepoint + count);
        }

        pack_range.begin_codepoint = begin_codepoint;
        pack_range.count = count;
    }

    void Font::bake_ascii() {
        bake_characters(32, 95);
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

        float x {0.0f};  // TODO C++20
        float y {baseline};

        for (const char32_t character : utf32_string) {
            const int codepoint {static_cast<int>(character)};

            bool found_character {false};
            stbtt_aligned_quad quad {};

            for (const PackRange& pack_range : pack_ranges) {
                if (codepoint >= pack_range.begin_codepoint && codepoint <= pack_range.begin_codepoint + pack_range.count) {
                    stbtt_GetPackedQuad(static_cast<stbtt_packedchar*>(pack_range.packed_characters), bitmap_size, bitmap_size, codepoint - pack_range.begin_codepoint, &x, &y, &quad, 0);
                    found_character = true;
                    break;
                }
            }

            assert(found_character);

            static constexpr std::size_t ITEM_SIZE {4};

            const float y0 {-quad.y0};
            const float y1 {-quad.y1};

            SM_PUSH_BACK_ITEM(buffer, quad.x0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, y0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, quad.s0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, quad.t0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, index, ITEM_SIZE);

            SM_PUSH_BACK_ITEM(buffer, quad.x0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, y1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, quad.s0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, quad.t1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, index, ITEM_SIZE);

            SM_PUSH_BACK_ITEM(buffer, quad.x1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, y1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, quad.s1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, quad.t1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, index, ITEM_SIZE);

            SM_PUSH_BACK_ITEM(buffer, quad.x1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, y1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, quad.s1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, quad.t1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, index, ITEM_SIZE);

            SM_PUSH_BACK_ITEM(buffer, quad.x1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, y0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, quad.s1, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, quad.t0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, index, ITEM_SIZE);

            SM_PUSH_BACK_ITEM(buffer, quad.x0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, y0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, quad.s0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, quad.t0, ITEM_SIZE);
            SM_PUSH_BACK_ITEM(buffer, index, ITEM_SIZE);
        }
    }

    std::pair<int, int> Font::get_string_size(const std::string& string, float scale) const {
        // const std::u32string utf32_string {utf8::utf8to32(string)};

        // int x {0};
        // int height {0};

        // for (const char32_t character : utf32_string) {
        //     const Glyph& glyph {get_character_glyph(character)};

        //     x += glyph.xadvance;

        //     height = std::max(height, static_cast<int>(std::roundf(static_cast<float>(glyph.yoff) * scale)));
        // }

        // const int width {static_cast<int>(std::roundf((static_cast<float>(x) * scale)))};

        // return std::make_pair(width, height);

        return {};
    }

    std::tuple<int, int, int> Font::get_vertical_metrics() const {
        int ascent {};
        int descent {};
        int line_gap {};
        stbtt_GetFontVMetrics(font_info, &ascent, &descent, &line_gap);

        return std::make_tuple(ascent, descent, line_gap);
    }

    void Font::write_bitmap_to_file(const char* name, const unsigned char* bitmap, int size) {
        const std::string file_name {"bitmap_" + std::string(name) + ".png"};

        if (!stbi_write_png(file_name.c_str(), size, size, 1, bitmap, 0)) {
            LOG_ERROR("Failed to create bitmap png file `{}`", file_name);
        }
    }
}
