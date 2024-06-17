#include "engine/graphics/font.hpp"

#include <algorithm>
#include <stdexcept>
#include <cassert>

#include <stb_truetype.h>
#include <stb_image_write.h>
#include <glm/glm.hpp>
#include <utf8.h>

#include "engine/application_base/platform.hpp"
#include "engine/application_base/panic.hpp"
#include "engine/graphics/opengl/buffer.hpp"
#include "engine/graphics/opengl/vertex_buffer_layout.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/utilities.hpp"

namespace sm {
    static constexpr char32_t ERROR_CHARACTER {127};

    static void blit_glyph(
        unsigned char* dest,
        int dest_width,
        int dest_height,
        unsigned char* glyph,
        int width,
        int height,
        int dest_x,
        int dest_y,
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

    Font::Font(
        const std::string& file_path,
        float size,
        int padding,
        unsigned char on_edge_value,
        int pixel_dist_scale,
        int bitmap_size
    )
        : bitmap_size(bitmap_size), padding(padding), on_edge_value(on_edge_value), pixel_dist_scale(pixel_dist_scale) {
        const auto contents {Utils::read_file(file_path)};

        if (!contents) {
            LOG_DIST_CRITICAL("Could not open file `{}` for reading", file_path);
            throw ResourceLoadingError;
        }

        font_info = new stbtt_fontinfo;

        if (!stbtt_InitFont(font_info, reinterpret_cast<const unsigned char*>(contents->c_str()), 0)) {
            LOG_DIST_CRITICAL("Could not load font `{}`", file_path);
            throw ResourceLoadingError;
        }

        sf = stbtt_ScaleForPixelHeight(font_info, size);

        initialize();

        name = Utils::get_file_name(file_path);

        LOG_DEBUG("Loaded font `{}`", file_path);
    }

    Font::~Font() {
        delete font_info;

        LOG_DEBUG("Unloaded font `{}`", name);
    }

    void Font::update_data(const float* data, std::size_t size) {
        auto vertex_buffer {buffer.lock()};  // Should be valid
        vertex_buffer->bind();
        vertex_buffer->upload_data(data, size);
        GlVertexBuffer::unbind();

        static constexpr std::size_t FLOATS_PER_VERTEX {4};

        assert(size % (sizeof(float) * FLOATS_PER_VERTEX) == 0);

        vertex_count = static_cast<int>(size / (sizeof(float) * FLOATS_PER_VERTEX));
    }

    void Font::begin_baking() {
        LOG_DEBUG("Begin baking font `{}`", name);

        // Delete the previous bitmap before creating another one
        bitmap_image.reset();

        const std::size_t SIZE {sizeof(unsigned char) * bitmap_size * bitmap_size};

        bake_context = {};
        bake_context.bitmap = new unsigned char[SIZE];
        std::memset(bake_context.bitmap, 0, SIZE);

        glyphs.clear();

        // This character should always be present
        bake_character(ERROR_CHARACTER);
    }

    void Font::end_baking() {
        TextureSpecification specification;
        specification.format = Format::R8;
        specification.border_color = std::make_optional<glm::vec4>(0.0f, 0.0f, 0.0f, 1.0f);

        bitmap_image = std::make_shared<GlTexture>(bitmap_size, bitmap_size, bake_context.bitmap, specification);

#ifdef SM_BUILD_DEBUG
        write_bitmap_to_file();
#endif

        delete[] bake_context.bitmap;

        LOG_DEBUG("End baking font `{}`", name);
    }

    void Font::bake_characters(int begin_codepoint, int end_codepoint) {
        int descent;
        stbtt_GetFontVMetrics(font_info, nullptr, &descent, nullptr);

        for (int codepoint {begin_codepoint}; codepoint <= end_codepoint; codepoint++) {
            try_bake_character(codepoint, descent);
        }
    }

    void Font::bake_characters(const char* string) {
        int descent;
        stbtt_GetFontVMetrics(font_info, nullptr, &descent, nullptr);

        const std::u32string utf32_string {utf8::utf8to32(std::string(string))};

        for (const char32_t character : utf32_string) {
            try_bake_character(character, descent);
        }
    }

    void Font::bake_character(int codepoint) {
        int descent;
        stbtt_GetFontVMetrics(font_info, nullptr, &descent, nullptr);

        try_bake_character(codepoint, descent);
    }

    void Font::bake_ascii() {
        bake_characters(32, 126);
    }

    void Font::render(const std::string& string, std::vector<float>& buffer) const {
        const std::u32string utf32_string {utf8::utf8to32(string)};

        int x {0};

        for (const char32_t character : utf32_string) {
            const Glyph& glyph {get_character_glyph(character)};

            const float x0 {static_cast<float>(x + glyph.xoff)};
            const float y0 {-static_cast<float>(glyph.height - glyph.yoff)};
            const float x1 {static_cast<float>(x + glyph.xoff + glyph.width)};
            const float y1 {static_cast<float>(glyph.yoff)};

            buffer.push_back(x0);
            buffer.push_back(y1);
            buffer.push_back(glyph.s0);
            buffer.push_back(glyph.t0);

            buffer.push_back(x0);
            buffer.push_back(y0);
            buffer.push_back(glyph.s0);
            buffer.push_back(glyph.t1);

            buffer.push_back(x1);
            buffer.push_back(y1);
            buffer.push_back(glyph.s1);
            buffer.push_back(glyph.t0);

            buffer.push_back(x1);
            buffer.push_back(y1);
            buffer.push_back(glyph.s1);
            buffer.push_back(glyph.t0);

            buffer.push_back(x0);
            buffer.push_back(y0);
            buffer.push_back(glyph.s0);
            buffer.push_back(glyph.t1);

            buffer.push_back(x1);
            buffer.push_back(y0);
            buffer.push_back(glyph.s1);
            buffer.push_back(glyph.t1);

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

        const int width {static_cast<int>(std::roundf((static_cast<float>(x + padding * 2) * scale)))};  // Take padding into consideration

        return std::make_pair(width, height);
    }

    void Font::initialize() {
        auto vertex_buffer {std::make_shared<GlVertexBuffer>(DrawHint::Stream)};

        VertexBufferLayout layout;
        layout.add(0, VertexBufferLayout::Float, 2);
        layout.add(1, VertexBufferLayout::Float, 2);

        vertex_array = std::make_shared<GlVertexArray>();
        vertex_array->bind();
        vertex_array->add_vertex_buffer(vertex_buffer, layout);
        GlVertexArray::unbind();

        buffer = vertex_buffer;
    }

    void Font::try_bake_character(int codepoint, int descent) {
        if (glyphs.count(codepoint) > 0) {
            LOG_WARNING("Character with codepoint `{}` is already baked");
            return;
        }

        int advance_width, left_side_bearing;
        stbtt_GetCodepointHMetrics(font_info, codepoint, &advance_width, &left_side_bearing);

        int y0;
        stbtt_GetCodepointBitmapBox(font_info, codepoint, sf, sf, nullptr, &y0, nullptr, nullptr);

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

        float s0, t0, s1, t1;
        blit_glyph(
            bake_context.bitmap,
            bitmap_size, bitmap_size,
            glyph,
            width, height,
            bake_context.x, bake_context.y,
            &s0, &t0, &s1, &t1
        );

        stbtt_FreeSDF(glyph, nullptr);

        bake_context.x += width;
        bake_context.max_row_height = std::max(bake_context.max_row_height, height);

        Glyph gl;
        gl.s0 = s0;
        gl.t0 = t0;
        gl.s1 = s1;
        gl.t1 = t1;
        gl.width = width;
        gl.height = height;
        gl.xoff = static_cast<int>(std::roundf(static_cast<float>(left_side_bearing) * sf));
        gl.yoff = static_cast<int>(std::roundf(static_cast<float>(-descent) * sf - static_cast<float>(y0)));
        gl.xadvance = static_cast<int>(std::roundf(static_cast<float>(advance_width) * sf));

        glyphs[static_cast<char32_t>(codepoint)] = gl;
    }

    const Font::Glyph& Font::get_character_glyph(char32_t character) const {
        try {
            return glyphs.at(character);
        } catch (const std::out_of_range&) {
            return glyphs.at(ERROR_CHARACTER);
        }
    }

    void Font::write_bitmap_to_file() {
        const std::string file_name {"bitmap_" + name + ".png"};

        if (!stbi_write_png(file_name.c_str(), bitmap_size, bitmap_size, 1, bake_context.bitmap, 0)) {
            LOG_ERROR("Failed to create bitmap png file `{}`", file_name);
        }
    }
}
