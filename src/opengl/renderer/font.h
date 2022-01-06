#pragma once

#include <string>
#include <memory>

#include <stb_truetype.h>
#include <glad/glad.h>

class Font {
public:
    Font(const std::string& file_path, float size, unsigned int bitmap_size);
    ~Font();

    stbtt_bakedchar* get_chars() const { return character_data; }
    unsigned int get_bitmap_size() const { return bitmap_size; }
private:
    static const unsigned char* get_file_data(const std::string& file_path);

    stbtt_bakedchar character_data[96];  // ASCII 32..126 is 95 glyphs
    unsigned char* bitmap = nullptr;
    unsigned int bitmap_size = 0;
    GLuint texture;
};

template<typename T>
using Rc = std::shared_ptr<T>;
