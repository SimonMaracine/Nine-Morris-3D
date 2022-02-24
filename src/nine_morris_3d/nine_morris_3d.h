#pragma once

#include <memory>

#include "application/application.h"
#include "graphics/renderer/hoverable.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/buffer.h"
#include "graphics/renderer/shader.h"
#include "graphics/renderer/texture.h"
#include "graphics/renderer/material.h"
#include "nine_morris_3d/options.h"
#include "other/model.h"

constexpr int VERSION_MAJOR = 0;
constexpr int VERSION_MINOR = 1;
constexpr int VERSION_PATCH = 0;

class NineMorris3D : public Application {
public:
    NineMorris3D();

    static void set_app_pointer(NineMorris3D* instance);

    options::Options options;
    unsigned int arrow_cursor = 0;
    unsigned int cross_cursor = 0;

    struct RenderData {
        bool loaded_board = false;
        hoverable::Id board_id = hoverable::null;
        std::shared_ptr<VertexArray> board_vertex_array;
        std::shared_ptr<Shader> board_shader;
        std::shared_ptr<Texture> board_diffuse_texture;

        std::shared_ptr<Material> board_material;
        std::shared_ptr<MaterialInstance> board_material_instance;
    } data;
};

extern NineMorris3D* app;
