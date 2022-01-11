#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
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

    stbtt_InitFont(&info, (unsigned char*) font_file_buffer, 0);
    sf = stbtt_ScaleForPixelHeight(&info, size);



    // stbtt_BakeFontBitmap((const unsigned char*) file_data, 0, size, bitmap, bitmap_size, bitmap_size, 32, 96, character_data);

    // delete[] file_data;

    // // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // glGenTextures(1, &texture);
    // glBindTexture(GL_TEXTURE_2D, texture);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, bitmap_size, bitmap_size);
    // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, bitmap_size, bitmap_size, GL_RED, GL_UNSIGNED_BYTE, bitmap);
    // LOG_ALLOCATION(bitmap_size * bitmap_size * 4);

    // glBindTexture(GL_TEXTURE_2D, 0);

    // // glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // if (!stbi_write_png("bitmap_test.png", bitmap_size, bitmap_size, 1, bitmap, 0)) {
    //     SPDLOG_CRITICAL("Failed to create png");
    //     std::exit(1);
    // }

    // delete[] bitmap;

    buffer = Buffer::create(1);

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 2);
    layout.add(1, BufferLayout::Type::Float, 2);

    vertex_array = VertexArray::create();
    vertex_array->add_buffer(buffer, layout);

    VertexArray::unbind();
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
    memset(&bake_context, 0, sizeof(BakeContext));
    bake_context.bitmap = new unsigned char[bitmap_size * bitmap_size];
}

void Font::bake_characters(int begin_codepoint, int end_codepoint) {
    for (int codepoint = begin_codepoint; codepoint <= end_codepoint; codepoint++) {
        int width, height, xoff, yoff;
        unsigned char* glyph = stbtt_GetCodepointSDF(&info, sf, codepoint, padding, onedge_value,
                (float) pixel_dist_scale, &width, &height, &xoff, &yoff);

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

        // TODO fill data

        glyphs[codepoint] = gl;
    }
}

void Font::bake_character(int codepoint) {

}

void Font::end_baking() {

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
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int index = (y + dest_y) * dest_width + (x + dest_x);
            dest[index] = glyph[y * width + x];
        }
    }

    *s0 = (float) dest_x / (float) dest_width;
    *t0 = (float) dest_y / (float) dest_height;
    *s1 = (float) (dest_x + width) / (float) dest_width;
    *t1 = (float) (dest_y + height) / (float) dest_height;
}
