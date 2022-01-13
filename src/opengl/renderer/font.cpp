#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <string.h>

#include <stb_truetype.h>
#include <stb_image_write.h>
#include <glad/glad.h>

#include "opengl/renderer/font.h"
#include "opengl/renderer/buffer.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/debug_opengl.h"
#include "other/logging.h"

Font::Font(const std::string& file_path, float size, int padding, unsigned char onedge_value,
        int pixel_dist_scale, int bitmap_size)
    : bitmap_size(bitmap_size), padding(padding), onedge_value(onedge_value),
      pixel_dist_scale(pixel_dist_scale) {
    font_file_buffer = get_file_data(file_path);

    if (!stbtt_InitFont(&info, (unsigned char*) font_file_buffer, 0)) {
        REL_CRITICAL("Could not load font '{}'", file_path.c_str());
        std::exit(1);
    }
    sf = stbtt_ScaleForPixelHeight(&info, size);

    buffer = Buffer::create(1);

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 2);
    layout.add(1, BufferLayout::Type::Float, 2);

    vertex_array = VertexArray::create();
    vertex_array->add_buffer(buffer, layout);

    VertexArray::unbind();

    name = get_name(file_path);

    SPDLOG_DEBUG("Loaded font '{}'", file_path.c_str());
}

Font::~Font() {
    glDeleteTextures(1, &texture);
    delete[] font_file_buffer;
}

void Font::update_data(const float* data, size_t size) {
    buffer->bind();
    buffer->update_data(data, size);
    vertex_count = size / sizeof(float);
}

void Font::begin_baking() {
    SPDLOG_DEBUG("Begin baking font {}", name.c_str());

    glDeleteTextures(1, &texture);
    memset(&bake_context, 0, sizeof(BakeContext));
    bake_context.bitmap = new unsigned char[sizeof(unsigned char) * bitmap_size * bitmap_size];
    memset(bake_context.bitmap, 0, sizeof(unsigned char) * bitmap_size * bitmap_size);
}

void Font::bake_characters(int begin_codepoint, int end_codepoint) {
    int ascent;
    stbtt_GetFontVMetrics(&info, &ascent, nullptr, nullptr);

    for (int codepoint = begin_codepoint; codepoint <= end_codepoint; codepoint++) {
        int advance_width, left_side_bearing;
        stbtt_GetCodepointHMetrics(&info, codepoint, &advance_width, &left_side_bearing);

        int x1, y1, x2, y2;
        stbtt_GetCodepointBitmapBox(&info, codepoint, sf, sf, &x1, &y1, &x2, &y2);  // TODO don't retreive all

        int width = 0, height = 0;  // Assume 0, because glyph can be NULL
        unsigned char* glyph = stbtt_GetCodepointSDF(&info, sf, codepoint, padding, onedge_value,
                (float) pixel_dist_scale, &width, &height, nullptr, nullptr);

        if (bake_context.x + width > bitmap_size) {
            bake_context.y += bake_context.max_row_height;
            bake_context.x = 0;
            bake_context.max_row_height = 0;
        }

        float s0, t0, s1, t1;
        blit_glyph(bake_context.bitmap, bitmap_size, bitmap_size, glyph, width, height, bake_context.x,
                bake_context.y, &s0, &t0, &s1, &t1);

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
        gl.xoff = (int) std::roundf(left_side_bearing * sf);
        gl.yoff = (int) std::roundf((ascent + y1) * sf);
        gl.xadvance = (int) std::roundf(advance_width * sf);

        assert(glyphs.count(codepoint) == 0);

        glyphs[codepoint] = gl;
    }
}

void Font::bake_character(int codepoint) {
    int ascent;
    stbtt_GetFontVMetrics(&info, &ascent, nullptr, nullptr);

    int advance_width, left_side_bearing;
    stbtt_GetCodepointHMetrics(&info, codepoint, &advance_width, &left_side_bearing);

    int x1, y1, x2, y2;
    stbtt_GetCodepointBitmapBox(&info, codepoint, sf, sf, &x1, &y1, &x2, &y2);

    int width = 0, height = 0;  // Assume 0, because glyph can be NULL
    unsigned char* glyph = stbtt_GetCodepointSDF(&info, sf, codepoint, padding, onedge_value,
            (float) pixel_dist_scale, &width, &height, nullptr, nullptr);

    if (bake_context.x + width > bitmap_size) {
        bake_context.y += bake_context.max_row_height;
        bake_context.x = 0;
        bake_context.max_row_height = 0;
    }

    float s0, t0, s1, t1;
    blit_glyph(bake_context.bitmap, bitmap_size, bitmap_size, glyph, width, height, bake_context.x,
            bake_context.y, &s0, &t0, &s1, &t1);

    stbtt_FreeSDF(glyph, nullptr);

    bake_context.x += width;
    bake_context.max_row_height = std::max(bake_context.max_row_height, height);

    Glyph gl;
    gl.s0 = s0;
    gl.t0 = t0;
    gl.s1 = s1;
    gl.t1 = t1;
    gl.xoff = (int) std::roundf(left_side_bearing * sf);
    gl.yoff = (int) std::roundf((ascent + y1) * sf);
    gl.xadvance = (int) std::roundf(advance_width * sf);

    assert(glyphs.count(codepoint) == 0);

    glyphs[codepoint] = gl;
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
    LOG_ALLOCATION(bitmap_size * bitmap_size * 4);

    glBindTexture(GL_TEXTURE_2D, 0);

#ifndef NDEBUG
    std::string file_name = "bitmap_" + name + ".png";
    if (!stbi_write_png(file_name.c_str(), bitmap_size, bitmap_size, 1, bake_context.bitmap, 0)) {
        SPDLOG_CRITICAL("Failed to create png");
        std::exit(1);
    }
#endif

    delete[] bake_context.bitmap;

    SPDLOG_DEBUG("End baking font {}", name.c_str());
}

const char* Font::get_file_data(const std::string& file_path) {
    std::ifstream file (file_path, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        REL_CRITICAL("Could not open file '{}' for reading", file_path.c_str());
        std::exit(1);
    }

    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg(0, file.beg);

    char* buffer = new char[length];
    file.read(buffer, length);

    return buffer;
}

void Font::blit_glyph(unsigned char* dest, int dest_width, int dest_height, unsigned char* glyph,
        int width, int height, int dest_x, int dest_y, float* s0, float* t0, float* s1, float* t1) {
    if (glyph != nullptr) {
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                int index = (y + dest_y) * dest_width + (x + dest_x);
                dest[index] = glyph[y * width + x];
            }
        }
    }

    *s0 = (float) dest_x / (float) dest_width;
    *t0 = (float) dest_y / (float) dest_height;
    *s1 = (float) (dest_x + width) / (float) dest_width;
    *t1 = (float) (dest_y + height) / (float) dest_height;
}

std::string Font::get_name(const std::string& file_path) {
    std::vector<std::string> tokens;

    char copy[256];
    strcpy(copy, file_path.c_str());

    char* token = strtok(copy, "/.");

    while (token != nullptr) {
        tokens.push_back(token);
        token = strtok(nullptr, "/.");
    }

    assert(tokens.size() >= 2);

    return tokens[tokens.size() - 2];  // It's ok
}
