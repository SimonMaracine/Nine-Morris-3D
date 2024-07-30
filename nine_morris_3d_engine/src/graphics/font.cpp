#include "nine_morris_3d_engine/graphics/font.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstddef>
#include <cassert>

#include <stb_truetype.h>
#include <stb_image_write.h>
#include <utf8.h>

#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm {
    Font::Font(const std::string& buffer, const FontSpecification& specification)
        : m_font_buffer(buffer), m_size_height(specification.size_height), m_bitmap_size(specification.bitmap_size) {
        assert(bitmap_size % 4 == 0);  // Needs 4 byte alignment

        m_font_info = new stbtt_fontinfo;

        if (!stbtt_InitFont(m_font_info, reinterpret_cast<unsigned char*>(m_font_buffer.data()), 0)) {
            SM_THROW_ERROR(ResourceError, "Could not load font");
        }

        m_sf = stbtt_ScaleForPixelHeight(m_font_info, specification.size_height);

        int y0 {};
        int x0, x1, y1;
        stbtt_GetFontBoundingBox(m_font_info, &x0, &y0, &x1, &y1);

        m_baseline = static_cast<float>(-y0) * m_sf;

        int ascent {};
        int descent {};
        int line_gap {};
        stbtt_GetFontVMetrics(m_font_info, &ascent, &descent, &line_gap);

        m_vertical_advance = static_cast<float>(ascent - descent + line_gap) * m_sf;

        LOG_DEBUG("Loaded font");
    }

    Font::~Font() {
        for (const PackRange& pack_range : m_pack_ranges) {
            delete[] static_cast<stbtt_packedchar*>(pack_range.packed_characters);
        }

        delete m_font_info;

        LOG_DEBUG("Freed font");
    }

    int Font::get_bitmap_size() const noexcept {
        return m_bitmap_size;
    }

    const GlTexture* Font::get_bitmap() const noexcept {
        return m_bitmap_texture.get();
    }

    void Font::begin_baking() {
        LOG_DEBUG("Begin baking font");

        m_bitmap = std::make_unique<unsigned char[]>(m_bitmap_size * m_bitmap_size);
        m_pack_context = new stbtt_pack_context;

        if (!stbtt_PackBegin(m_pack_context, m_bitmap.get(), m_bitmap_size, m_bitmap_size, 0, 1, nullptr)) {
            SM_THROW_ERROR(ResourceError, "Could not begin packing");
        }

        m_bitmap_texture.reset();
    }

    void Font::end_baking([[maybe_unused]] const char* name) {
        stbtt_PackEnd(m_pack_context);
        delete m_pack_context;

        TextureSpecification specification;
        specification.format = TextureFormat::R8;
        specification.border_color = std::make_optional<glm::vec4>(0.0f, 0.0f, 0.0f, 1.0f);

        m_bitmap_texture = std::make_unique<GlTexture>(m_bitmap_size, m_bitmap_size, m_bitmap.get(), specification);

#ifndef SM_BUILD_DISTRIBUTION
        write_bitmap_to_file(name, m_bitmap.get(), m_bitmap_size);
#endif

        m_bitmap.reset();

        LOG_DEBUG("End baking font");
    }

    void Font::bake_characters(int begin_codepoint, int count) {
        PackRange& pack_range {m_pack_ranges.emplace_back()};

        pack_range.packed_characters = new stbtt_packedchar[count];

        const auto* data {reinterpret_cast<unsigned char*>(m_font_buffer.data())};
        auto* characters {static_cast<stbtt_packedchar*>(pack_range.packed_characters)};

        if (!stbtt_PackFontRange(m_pack_context, data, 0, m_size_height, begin_codepoint, count, characters)) {
            SM_THROW_ERROR(ResourceError, "Could not pack range [{}, {}]", begin_codepoint, begin_codepoint + count);
        }

        pack_range.begin_codepoint = begin_codepoint;
        pack_range.count = count;
    }

    void Font::bake_ascii() {
        bake_characters(32, 95);
    }

    void Font::render(const std::string& string, int index, std::vector<CharacterBuffer>& buffer) const {
        const std::u32string utf32_string {utf8::utf8to32(string)};

        float x {0.0f};  // TODO C++20
        float y {-m_baseline};

        // This makes multi-line text have its origin at bottom-left
        float vertical_positioning {0.0f};

        std::size_t character_count {};

        for (const char32_t character : utf32_string) {
            if (character == '\n') {
                y -= (-m_vertical_advance);
                vertical_positioning += m_vertical_advance;
                x = 0.0f;
                continue;
            }

            Quad quad {};
            get_character_quad(static_cast<int>(character), &x, &y, &quad);

            CharacterBuffer character_buffer;
            character_buffer.f0 = quad.x0;
            character_buffer.f1 = -quad.y0;
            character_buffer.f2 = quad.s0;
            character_buffer.f3 = quad.t0;
            character_buffer.i0 = index;
            character_buffer.f4 = quad.x0;
            character_buffer.f5 = -quad.y1;
            character_buffer.f6 = quad.s0;
            character_buffer.f7 = quad.t1;
            character_buffer.i1 = index;
            character_buffer.f8 = quad.x1;
            character_buffer.f9 = -quad.y1;
            character_buffer.f10 = quad.s1;
            character_buffer.f11 = quad.t1;
            character_buffer.i2 = index;
            character_buffer.f12 = quad.x1;
            character_buffer.f13 = -quad.y1;
            character_buffer.f14 = quad.s1;
            character_buffer.f15 = quad.t1;
            character_buffer.i3 = index;
            character_buffer.f16 = quad.x1;
            character_buffer.f17 = -quad.y0;
            character_buffer.f18 = quad.s1;
            character_buffer.f19 = quad.t0;
            character_buffer.i4 = index;
            character_buffer.f20 = quad.x0;
            character_buffer.f21 = -quad.y0;
            character_buffer.f22 = quad.s0;
            character_buffer.f23 = quad.t0;
            character_buffer.i5 = index;

            buffer.push_back(character_buffer);

            character_count++;
        }

        for (std::size_t i {buffer.size() - character_count}; i < buffer.size(); i++) {
            CharacterBuffer& character {buffer[i]};

            character.f1 += vertical_positioning;
            character.f5 += vertical_positioning;
            character.f9 += vertical_positioning;
            character.f13 += vertical_positioning;
            character.f17 += vertical_positioning;
            character.f21 += vertical_positioning;
        }
    }

    std::pair<int, int> Font::get_string_size(const std::string& string, float scale) const {
        const std::u32string utf32_string {utf8::utf8to32(string)};

        float width {-1.0f};
        float height {m_size_height};

        float line_width {0.0f};

        for (const char32_t character : utf32_string) {
            if (character == '\n') {
                height += m_vertical_advance;

                width = std::max(width, line_width);
                line_width = 0.0f;

                continue;
            }

            int advance_width {};
            stbtt_GetCodepointHMetrics(m_font_info, static_cast<int>(character), &advance_width, nullptr);

            line_width += static_cast<float>(advance_width) * m_sf;
        }

        width = std::max(width, line_width);

        return std::make_pair(width * std::min(scale, 1.0f), height * std::min(scale, 1.0f));
    }

    void Font::get_character_quad(int codepoint, float* x, float* y, Quad* quad) const noexcept {
        stbtt_aligned_quad aligned_quad {};
        [[maybe_unused]] bool found_character {false};

        for (const PackRange& pack_range : m_pack_ranges) {
            if (codepoint >= pack_range.begin_codepoint && codepoint <= pack_range.begin_codepoint + pack_range.count) {
                stbtt_GetPackedQuad(
                    static_cast<stbtt_packedchar*>(pack_range.packed_characters),
                    m_bitmap_size,
                    m_bitmap_size,
                    codepoint - pack_range.begin_codepoint,
                    x,
                    y,
                    &aligned_quad,
                    0
                );
                found_character = true;
                break;
            }
        }

        assert(found_character);

        quad->x0 = aligned_quad.x0;
        quad->y0 = aligned_quad.y0;
        quad->x1 = aligned_quad.x1;
        quad->y1 = aligned_quad.y1;
        quad->s0 = aligned_quad.s0;
        quad->t0 = aligned_quad.t0;
        quad->s1 = aligned_quad.s1;
        quad->t1 = aligned_quad.t1;
    }

    void Font::write_bitmap_to_file(const char* name, const unsigned char* bitmap, int size) {
        const std::string file_name {"bitmap_" + std::string(name) + ".png"};

        if (!stbi_write_png(file_name.c_str(), size, size, 1, bitmap, 0)) {
            LOG_ERROR("Failed to create bitmap png file `{}`", file_name);
        }
    }
}
