#include <string>
#include <fstream>

#include <stb_truetype.h>
#include <glad/glad.h>

#include "opengl/renderer/font.h"
#include "opengl/debug_opengl.h"
#include "other/logging.h"

Font::Font(const std::string& file_path, float size, unsigned int bitmap_size)
    : bitmap_size(bitmap_size) {
    const unsigned char* file_data = get_file_data(file_path);

    bitmap = new unsigned char[bitmap_size * bitmap_size];

    stbtt_BakeFontBitmap(file_data, 0, size, bitmap, bitmap_size, bitmap_size, 32, 96, character_data);

    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, bitmap_size, bitmap_size);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, bitmap_size, bitmap_size, GL_RED, GL_UNSIGNED_BYTE, bitmap);
    LOG_ALLOCATION(bitmap_size * bitmap_size * 4);

    glBindTexture(GL_TEXTURE_2D, 0);

    delete[] bitmap;
}

Font::~Font() {
    glDeleteTextures(1, texture)
}

const unsigned char* Font::get_file_data(const std::string& file_path) {
    std::ifstream file (file_path, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        REL_CRITICAL("Could not open file '{}' for reading", file_path.c_str());
        std::exit(1);
    }

    // TODO implement this

    return nullptr;
}
