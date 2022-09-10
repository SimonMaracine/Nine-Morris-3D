#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "assets_data.h"

struct NineMorris3D : public Application {
    NineMorris3D(const ApplicationBuilder& builder);
    virtual ~NineMorris3D();

    void set_bloom(bool enable);
    void set_bloom_strength(float strength);

    // options::Options options;
    unsigned int arrow_cursor = 0;
    unsigned int cross_cursor = 0;

    // std::shared_ptr<AssetsData> assets_data;

    ImFont* imgui_info_font = nullptr;
    ImFont* imgui_windows_font = nullptr;

    // struct RenderData {
    //     std::shared_ptr<Texture> splash_screen_texture;
    //     std::shared_ptr<Font> good_dog_plain_font;

    //     ImFont* imgui_info_font = nullptr;
    //     ImFont* imgui_windows_font = nullptr;

    //     std::shared_ptr<Texture> board_normal_texture;
    //     std::shared_ptr<Texture> piece_normal_texture;
    
    //     std::shared_ptr<VertexArray> board_wood_vertex_array;
    //     std::shared_ptr<Shader> board_wood_shader;
    //     std::shared_ptr<Texture> board_wood_diffuse_texture;

    //     std::shared_ptr<Material> wood_material;
    //     std::shared_ptr<MaterialInstance> board_wood_material_instance;

    //     hover::Id pieces_id[18];
    //     std::shared_ptr<VertexArray> piece_vertex_arrays[18];
    //     std::shared_ptr<Shader> piece_shader;
    //     std::shared_ptr<Texture> white_piece_diffuse_texture;
    //     std::shared_ptr<Texture> black_piece_diffuse_texture;    

    //     std::shared_ptr<Material> tinted_wood_material;
    //     std::shared_ptr<MaterialInstance> piece_material_instances[18];

    //     hover::Id nodes_id[24];
    //     std::shared_ptr<VertexArray> node_vertex_arrays[24];
    //     std::shared_ptr<Shader> node_shader;

    //     std::shared_ptr<Material> basic_material;
    //     std::shared_ptr<MaterialInstance> node_material_instances[24];

    //     hover::Id board_paint_id = hover::null;
    //     std::shared_ptr<VertexArray> board_paint_vertex_array;
    //     std::shared_ptr<Shader> board_paint_shader;
    //     std::shared_ptr<Texture> board_paint_diffuse_texture;

    //     std::shared_ptr<Material> paint_material;
    //     std::shared_ptr<MaterialInstance> board_paint_material_instance;

    //     std::shared_ptr<Texture> white_indicator_texture;
    //     std::shared_ptr<Texture> black_indicator_texture;
    //     std::shared_ptr<Texture> wait_indicator_texture;
    //     std::shared_ptr<Texture> keyboard_controls_texture;
    //     std::shared_ptr<Texture> keyboard_controls_cross_texture;
    // } data;
};
