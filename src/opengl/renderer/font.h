#pragma once

#include <string>
#include <memory>

#include <stb_truetype.h>
#include <glad/glad.h>

#include "opengl/renderer/vertex_array.h"

class Font {
public:
    Font(const std::string& file_path, float size, unsigned int bitmap_size);
    ~Font();

    void update_data(const float* data, size_t size);

    const stbtt_bakedchar* get_chars() const { return character_data; }
    unsigned int get_bitmap_size() const { return bitmap_size; }

    const VertexArray* get_vertex_array() const { return vertex_array.get(); }
    GLuint get_texture() const { return texture; }
    unsigned int get_vertex_count() const { return vertex_count; }
private:
    static const char* get_file_data(const std::string& file_path);

    stbtt_bakedchar character_data[96];  // ASCII 32..126 is 95 glyphs
    unsigned int bitmap_size = 0;
    GLuint texture = 0;
    Rc<VertexArray> vertex_array;
    Rc<Buffer> buffer;
    unsigned int vertex_count = 0;
};
