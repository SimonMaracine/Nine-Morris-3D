#pragma once

#include <stb_truetype.h>
#include <glad/glad.h>

#include "nine_morris_3d_engine/graphics/opengl/vertex_array.h"

class Font {
public:
    struct Glyph {
        float s0, t0, s1, t1;
        int width, height;
        int xoff, yoff, xadvance;
    };

    Font(std::string_view file_path, float size, int padding, unsigned char on_edge_value,
        int pixel_dist_scale, int bitmap_size);
    ~Font();

    void update_data(const float* data, size_t size);

    std::unordered_map<int, Glyph>& get_glyphs() { return glyphs; }
    unsigned int get_bitmap_size() { return static_cast<unsigned int>(bitmap_size); }

    VertexArray& get_vertex_array() { return *vertex_array; }
    GLuint get_texture() { return texture; }
    unsigned int get_vertex_count() { return vertex_count; }

    // Baking API
    void begin_baking();
    void end_baking();
    void bake_characters(int begin_codepoint, int end_codepoint);
    void bake_character(int codepoint);

    // Call render to get the buffer of data used in the end by OpenGL; pointer returned needs to be deleted
    void render(std::string_view string, size_t* out_size, float** out_buffer);

    // Get width and height of a line of text
    void get_string_size(std::string_view string, float scale, int* out_width, int* out_height);
private:
    static const char* get_file_data(std::string_view file_path);
    static void blit_glyph(unsigned char* dest, int dest_width, int dest_height, unsigned char* glyph,
        int width, int height, int dest_x, int dest_y, float* s0, float* t0, float* s1, float* t1);
    static std::string get_name(std::string_view file_path);

    struct BakeContext {
        int x = 0, y = 0;
        int max_row_height = 0;
        unsigned char* bitmap = nullptr;
    } bake_context;

    std::unordered_map<int, Glyph> glyphs;

    stbtt_fontinfo info;
    const char* font_file_buffer = nullptr;
    int bitmap_size = 0;
    int padding = 0;
    unsigned char on_edge_value = 0;
    int pixel_dist_scale = 0;
    float sf = 0.0f;

    std::string name;

    GLuint texture = 0;

    // Store references to vertex array and buffer
    std::shared_ptr<VertexArray> vertex_array;
    std::shared_ptr<Buffer> buffer;
    unsigned int vertex_count = 0;
};
