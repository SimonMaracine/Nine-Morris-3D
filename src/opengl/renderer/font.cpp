#include <string>
#include <fstream>
#include <vector>
#include <iterator>

#include <stb_truetype.h>
#include <glad/glad.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "opengl/renderer/temp_stb_image_write.h"

#include "opengl/renderer/font.h"
#include "opengl/renderer/buffer.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/debug_opengl.h"
#include "other/logging.h"

Font::Font(const std::string& file_path, float size, unsigned int bitmap_size)
    : bitmap_size(bitmap_size) {
    const char* file_data = get_file_data(file_path);

    unsigned char* bitmap = new unsigned char[bitmap_size * bitmap_size];

    stbtt_BakeFontBitmap(*((const unsigned char**) (&file_data)), 0, size, bitmap, bitmap_size, bitmap_size, 32, 96, character_data);

    delete[] file_data;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    if (!stbi_write_png("bitmap_test.png", bitmap_size, bitmap_size, 1, bitmap, 0)) {
        SPDLOG_CRITICAL("Failed to create png");
        std::exit(1);
    }

    delete[] bitmap;

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

void Font::update_data(const float* data, size_t size) {
    buffer->bind();
    buffer->update_data(data, size);
    vertex_count = size / sizeof(float);
}
