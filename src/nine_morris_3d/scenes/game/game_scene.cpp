#include <vector>
#include <array>
#include <memory>
#include <cassert>
#include <stdlib.h>
#include <time.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "application/app.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/buffer.h"
#include "graphics/renderer/texture.h"
#include "graphics/renderer/buffer_layout.h"
#include "graphics/renderer/camera.h"
#include "graphics/renderer/light.h"
#include "graphics/debug_opengl.h"
#include "other/texture_data.h"
#include "other/assets.h"
#include "other/logging.h"
#include "nine_morris_3d/scenes/game/game_scene.h"
#include "nine_morris_3d/board.h"
#include "nine_morris_3d/save_load.h"
#include "nine_morris_3d/options.h"

void GameScene::on_enter() {
    SPDLOG_DEBUG("Enter game scene");

    srand(time(nullptr));

    board_state_history = std::make_shared<std::vector<Board>>();
    build_board();

    if (!app->storage->white_piece_diff_texture) {
        if (app->options.texture_quality == options::NORMAL) {
            app->storage->white_piece_diff_texture = Texture::create(app->assets_load->white_piece_diff_texture, true, -1.5f);
            app->storage->black_piece_diff_texture = Texture::create(app->assets_load->black_piece_diff_texture, true, -1.5f);
        } else if (app->options.texture_quality == options::LOW) {
            app->storage->white_piece_diff_texture = Texture::create(app->assets_load->white_piece_diff_texture_small, true, -1.5f);
            app->storage->black_piece_diff_texture = Texture::create(app->assets_load->black_piece_diff_texture_small, true, -1.5f);
        } else {
            assert(false);
        }
    }

    for (unsigned int i = 0; i < 9; i++) {
        build_piece(i, Piece::Type::White, app->assets_load->white_piece_mesh,
                app->storage->white_piece_diff_texture, glm::vec3(-4.0f, 0.3f, -2.0f + i * 0.5f));
    }
    for (unsigned int i = 9; i < 18; i++) {
        build_piece(i, Piece::Type::Black, app->assets_load->black_piece_mesh,
                app->storage->black_piece_diff_texture, glm::vec3(4.0f, 0.3f, -2.0f + (i - 9) * 0.5f));
    }

    for (unsigned int i = 0; i < 24; i++) {
        build_node(i, NODE_POSITIONS[i]);
    }

    build_board_paint();
    build_camera();
    build_skybox();
    build_light();
    build_turn_indicator();
}

void GameScene::on_exit() {
    SPDLOG_DEBUG("Exit game scene");

    options::save_options_to_file(app->options);

    if (app->options.save_on_exit && !app->running) {
        board.finalize_pieces_state();

        save_load::GameState state;
        state.board = board;
        state.camera = camera;
        state.time = timer.get_time_raw();

        const time_t current = time(nullptr);
        state.date = ctime(&current);

        save_load::save_game(state);
    }

    timer = Timer();
}

std::shared_ptr<Buffer> GameScene::create_ids_buffer(unsigned int vertices_size, hoverable::Id id) {
    std::vector<int> array;
    array.resize(vertices_size);
    for (unsigned int i = 0; i < array.size(); i++) {
        array[i] = (int) id;
    }
    std::shared_ptr<Buffer> buffer = Buffer::create(array.data(), array.size() * sizeof(int));

    return buffer;
}

std::shared_ptr<VertexArray> GameScene::create_entity_vertex_array(std::shared_ptr<model::Mesh<model::Vertex>> mesh,
        hoverable::Id id) {
    std::shared_ptr<Buffer> vertices = Buffer::create(mesh->vertices.data(),
            mesh->vertices.size() * sizeof(model::Vertex));

    std::shared_ptr<Buffer> ids = create_ids_buffer(mesh->vertices.size(), id);

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    layout.add(1, BufferLayout::Type::Float, 2);
    layout.add(2, BufferLayout::Type::Float, 3);

    BufferLayout layout2;
    layout2.add(3, BufferLayout::Type::Int, 1);

    std::shared_ptr<IndexBuffer> indices = IndexBuffer::create(mesh->indices.data(),
            mesh->indices.size() * sizeof(unsigned int));

    std::shared_ptr<VertexArray> vertex_array = VertexArray::create();
    vertex_array->add_buffer(vertices, layout);
    vertex_array->add_buffer(ids, layout2);
    vertex_array->add_index_buffer(indices);

    VertexArray::unbind();

    return vertex_array;
}

void GameScene::build_board() {
    if (!app->storage->board_vertex_array) {
        hoverable::Id id = hoverable::generate_id();
        app->storage->board_id = id;

        app->storage->board_vertex_array = create_entity_vertex_array(app->assets_load->board_mesh, id);
    }

    if (!app->storage->board_wood_diff_texture) {
        if (app->options.texture_quality == options::NORMAL) {
            app->storage->board_wood_diff_texture =
                    Texture::create(app->assets_load->board_wood_diff_texture, true, -2.0f);
        } else if (app->options.texture_quality == options::LOW) {
            app->storage->board_wood_diff_texture =
                    Texture::create(app->assets_load->board_wood_diff_texture_small, true, -2.0f);
        } else {
            assert(false);
        }
    }

    board = Board(app->storage->board_id, board_state_history);

    board.scale = 20.0f;
    board.vertex_array = app->storage->board_vertex_array;
    board.index_count = app->assets_load->board_mesh->indices.size();
    board.diffuse_texture = app->storage->board_wood_diff_texture;
    board.specular_color = glm::vec3(0.2f);
    board.shininess = 4.0f;

    SPDLOG_DEBUG("Built board");
}

void GameScene::build_board_paint() {
    if (!app->storage->board_paint_vertex_array) {
        std::shared_ptr<Buffer> vertices = Buffer::create(app->assets_load->board_paint_mesh->vertices.data(),
                app->assets_load->board_paint_mesh->vertices.size() * sizeof(model::Vertex));

        std::shared_ptr<VertexArray> vertex_array = VertexArray::create();

        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 3);
        layout.add(1, BufferLayout::Type::Float, 2);
        layout.add(2, BufferLayout::Type::Float, 3);

        std::shared_ptr<IndexBuffer> indices = IndexBuffer::create(app->assets_load->board_paint_mesh->indices.data(),
                app->assets_load->board_paint_mesh->indices.size() * sizeof(unsigned int));

        vertex_array->add_buffer(vertices, layout);
        vertex_array->add_index_buffer(indices);

        VertexArray::unbind();

        app->storage->board_paint_vertex_array = vertex_array;
    }

    if (!app->storage->board_paint_diff_texture) {
        if (app->options.texture_quality == options::NORMAL) {
            app->storage->board_paint_diff_texture =
                    Texture::create(app->assets_load->board_paint_diff_texture, true, -1.0f);
        } else if (app->options.texture_quality == options::LOW) {
            app->storage->board_paint_diff_texture =
                    Texture::create(app->assets_load->board_paint_diff_texture_small, true, -1.0f);
        } else {
            assert(false);
        }
    }

    board.paint.position = glm::vec3(0.0f, 0.062f, 0.0f);
    board.paint.scale = 20.0f;
    board.paint.vertex_array = app->storage->board_paint_vertex_array;
    board.paint.index_count = app->assets_load->board_paint_mesh->indices.size();
    board.paint.diffuse_texture = app->storage->board_paint_diff_texture;
    board.paint.specular_color = glm::vec3(0.2f);
    board.paint.shininess = 4.0f;

    SPDLOG_DEBUG("Built board paint");
}

void GameScene::build_piece(unsigned int index, Piece::Type type, std::shared_ptr<model::Mesh<model::Vertex>> mesh,
        std::shared_ptr<Texture> texture, const glm::vec3& position) {
    if (!app->storage->piece_vertex_arrays[index]) {
        hoverable::Id id = hoverable::generate_id();
        app->storage->pieces_id[index] = id;

        app->storage->piece_vertex_arrays[index] = create_entity_vertex_array(mesh, id);
    }

    board.pieces[index] = Piece(app->storage->pieces_id[index], type);

    int random_rotation = rand() % 360;

    board.pieces[index].position = position;
    board.pieces[index].rotation = glm::vec3(0.0f, glm::radians((float) random_rotation), 0.0f);
    board.pieces[index].scale = 20.0f;
    board.pieces[index].vertex_array = app->storage->piece_vertex_arrays[index];
    board.pieces[index].index_count = mesh->indices.size();
    board.pieces[index].diffuse_texture = texture;
    board.pieces[index].specular_color = glm::vec3(0.2f);
    board.pieces[index].shininess = 4.0f;
    board.pieces[index].select_color = glm::vec3(1.0f, 0.0f, 0.0f);
    board.pieces[index].hover_color = glm::vec3(1.0f, 0.5f, 0.0f);

    SPDLOG_DEBUG("Built piece {}", index);
}

void GameScene::build_node(unsigned int index, const glm::vec3& position) {
    if (!app->storage->node_vertex_arrays[index]) {
        hoverable::Id id = hoverable::generate_id();
        app->storage->nodes_id[index] = id;

        std::shared_ptr<Buffer> vertices = Buffer::create(app->assets_load->node_mesh->vertices.data(),
                app->assets_load->node_mesh->vertices.size() * sizeof(model::VertexP));

        std::shared_ptr<Buffer> ids = create_ids_buffer(app->assets_load->node_mesh->vertices.size(), id);

        std::shared_ptr<VertexArray> vertex_array = VertexArray::create();

        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 3);
        BufferLayout layout2;
        layout2.add(1, BufferLayout::Type::Int, 1);

        std::shared_ptr<IndexBuffer> indices = IndexBuffer::create(app->assets_load->node_mesh->indices.data(),
                app->assets_load->node_mesh->indices.size() * sizeof(unsigned int));

        vertex_array->add_buffer(vertices, layout);
        vertex_array->add_buffer(ids, layout2);
        vertex_array->add_index_buffer(indices);

        VertexArray::unbind();

        app->storage->node_vertex_arrays[index] = vertex_array;
    }

    board.nodes[index] = Node(app->storage->nodes_id[index], index);

    board.nodes[index].position = position;
    board.nodes[index].scale = 20.0f;
    board.nodes[index].vertex_array = app->storage->node_vertex_arrays[index];
    board.nodes[index].index_count = app->assets_load->node_mesh->indices.size();

    SPDLOG_DEBUG("Built node {}", index);
}

void GameScene::build_camera() {
    camera = Camera(
        47.0f,
        glm::perspective(glm::radians(FOV), (float) app->data.width / (float) app->data.height, NEAR, FAR),
        glm::vec3(0.0f),
        8.0f
    );

    SPDLOG_DEBUG("Built camera");
}

void GameScene::build_skybox() {
    if (!app->storage->skybox_vertex_array) {
        std::shared_ptr<Buffer> vertices = Buffer::create(SKYBOX_VERTICES, sizeof(SKYBOX_VERTICES));

        std::shared_ptr<VertexArray> vertex_array = VertexArray::create();

        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 3);

        vertex_array->add_buffer(vertices, layout);

        VertexArray::unbind();

        app->storage->skybox_vertex_array = vertex_array;
    }

    if (!app->storage->skybox_texture) {
        std::array<std::shared_ptr<TextureData>, 6> data;

        if (app->options.texture_quality == options::NORMAL) {
            data = {
                app->assets_load->skybox_px_texture,
                app->assets_load->skybox_nx_texture,
                app->assets_load->skybox_py_texture,
                app->assets_load->skybox_ny_texture,
                app->assets_load->skybox_pz_texture,
                app->assets_load->skybox_nz_texture
            };
        } else if (app->options.texture_quality == options::LOW) {
            data = {
                app->assets_load->skybox_px_texture_small,
                app->assets_load->skybox_nx_texture_small,
                app->assets_load->skybox_py_texture_small,
                app->assets_load->skybox_ny_texture_small,
                app->assets_load->skybox_pz_texture_small,
                app->assets_load->skybox_nz_texture_small
            };
        } else {
            assert(false);
        }

        app->storage->skybox_texture = Texture3D::create(data);
    }

    SPDLOG_DEBUG("Built skybox");
}

void GameScene::build_light() {
    if (app->options.skybox == options::FIELD) {
        light = LIGHT_FIELD;
    } else if (app->options.skybox == options::AUTUMN) {
        light = LIGHT_AUTUMN;
    } else {
        assert(false);
    }

    SPDLOG_DEBUG("Built light");
}

void GameScene::build_turn_indicator() {
    if (!app->storage->white_indicator_texture) {
        app->storage->white_indicator_texture = Texture::create(app->assets_load->white_indicator_texture, false);
        app->storage->black_indicator_texture = Texture::create(app->assets_load->black_indicator_texture, false);
    }

    turn_indicator.position = glm::vec3((float) (app->data.width - 90), (float) (app->data.height - 115), 0.0f);
    turn_indicator.scale = 1.0f;

    SPDLOG_DEBUG("Built turn indicator");
}
