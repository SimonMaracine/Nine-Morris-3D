#include <stb_truetype.h>
#include <stb_image_write.h>
#include <glad/glad.h>

#include <utf8.h>

#include "engine/application/platform.h"
#include "engine/graphics/font.h"
#include "engine/graphics/opengl/buffer.h"
#include "engine/graphics/opengl/vertex_array.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"
#include "engine/other/exit.h"

static constexpr char32_t ERROR_CHARACTER = 127;

static std::string get_name(std::string_view file_path) {
    size_t last_slash = file_path.find_last_of("/");
    ASSERT(last_slash != std::string::npos, "Could not find slash");

    return std::string(file_path.substr(last_slash + 1));
}

static const char* get_font_file_data(std::string_view file_path) {
    std::ifstream file {std::string(file_path), std::ios::binary};

    if (!file.is_open()) {
        REL_CRITICAL("Could not open file `{}` for reading, exiting...", file_path);
        application_exit::panic();
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

            ASSERT(index < static_cast<size_t>(dest_width * dest_height), "Write out of bounds");

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
    font_info_buffer = get_font_file_data(file_path);

    if (!stbtt_InitFont(&info, reinterpret_cast<const unsigned char*>(font_info_buffer), 0)) {
        REL_CRITICAL("Could not load font `{}`, exiting...", file_path);
        application_exit::panic();
    }

    sf = stbtt_ScaleForPixelHeight(&info, size);

    initialize();

    name = get_name(file_path);

    DEB_DEBUG("Loaded font `{}`", file_path);
}

Font::~Font() {
    glDeleteTextures(1, &texture);
    delete[] font_info_buffer;

    DEB_DEBUG("Unloaded font `{}`", name);
}

void Font::update_data(const float* data, size_t size) {
    buffer->bind();
    buffer->upload_data(data, size);

    gl::VertexBuffer::unbind();

    static constexpr size_t FLOATS_PER_VERTEX = 4;

    ASSERT(size % (sizeof(float) * FLOATS_PER_VERTEX) == 0, "Data may be corrupted");

    vertex_count = static_cast<int>(size / (sizeof(float) * FLOATS_PER_VERTEX));
}

void Font::begin_baking() {
    DEB_DEBUG("Begin baking font `{}`", name);

    glDeleteTextures(1, &texture);

    const size_t SIZE = sizeof(unsigned char) * bitmap_size * bitmap_size;

    bake_context = BakeContext {};
    bake_context.bitmap = new unsigned char[SIZE];
    memset(bake_context.bitmap, 0, SIZE);

    glyphs.clear();

    // This character should always be present
    bake_character(ERROR_CHARACTER);
}

void Font::end_baking() {
    glGenTextures(1, &texture);  // TODO maybe move this to Texture class
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float border_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, bitmap_size, bitmap_size);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, bitmap_size, bitmap_size, GL_RED, GL_UNSIGNED_BYTE, bake_context.bitmap);

    glBindTexture(GL_TEXTURE_2D, 0);

#ifdef NM3D_PLATFORM_DEBUG
    write_bitmap_to_file();
#endif

    delete[] bake_context.bitmap;

    DEB_DEBUG("End baking font `{}`", name);
}

void Font::bake_characters(int begin_codepoint, int end_codepoint) {
    int descent;
    stbtt_GetFontVMetrics(&info, nullptr, &descent, nullptr);

    for (int codepoint = begin_codepoint; codepoint <= end_codepoint; codepoint++) {
        try_bake_character(codepoint, descent);
    }
}

void Font::bake_characters(const char* string) {
    int descent;
    stbtt_GetFontVMetrics(&info, nullptr, &descent, nullptr);

    const std::u32string utf32_string = utf8::utf8to32(string);

    for (const char32_t character : utf32_string) {
        try_bake_character(character, descent);
    }
}

void Font::bake_character(int codepoint) {
    int descent;
    stbtt_GetFontVMetrics(&info, nullptr, &descent, nullptr);

    try_bake_character(codepoint, descent);
}

void Font::bake_ascii() {
    bake_characters(32, 126);
}

void Font::render(std::string_view string, std::vector<float>& buffer) {
    const std::u32string utf32_string = utf8::utf8to32(string);

    int x = 0;

    for (const char32_t character : utf32_string) {
        const Glyph& glyph = get_character_glyph(character);

        const float x0 = static_cast<float>(x + glyph.xoff);
        const float y0 = -static_cast<float>(glyph.height - glyph.yoff);
        const float x1 = static_cast<float>(x + glyph.xoff + glyph.width);
        const float y1 = static_cast<float>(glyph.yoff);

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

std::pair<int, int> Font::get_string_size(std::string_view string, float scale) {
    const std::u32string utf32_string = utf8::utf8to32(string);

    int x = 0;
    int height = 0;

    for (const char32_t character : utf32_string) {
        const Glyph& glyph = get_character_glyph(character);

        x += glyph.xadvance;

        height = std::max(height, static_cast<int>(std::roundf(static_cast<float>(glyph.yoff) * scale)));
    }

    const int width = static_cast<int>(std::roundf((x + padding * 2) * scale));  // Take padding into consideration

    return std::make_pair(width, height);
}

void Font::initialize() {
    buffer = std::make_shared<gl::VertexBuffer>(gl::DrawHint::Stream);

    VertexBufferLayout layout = VertexBufferLayout {}
        .add(0, VertexBufferLayout::Float, 2)
        .add(1, VertexBufferLayout::Float, 2);

    vertex_array = std::make_shared<gl::VertexArray>();
    vertex_array->begin_definition()
        .add_buffer(buffer, layout)
        .end_definition();
}

void Font::try_bake_character(int codepoint, int descent) {
    if (glyphs.count(codepoint) > 0) {
        DEB_WARNING("Character with codepoint `{}` is already baked");
        return;
    }

    int advance_width, left_side_bearing;
    stbtt_GetCodepointHMetrics(&info, codepoint, &advance_width, &left_side_bearing);

    int y0;
    stbtt_GetCodepointBitmapBox(&info, codepoint, sf, sf, nullptr, &y0, nullptr, nullptr);

    int width = 0, height = 0;  // Assume 0, because glyph can be null
    unsigned char* glyph = stbtt_GetCodepointSDF(
        &info,
        sf,
        codepoint,
        padding,
        on_edge_value,
        static_cast<float>(pixel_dist_scale),
        &width,
        &height,
        nullptr,
        nullptr
    );

    if (glyph == nullptr) {
        DEB_WARNING("Could not bake character with codepoint `{}`; still adding to map...", codepoint);
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

const Font::Glyph& Font::get_character_glyph(char32_t character) {
    try {
        return glyphs.at(character);
    } catch (const std::out_of_range&) {
        return glyphs[ERROR_CHARACTER];
    }
}

void Font::write_bitmap_to_file() {
    const std::string file_name = "bitmap_" + name + ".png";

    if (!stbi_write_png(file_name.c_str(), bitmap_size, bitmap_size, 1, bake_context.bitmap, 0)) {
        DEB_ERROR("Failed to create bitmap png file `{}`", file_name);
    }
}
