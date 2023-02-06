#pragma once

#include <stb_truetype.h>
#include <glad/glad.h>

#include "engine/graphics/opengl/vertex_array.h"
#include "engine/graphics/opengl/buffer.h"

class Font {
private:
    struct Glyph {
        float s0, t0, s1, t1;
        int width, height;
        int xoff, yoff, xadvance;
    };
public:
    Font(std::string_view file_path, float size, int padding, unsigned char on_edge_value,
        int pixel_dist_scale, int bitmap_size);
    ~Font();

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;
    Font(Font&&) = delete;
    Font& operator=(Font&&) = delete;

    void update_data(const float* data, size_t size);

    unsigned int get_bitmap_size() { return static_cast<unsigned int>(bitmap_size); }

    gl::VertexArray& get_vertex_array() { return *vertex_array; }
    GLuint get_texture() { return texture; }
    int get_vertex_count() { return vertex_count; }

    // Baking API
    void begin_baking();
    void end_baking();
    void bake_characters(int begin_codepoint, int end_codepoint);
    void bake_characters(const char* string);
    void bake_character(int codepoint);
    void bake_ascii();

    // Call render to get the buffer of data used in the end by OpenGL
    void render(std::string_view string, std::vector<float>& buffer);

    // Get width and height of a line of text
    std::pair<int, int> get_string_size(std::string_view string, float scale);
private:
    void initialize();
    void bake_character_and_add_to_map(int codepoint, int descent);
    const Glyph& get_character_glyph(char32_t character);
    void write_bitmap_to_file();

    struct BakeContext {
        int x = 0, y = 0;
        int max_row_height = 0;
        unsigned char* bitmap = nullptr;
    } bake_context;

    std::unordered_map<char32_t, Glyph> glyphs;

    stbtt_fontinfo info;
    const char* font_info_buffer = nullptr;
    int bitmap_size = 0;
    int padding = 0;  // Between glyphs
    unsigned char on_edge_value = 0;
    int pixel_dist_scale = 0;
    float sf = 0.0f;  // Scale factor

    std::string name;

    // Bitmap
    GLuint texture = 0;

    // Store references to vertex array and buffer
    std::shared_ptr<gl::VertexArray> vertex_array;
    std::shared_ptr<gl::VertexBuffer> buffer;
    int vertex_count = 0;
};
