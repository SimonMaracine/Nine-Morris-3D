#include "application/application.h"
#include "nine_morris_3d/scenes/game/game_scene.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/scenes/loading/loading_scene.h"
#include "nine_morris_3d/layers/loading/loading_layer.h"

#include <fstream>
#include <stb_truetype.h>
#include <stb_image_write.h>

void blit_image(unsigned char* dest, int dest_width, int dest_height, unsigned char* image,
        int width, int height, int dest_x, int dest_y, float* s0, float* t0, float* s1, float* t1) {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int index = (y + dest_y) * dest_width + (x + dest_x);
            dest[index] = image[y * width + x];
        }
    }

    *s0 = (float) dest_x / (float) dest_width;
    *t0 = (float) dest_y / (float) dest_height;
    *s1 = (float) (dest_x + width) / (float) dest_width;
    *t1 = (float) (dest_y + height) / (float) dest_height;
}

int main() {
    // constexpr int width = 1024;
    // constexpr int height = 576;

    // Application* application = new Application(width, height, "Nine Morris 3D");

    // GameScene* game_scene = new GameScene(0, application);
    // LoadingScene* loading_scene = new LoadingScene(1, application);

    // application->add_scene(game_scene);
    // application->push_layer(new GameLayer(0, application, game_scene), game_scene);
    // application->push_layer(new GuiLayer(1, application, game_scene), game_scene);
    // application->push_layer(new ImGuiLayer(2, application, game_scene), game_scene);

    // application->add_scene(loading_scene);
    // application->push_layer(new LoadingLayer(0, application, loading_scene), loading_scene);

    // application->set_starting_scene(loading_scene);
    // application->run();

    // delete application;

    std::ifstream file ("data/fonts/OpenSans/OpenSans-Regular.ttf", std::ios::binary);
    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg(0, file.beg);

    char* buffer = new char[length];
    file.read(buffer, length);

    int bitmap_width = 1024, bitmap_height = 1024;
    unsigned char* bitmap = new unsigned char[bitmap_width * bitmap_height];

    stbtt_fontinfo font_info;
    stbtt_InitFont(&font_info, (unsigned char*) buffer, 0);

    float sf = stbtt_ScaleForPixelHeight(&font_info, 128.0f);

    int x = 0, y = 0;
    int max_row_height = 0;

    for (int codepoint = 32; codepoint <= 127; codepoint++) {
        int width, height, xoff, yoff;
        unsigned char* glyph = stbtt_GetCodepointSDF(&font_info, sf, codepoint, 5, 180, 36, &width, &height, &xoff, &yoff);

        if (x + width > bitmap_width) {
            y += max_row_height;
            x = 0;
            max_row_height = 0;
        }

        float s0, t0, s1, t1;
        blit_image(bitmap, bitmap_width, bitmap_height, glyph, width, height, x, y, &s0, &t0, &s1, &t1);

        printf("%f, %f\n", s0, t0);
        printf("%f, %f\n", s1, t1);

        stbtt_FreeSDF(glyph, nullptr);

        x += width;
        max_row_height = std::max(max_row_height, height);
    }

    stbi_write_png("test.png", bitmap_width, bitmap_height, 1, bitmap, 0);

    printf("Done\n");

    delete[] bitmap;
    delete[] buffer;
}
