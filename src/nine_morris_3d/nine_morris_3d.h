#pragma once

#include <imgui.h>

#include "application/platform.h"
#include "application/application.h"
#include "graphics/renderer/hoverable.h"
#include "graphics/renderer/material.h"
#include "graphics/renderer/font.h"
#include "graphics/renderer/opengl/vertex_array.h"
#include "graphics/renderer/opengl/buffer.h"
#include "graphics/renderer/opengl/shader.h"
#include "graphics/renderer/opengl/texture.h"
#include "nine_morris_3d/options.h"
#include "nine_morris_3d/assets_data.h"
#include "other/mesh.h"

#if defined(PLATFORM_GAME_LINUX)
    #define APP_NAME "ninemorris3d"
#elif defined(PLATFORM_GAME_WINDOWS)
    #define APP_NAME "NineMorris3D"
#endif

#if defined(PLATFORM_GAME_LINUX)
    #define LOG_FILE "log.txt"
    #define INFO_FILE "info.txt"
#elif defined(PLATFORM_GAME_WINDOWS)
    #define LOG_FILE "ninemorris3d_log.txt"
    #define INFO_FILE "ninemorris3d_info.txt"
#endif

constexpr unsigned int VERSION_MAJOR = 0;
constexpr unsigned int VERSION_MINOR = 1;
constexpr unsigned int VERSION_PATCH = 0;

class NineMorris3D : public Application {
public:
    NineMorris3D(std::string_view info_file, std::string_view log_file, std::string_view application_name);
    virtual ~NineMorris3D();

    static void set_app_pointer(NineMorris3D* instance);

    options::Options options;
    unsigned int arrow_cursor = 0;
    unsigned int cross_cursor = 0;

    std::shared_ptr<AssetsData> assets_data;

    struct RenderData {
        std::shared_ptr<Texture> splash_screen_texture;
        std::shared_ptr<Font> good_dog_plain_font;

        ImFont* imgui_info_font = nullptr;
        ImFont* imgui_windows_font = nullptr;

        std::shared_ptr<VertexArray> board_vertex_array;
        std::shared_ptr<Shader> board_shader;
        std::shared_ptr<Texture> board_diffuse_texture;
        std::shared_ptr<Texture> board_normal_texture;

        std::shared_ptr<Material> wood_material;
        std::shared_ptr<MaterialInstance> board_material_instance;

        hoverable::Id pieces_id[18];
        std::shared_ptr<VertexArray> piece_vertex_arrays[18];
        std::shared_ptr<Shader> piece_shader;
        std::shared_ptr<Texture> white_piece_diffuse_texture;
        std::shared_ptr<Texture> black_piece_diffuse_texture;
        std::shared_ptr<Texture> piece_normal_texture;

        std::shared_ptr<Material> tinted_wood_material;
        std::shared_ptr<MaterialInstance> piece_material_instances[18];

        hoverable::Id nodes_id[24];
        std::shared_ptr<VertexArray> node_vertex_arrays[24];
        std::shared_ptr<Shader> node_shader;

        std::shared_ptr<Material> basic_material;
        std::shared_ptr<MaterialInstance> node_material_instances[24];

        hoverable::Id board_paint_id = hoverable::null;
        std::shared_ptr<VertexArray> board_paint_vertex_array;
        std::shared_ptr<Shader> board_paint_shader;
        std::shared_ptr<Texture> board_paint_diffuse_texture;
        std::shared_ptr<Texture> board_paint_normal_texture;

        std::shared_ptr<Material> paint_material;
        std::shared_ptr<MaterialInstance> board_paint_material_instance;

        std::shared_ptr<Texture> white_indicator_texture;
        std::shared_ptr<Texture> black_indicator_texture;
    } data;
};

extern NineMorris3D* app;
