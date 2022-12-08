#include <stb_truetype.h>
#include <stb_image_write.h>
#include <glad/glad.h>

#include <utf8.h>

#include "nine_morris_3d_engine/application/platform.h"
#include "nine_morris_3d_engine/graphics/font.h"
#include "nine_morris_3d_engine/graphics/opengl/buffer.h"
#include "nine_morris_3d_engine/graphics/opengl/vertex_array.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"
#include "nine_morris_3d_engine/other/exit.h"

constexpr char ERROR_CHARACTER = 127;

static std::string get_name(std::string_view file_path) {
    size_t last_slash = file_path.find_last_of("/");
    ASSERT(last_slash != std::string::npos, "Could not find slash");

    return std::string(file_path.substr(last_slash + 1));
}

static const char* get_file_data(std::string_view file_path) {
    std::ifstream file {std::string(file_path), std::ios::binary};

    if (!file.is_open()) {
        REL_CRITICAL("Could not open file `{}` for reading, exiting...", file_path);
        game_exit::exit_critical();
    }

    file.seekg(0, file.end);
    const size_t length = file.tellg();
    file.seekg(0, file.beg);

    char* buffer = new char[length];
    file.read(buffer, length);

    return buffer;
}

static void blit_glyph(unsigned char* dest, int dest_width, int dest_height, unsigned char* glyph,
        int width, int height, int dest_x, int dest_y, float* s0, float* t0, float* s1, float* t1) {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            const size_t index = static_cast<size_t>((y + dest_y) * dest_width + (x + dest_x));
            dest[index] = glyph[y * width + x];
        }
    }

    *s0 = static_cast<float>(dest_x) / static_cast<float>(dest_width);
    *t0 = static_cast<float>(dest_y) / static_cast<float>(dest_height);
    *s1 = static_cast<float>(dest_x + width) / static_cast<float>(dest_width);
    *t1 = static_cast<float>(dest_y + height) / static_cast<float>(dest_height);
}

Font::Font(std::string_view file_path, float size, int padding, unsigned char on_edge_value,
        int pixel_dist_scale, int bitmap_size)
    : bitmap_size(bitmap_size), padding(padding), on_edge_value(on_edge_value),
      pixel_dist_scale(pixel_dist_scale) {
    font_file_buffer = get_file_data(file_path);

    if (!stbtt_InitFont(&info, reinterpret_cast<const unsigned char*>(font_file_buffer), 0)) {
        REL_CRITICAL("Could not load font `{}`, exiting...", file_path);
        game_exit::exit_critical();
    }
    sf = stbtt_ScaleForPixelHeight(&info, size);

    buffer = std::make_shared<gl::Buffer>(1, gl::DrawHint::Stream);

    BufferLayout layout;
    layout.add(0, BufferLayout::Float, 2);
    layout.add(1, BufferLayout::Float, 2);

    vertex_array = std::make_shared<gl::VertexArray>();
    vertex_array->add_buffer(buffer, layout);
    gl::VertexArray::unbind();

    name = get_name(file_path);

    DEB_DEBUG("Loaded font `{}`", file_path);
}

Font::~Font() {
    glDeleteTextures(1, &texture);
    delete[] font_file_buffer;

    DEB_DEBUG("Unloaded font `{}`", name);
}

void Font::update_data(const float* data, size_t size) {
    buffer->bind();
    buffer->update_data(data, size);
    vertex_count = size / sizeof(float);
}

void Font::begin_baking() {
    DEB_DEBUG("Begin baking font `{}`", name);

    glDeleteTextures(1, &texture);

    const size_t SIZE = sizeof(unsigned char) * bitmap_size * bitmap_size;

    bake_context = BakeContext {};
    bake_context.bitmap = new unsigned char[SIZE];
    memset(bake_context.bitmap, 0, SIZE);
}

void Font::end_baking() {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, bitmap_size, bitmap_size);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, bitmap_size, bitmap_size, GL_RED, GL_UNSIGNED_BYTE, bake_context.bitmap);

    glBindTexture(GL_TEXTURE_2D, 0);

#ifdef PLATFORM_GAME_DEBUG
    const std::string file_name = "bitmap_" + name + ".png";
    if (!stbi_write_png(file_name.c_str(), bitmap_size, bitmap_size, 1, bake_context.bitmap, 0)) {
        DEB_ERROR("Failed to create bitmap png file `{}`", file_name);
    }
#endif

    delete[] bake_context.bitmap;

    DEB_DEBUG("End baking font `{}`", name);
}

void Font::bake_characters(int begin_codepoint, int end_codepoint) {
    int descent;
    stbtt_GetFontVMetrics(&info, nullptr, &descent, nullptr);

    for (int codepoint = begin_codepoint; codepoint <= end_codepoint; codepoint++) {
        int advance_width, left_side_bearing;
        stbtt_GetCodepointHMetrics(&info, codepoint, &advance_width, &left_side_bearing);

        int y0;
        stbtt_GetCodepointBitmapBox(&info, codepoint, sf, sf, nullptr, &y0, nullptr, nullptr);

        int width = 0, height = 0;  // Assume 0, because glyph can be null
        unsigned char* glyph = stbtt_GetCodepointSDF(
            &info, sf, codepoint, padding, on_edge_value,
            static_cast<float>(pixel_dist_scale), &width, &height, nullptr, nullptr
        );

        if (glyph == nullptr) {
            DEB_WARN("Couldn't bake character with codepoint `{}`; still adding to map...", codepoint);
        }

        if (bake_context.x + width > bitmap_size) {
            bake_context.y += bake_context.max_row_height;
            bake_context.x = 0;
            bake_context.max_row_height = 0;
        }

        float s0, t0, s1, t1;
        blit_glyph(
            bake_context.bitmap, bitmap_size, bitmap_size, glyph, width, height, bake_context.x,
            bake_context.y, &s0, &t0, &s1, &t1
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
        gl.xoff = static_cast<int>(std::roundf(left_side_bearing * sf));
        gl.yoff = static_cast<int>(std::roundf(-descent * sf - y0));
        gl.xadvance = static_cast<int>(std::roundf(advance_width * sf));

        ASSERT(glyphs.count(codepoint) == 0, "There should be only one of each glyph");

        glyphs[codepoint] = gl;
    }
}

void Font::bake_character(int codepoint) {
    int descent;
    stbtt_GetFontVMetrics(&info, nullptr, &descent, nullptr);

    int advance_width, left_side_bearing;
    stbtt_GetCodepointHMetrics(&info, codepoint, &advance_width, &left_side_bearing);

    int y0;
    stbtt_GetCodepointBitmapBox(&info, codepoint, sf, sf, nullptr, &y0, nullptr, nullptr);

    int width = 0, height = 0;  // Assume 0, because glyph can be null
    unsigned char* glyph = stbtt_GetCodepointSDF(
        &info, sf, codepoint, padding, on_edge_value,
        static_cast<float>(pixel_dist_scale), &width, &height, nullptr, nullptr
    );

    if (glyph == nullptr) {
        DEB_WARN("Couldn't bake character with codepoint `{}`; still adding to map...", codepoint);
    }

    if (bake_context.x + width > bitmap_size) {
        bake_context.y += bake_context.max_row_height;
        bake_context.x = 0;
        bake_context.max_row_height = 0;
    }

    float s0, t0, s1, t1;
    blit_glyph(
        bake_context.bitmap, bitmap_size, bitmap_size, glyph, width, height, bake_context.x,
        bake_context.y, &s0, &t0, &s1, &t1
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
    gl.xoff = static_cast<int>(std::roundf(left_side_bearing * sf));
    gl.yoff = static_cast<int>(std::roundf(-descent * sf - y0));
    gl.xadvance = static_cast<int>(std::roundf(advance_width * sf));

    ASSERT(glyphs.count(codepoint) == 0, "There should be only one of each glyph");

    glyphs[codepoint] = gl;
}

void Font::render(std::string_view string, size_t* out_size, float** out_buffer) {
    const std::u16string utf16_string = utf8::utf8to16(string);

    const size_t SIZE = sizeof(float) * utf16_string.length() * 24;

    float* buffer = new float[SIZE];
    unsigned int buffer_index = 0;

    int x = 0;

    for (const char16_t character : utf16_string) {
        const Font::Glyph* glyph;

        try {
            glyph = &glyphs.at(character);
        } catch (const std::out_of_range&) {
            glyph = &glyphs[ERROR_CHARACTER];
        }

        const float x0 = static_cast<float>(x + glyph->xoff);
        const float y0 = -static_cast<float>(glyph->height - glyph->yoff);
        const float x1 = static_cast<float>(x + glyph->xoff + glyph->width);
        const float y1 = static_cast<float>(glyph->yoff);

        buffer[buffer_index++] = x0;
        buffer[buffer_index++] = y1;
        buffer[buffer_index++] = glyph->s0;
        buffer[buffer_index++] = glyph->t0;

        buffer[buffer_index++] = x0;
        buffer[buffer_index++] = y0;
        buffer[buffer_index++] = glyph->s0;
        buffer[buffer_index++] = glyph->t1;

        buffer[buffer_index++] = x1;
        buffer[buffer_index++] = y1;
        buffer[buffer_index++] = glyph->s1;
        buffer[buffer_index++] = glyph->t0;

        buffer[buffer_index++] = x1;
        buffer[buffer_index++] = y1;
        buffer[buffer_index++] = glyph->s1;
        buffer[buffer_index++] = glyph->t0;

        buffer[buffer_index++] = x0;
        buffer[buffer_index++] = y0;
        buffer[buffer_index++] = glyph->s0;
        buffer[buffer_index++] = glyph->t1;

        buffer[buffer_index++] = x1;
        buffer[buffer_index++] = y0;
        buffer[buffer_index++] = glyph->s1;
        buffer[buffer_index++] = glyph->t1;

        x += glyph->xadvance;
    }

    *out_size = SIZE;
    *out_buffer = buffer;
}

void Font::get_string_size(std::string_view string, float scale, int* out_width, int* out_height) {
    const std::u16string utf16_string = utf8::utf8to16(string);

    int x = 0;
    *out_height = 0;

    for (const char16_t character : utf16_string) {
        const Font::Glyph* glyph;

        try {
            glyph = &glyphs.at(character);
        } catch (const std::out_of_range&) {
            glyph = &glyphs[ERROR_CHARACTER];
        }

        x += glyph->xadvance;

        *out_height = std::max(*out_height, static_cast<int>(roundf(glyph->yoff * scale)));
    }

    *out_width = static_cast<int>(roundf((x + 2) * scale));
}
