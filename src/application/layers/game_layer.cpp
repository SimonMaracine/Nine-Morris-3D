#include <functional>
#include <memory>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "application/layers/game_layer.h"
#include "application/layers/imgui_layer.h"
#include "application/application.h"
#include "application/layer.h"
#include "application/window.h"
#include "application/events.h"
#include "application/input.h"
#include "opengl/debug_opengl.h"
#include "opengl/renderer/renderer.h"
#include "opengl/renderer/texture.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/buffer.h"
#include "ecs_and_game/components.h"
#include "ecs_and_game/systems.h"
#include "ecs_and_game/game.h"
#include "other/model.h"
#include "other/loader.h"
#include "other/logging.h"
#include "other/options.h"
#include "other/save_load.h"

void GameLayer::on_attach() {
    start();
    active = false;
}

void GameLayer::on_detach() {
    end();
}

void GameLayer::on_bind_layers() {
    
}

void GameLayer::on_update(float dt) {
    static bool loaded = false;

    if (!loaded) {
        assets = loader.get_assets();
        start_after_load();
        loaded = true;
    }

    systems::camera(registry, mouse_wheel, dx, dy, dt);
    systems::lighting_move(registry, dt);
    systems::move_pieces(registry, dt);
    // systems::node_move(registry, dt, camera);

    mouse_wheel = 0.0f;
    dx = 0.0f;
    dy = 0.0f;
}

void GameLayer::on_draw() {
    glm::mat4 projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 9.0f);
    glm::mat4 view = glm::lookAt(LIGHT_POSITION / 4.0f,
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 light_space_matrix = projection * view;
    storage->shadow_shader->bind();
    storage->shadow_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);

    storage->depth_map_framebuffer->bind();

    renderer::clear(renderer::Depth);
    renderer::set_viewport(2048, 2048);

    systems::render_to_depth(registry);

    storage->scene_framebuffer->bind();

    renderer::clear(renderer::Color | renderer::Depth | renderer::Stencil);
    renderer::set_viewport(application->data.width, application->data.height);
    renderer::set_stencil_mask_zero();

    storage->board_shader->bind();
    storage->board_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);
    storage->board_shader->set_uniform_int("u_shadow_map", 1);
    storage->board_paint_shader->bind();
    storage->board_paint_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);
    storage->board_paint_shader->set_uniform_int("u_shadow_map", 1);
    storage->piece_shader->bind();
    storage->piece_shader->set_uniform_matrix("u_light_space_matrix", light_space_matrix);
    storage->piece_shader->set_uniform_int("u_shadow_map", 1);
    renderer::bind_texture(storage->depth_map_framebuffer->get_depth_attachment(), 1);

    systems::load_projection_view(registry, camera);
    systems::skybox_render(registry, camera, storage);
    systems::lighting(registry, camera, storage);
    systems::board_render(registry);
    systems::board_paint_render(registry);
    systems::piece_render(registry, hovered_entity, camera, storage);
    systems::node_render(registry, hovered_entity, board);
    systems::origin_render(registry, camera, storage);
    systems::lighting_render(registry, camera, storage);

    Framebuffer::resolve_framebuffer(storage->scene_framebuffer->get_id(),
                                     storage->intermediate_framebuffer->get_id(),
                                     application->data.width, application->data.height);

    storage->intermediate_framebuffer->bind();

    int x = input::get_mouse_x();
    int y = application->data.height - input::get_mouse_y();
    hovered_entity = (entt::entity) storage->intermediate_framebuffer->read_pixel(1, x, y);

    Framebuffer::bind_default();

    renderer::clear(renderer::Color);
    storage->screen_quad_shader->bind();
    storage->screen_quad_shader->set_uniform_int("u_screen_texture", 0);
    renderer::bind_texture(storage->intermediate_framebuffer->get_color_attachment(0), 0);
    renderer::draw_screen_quad();
}

void GameLayer::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher = Dispatcher(event);

    dispatcher.dispatch<MouseScrolledEvent>(MouseScrolled, BIND(GameLayer::on_mouse_scrolled));
    dispatcher.dispatch<MouseMovedEvent>(MouseMoved, BIND(GameLayer::on_mouse_moved));
    dispatcher.dispatch<MouseButtonPressedEvent>(MouseButtonPressed, BIND(GameLayer::on_mouse_button_pressed));
    dispatcher.dispatch<MouseButtonReleasedEvent>(MouseButtonReleased, BIND(GameLayer::on_mouse_button_released));
    dispatcher.dispatch<WindowResizedEvent>(WindowResized, BIND(GameLayer::on_window_resized));
}

bool GameLayer::on_mouse_scrolled(events::MouseScrolledEvent& event) {
    mouse_wheel = event.scroll;

    return true;
}

bool GameLayer::on_mouse_moved(events::MouseMovedEvent& event) {
    dx = last_mouse_x - event.mouse_x;
    dy = last_mouse_y - event.mouse_y;
    last_mouse_x = event.mouse_x;
    last_mouse_y = event.mouse_y;

    return true;
}

bool GameLayer::on_mouse_button_pressed(events::MouseButtonPressedEvent& event) {
    systems::press(registry, board, hovered_entity);

    return false;
}

bool GameLayer::on_mouse_button_released(events::MouseButtonReleasedEvent& event) {
    auto& state = STATE(board);

    if (event.button == MOUSE_BUTTON_LEFT) {
        if (state.phase == Phase::PlacePieces) {
            if (state.should_take_piece) {
                systems::take_piece(registry, board, hovered_entity);
            } else {
                systems::place_piece(registry, board, hovered_entity);
            }
        } else if (state.phase == Phase::MovePieces) {
            if (state.should_take_piece) {
                systems::take_piece(registry, board, hovered_entity);
            } else {
                systems::select_piece(registry, board, hovered_entity);
                systems::put_piece(registry, board, hovered_entity);
            }
        }

        systems::release(registry, board);
    }

    return false;
}

bool GameLayer::on_window_resized(events::WindowResizedEvent& event) {
    storage->scene_framebuffer->resize(event.width, event.height);
    storage->intermediate_framebuffer->resize(event.width, event.height);
    systems::projection_matrix(registry, (float) event.width, (float) event.height);
    storage->orthographic_projection_matrix = glm::ortho(0.0f, (float) event.width, 0.0f, (float) event.height);

    return false;
}

void GameLayer::start() {
    logging::init();
    logging::log_opengl_and_dependencies_info(logging::LogTarget::Console);
    debug_opengl::maybe_init_debugging();
    input::init(application->window->get_handle());
    options::load_options_from_file(options);
    storage = renderer::init(application->data.width, application->data.height, options.samples);
    loader.start_loading_thread(options.texture_quality);
    application->window->set_vsync(options.vsync);

    auto [version_major, version_minor] = debug_opengl::get_version();
    if (!(version_major == 4 && version_minor >= 3)) {
        spdlog::critical("Graphics card must support minimum OpenGL 4.3");
        std::exit(1);
    }

    build_camera();
    build_directional_light();
    build_origin();

    srand(time(nullptr));

    SPDLOG_INFO("Finished initializing the first part of the program");
}

void GameLayer::start_after_load() {
    SPDLOG_INFO("Done loading assets; initializing the rest of the game...");

    build_skybox();

    storage->white_piece_diffuse_texture = Texture::create(assets->white_piece_diffuse_data, true, -1.5f);
    storage->black_piece_diffuse_texture = Texture::create(assets->black_piece_diffuse_data, true, -1.5f);

    for (int i = 0; i < 9; i++) {
        build_piece(i, Piece::White, assets->white_piece_mesh, storage->white_piece_diffuse_texture,
                    glm::vec3(-4.0f, 0.3f, -2.0f + i * 0.5f));
    }

    for (int i = 9; i < 18; i++) {
        build_piece(i, Piece::Black, assets->black_piece_mesh, storage->black_piece_diffuse_texture,
                    glm::vec3(4.0f, 0.3f, -2.0f + (i - 9) * 0.5f));
    }

    for (int i = 0; i < 24; i++) {
        build_node(i, NODE_POSITIONS[i]);
    }

    build_board();
    build_board_paint();
    build_turn_indicator();

    assets->board_diffuse_data = nullptr;
    assets->white_piece_diffuse_data = nullptr;
    assets->black_piece_diffuse_data = nullptr;
    assets->skybox_data.fill(nullptr);
    assets->board_paint_data = nullptr;
    assets->white_indicator_data = nullptr;
    assets->black_indicator_data = nullptr;

    SPDLOG_INFO("Finished initializing program");
    STOP_ALLOCATION_LOG;
}

void GameLayer::restart() {
    registry.clear();

    build_camera();
    build_directional_light();
    build_origin();
    build_skybox();

    for (int i = 0; i < 9; i++) {
        build_piece(i, Piece::White, assets->white_piece_mesh, storage->white_piece_diffuse_texture,
                    glm::vec3(-4.0f, 0.3f, -2.0f + i * 0.5f));
    }

    for (int i = 9; i < 18; i++) {
        build_piece(i, Piece::Black, assets->black_piece_mesh, storage->black_piece_diffuse_texture,
                    glm::vec3(4.0f, 0.3f, -2.0f + (i - 9) * 0.5f));
    }

    for (int i = 0; i < 24; i++) {
        build_node(i, NODE_POSITIONS[i]);
    }

    build_board();
    build_board_paint();
    build_turn_indicator();

    SPDLOG_INFO("Restarted game");
}

void GameLayer::end() {
    SPDLOG_INFO("Closing program");

    renderer::terminate();

    if (loader.get_thread().joinable()) {
        loader.get_thread().detach();
    }

    options::save_options_to_file(options);

    if (options.save_on_exit) {
        save_load::save_game(registry, save_load::gather_entities(board, camera, nodes, pieces));
    }
}

void GameLayer::set_scene_framebuffer(int samples) {
    int width = application->data.width;
    int height = application->data.height;
    storage->scene_framebuffer = Framebuffer::create(Framebuffer::Type::Scene, width, height, samples, 2);
}

void GameLayer::set_textures_quality(int quality) {
    if (quality == 0) {
        storage->board_diffuse_texture = Texture::create("data/textures/board/board_wood.png", true, -2.0f);
        storage->board_paint_texture = Texture::create("data/textures/board/board_paint.png", true, -1.0f);
        storage->white_piece_diffuse_texture = Texture::create("data/textures/piece/white_piece.png", true, -1.5f);
        storage->black_piece_diffuse_texture = Texture::create("data/textures/piece/black_piece.png", true, -1.5f);
    } else if (quality == 1) {
        storage->board_diffuse_texture = Texture::create("data/textures/board/board_wood-small.png", true, -2.0f);
        storage->board_paint_texture = Texture::create("data/textures/board/board_paint-small.png", true, -1.0f);
        storage->white_piece_diffuse_texture = Texture::create("data/textures/piece/white_piece-small.png", true, -1.5f);
        storage->black_piece_diffuse_texture = Texture::create("data/textures/piece/black_piece-small.png", true, -1.5f);
    } else {
        assert(false);
    }
}

void GameLayer::load_game() {
    registry.clear();

    save_load::Entities entities;
    save_load::load_game(registry, entities);

    save_load::reset_entities(entities, &board, &camera, nodes, pieces);

    rebuild_board_after_load();
    rebuild_camera_after_load();
    for (int i = 0; i < 9; i++) {
        rebuild_piece_after_load(pieces[i], assets->white_piece_mesh, storage->white_piece_diffuse_texture);
    }
    for (int i = 9; i < 18; i++) {
        rebuild_piece_after_load(pieces[i], assets->black_piece_mesh, storage->black_piece_diffuse_texture);
    }
    for (int i = 0; i < 24; i++) {
        rebuild_node_after_load(nodes[i]);
    }

    build_board_paint();
    build_skybox();
    build_directional_light();
#ifndef NDEBUG
    build_origin();
#endif
    build_turn_indicator();

    systems::projection_matrix(registry, (float) application->data.width, (float) application->data.height);

    SPDLOG_INFO("Loaded game");
}

Rc<Buffer> GameLayer::create_ids_buffer(unsigned int vertices_size, entt::entity entity) {
    std::vector<int> array;
    array.resize(vertices_size);
    for (unsigned int i = 0; i < array.size(); i++) {
        array[i] = (int) entt::to_integral(entity);
    }
    Rc<Buffer> buffer = Buffer::create(array.data(), array.size() * sizeof(int));

    return buffer;
}

Rc<VertexArray> GameLayer::create_entity_vertex_array(Rc<model::Mesh<FullVertex>> mesh, entt::entity entity) {
    Rc<Buffer> vertices = Buffer::create(mesh->vertices.data(), mesh->vertices.size() * sizeof(model::FullVertex));

    Rc<Buffer> ids = create_ids_buffer(mesh->vertices.size(), entity);

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    layout.add(1, BufferLayout::Type::Float, 2);
    layout.add(2, BufferLayout::Type::Float, 3);

    BufferLayout layout2;
    layout2.add(3, BufferLayout::Type::Int, 1);

    Rc<Buffer> index_buffer = Buffer::create_index(mesh->indices.data(), mesh->indices.size() * sizeof(unsigned int));

    Rc<VertexArray> vertex_array = VertexArray::create();
    index_buffer->bind();
    vertex_array->add_buffer(vertices, layout);
    vertex_array->add_buffer(ids, layout2);
    vertex_array->hold_index_buffer(index_buffer);

    VertexArray::unbind();

    return vertex_array;
}

void GameLayer::build_board() {;
    board = registry.create();

    Rc<VertexArray> vertex_array = create_entity_vertex_array(assets->board_mesh, board);

    if (!storage->board_diffuse_texture) {
        storage->board_diffuse_texture = Texture::create(assets->board_diffuse_data, true, -2.0f);
    }

    auto& transform = registry.emplace<TransformComponent>(board);
    transform.scale = 20.0f;

    registry.emplace<MeshComponent>(board, vertex_array, assets->board_mesh->indices.size());
    registry.emplace<MaterialComponent>(board, glm::vec3(0.25f), 8.0f);
    registry.emplace<ShadowComponent>(board);

    registry.emplace<GameStateComponent>(board, nodes);
    registry.emplace<MovesHistoryComponent>(board);

    SPDLOG_DEBUG("Built board entity {}", board);
}

void GameLayer::build_board_paint() {
    Rc<Buffer> vertices = Buffer::create(assets->board_paint_mesh->vertices.data(),
                                         assets->board_paint_mesh->vertices.size() * sizeof(model::FullVertex));
    Rc<VertexArray> vertex_array = VertexArray::create();

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    layout.add(1, BufferLayout::Type::Float, 2);
    layout.add(2, BufferLayout::Type::Float, 3);

    Rc<Buffer> index_buffer = Buffer::create_index(assets->board_paint_mesh->indices.data(),
                                                   assets->board_paint_mesh->indices.size() * sizeof(unsigned int));

    index_buffer->bind();
    vertex_array->add_buffer(vertices, layout);
    vertex_array->hold_index_buffer(index_buffer);

    VertexArray::unbind();

    if (!storage->board_paint_texture) {
        storage->board_paint_texture = Texture::create(assets->board_paint_data, true, -1.0f);
    }

    entt::entity board_paint = registry.create();

    auto& transform = registry.emplace<TransformComponent>(board_paint);
    transform.position = glm::vec3(0.0f, 0.062f, 0.0f);
    transform.scale = 20.0f;

    registry.emplace<MeshComponent>(board_paint, vertex_array, assets->board_paint_mesh->indices.size());
    registry.emplace<MaterialComponent>(board_paint, glm::vec3(0.25f), 8.0f);
    registry.emplace<BoardPaintComponent>(board_paint);

    SPDLOG_DEBUG("Built board paint entity {}", board_paint);
}

void GameLayer::build_camera() {
    camera = registry.create();

    auto& transform = registry.emplace<TransformComponent>(camera);
    transform.rotation = glm::vec3(47.0f, 0.0f, 0.0f);

    registry.emplace<CameraComponent>(camera,
        glm::perspective(glm::radians(45.0f), 1024.0f / 576.0f, 0.1f, 70.0f),
        glm::vec3(0.0f), 8.0f);
    registry.emplace<CameraMoveComponent>(camera);

    SPDLOG_DEBUG("Built camera entity {}", camera);
}

void GameLayer::build_skybox() {
    Rc<Buffer> positions = Buffer::create(skybox_points, sizeof(skybox_points));

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);

    storage->skybox_vertex_array = VertexArray::create();
    storage->skybox_vertex_array->add_buffer(positions, layout);
    VertexArray::unbind();

    if (!storage->skybox_texture) {
        storage->skybox_texture = Texture3D::create(assets->skybox_data);
    }

    entt::entity skybox = registry.create();

    registry.emplace<SkyboxComponent>(skybox);

    SPDLOG_DEBUG("Built skybox entity {}", skybox);
}

void GameLayer::build_piece(int id, Piece type, Rc<model::Mesh<FullVertex>> mesh,
                            Rc<Texture> diffuse_texture, const glm::vec3& position) {
    entt::entity piece = registry.create();
    pieces[id] = piece;

    Rc<VertexArray> vertex_array = create_entity_vertex_array(mesh, piece);

    int random_rotation = rand() % 360;

    auto& transform = registry.emplace<TransformComponent>(piece);
    transform.position = position;
    transform.rotation = glm::vec3(0.0f, glm::radians((float) random_rotation), 0.0f);
    transform.scale = 20.0f;

    registry.emplace<MeshComponent>(piece, vertex_array, mesh->indices.size());
    registry.emplace<MaterialComponent>(piece, glm::vec3(0.25f), 8.0f);
    registry.emplace<PieceTextureComponent>(piece, diffuse_texture);
    registry.emplace<OutlineComponent>(piece, storage->outline_shader, glm::vec3(1.0f, 0.0f, 0.0f));
    registry.emplace<ShadowComponent>(piece);

    registry.emplace<PieceComponent>(piece, id, type);
    registry.emplace<MoveComponent>(piece);

    SPDLOG_DEBUG("Built piece entity {}", piece);
}

void GameLayer::build_directional_light() {
    entt::entity directional_light = registry.create();

    auto& transform = registry.emplace<TransformComponent>(directional_light);
    transform.position = LIGHT_POSITION;
    transform.scale = 0.3f;

    registry.emplace<LightComponent>(directional_light, glm::vec3(0.15f), glm::vec3(0.8f), glm::vec3(1.0f));
#ifndef NDEBUG
    registry.emplace<QuadTextureComponent>(directional_light, storage->light_texture);
#endif

    SPDLOG_DEBUG("Built directional light entity {}", directional_light);
}

void GameLayer::build_origin() {
    entt::entity origin = registry.create();

    registry.emplace<OriginComponent>(origin);

    SPDLOG_DEBUG("Built origin entity {}", origin);
}

void GameLayer::build_node(int index, const glm::vec3& position) {
    entt::entity node = registry.create();
    nodes[index] = node;

    Rc<Buffer> vertices = Buffer::create(assets->node_mesh->vertices.data(),
                                         assets->node_mesh->vertices.size() * sizeof(glm::vec3));

    Rc<Buffer> ids = create_ids_buffer(assets->node_mesh->vertices.size(), node);

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    BufferLayout layout2;
    layout2.add(1, BufferLayout::Type::Int, 1);

    Rc<Buffer> index_buffer = Buffer::create_index(assets->node_mesh->indices.data(),
                                                   assets->node_mesh->indices.size() * sizeof(unsigned int));

    Rc<VertexArray> vertex_array = VertexArray::create();
    index_buffer->bind();
    vertex_array->add_buffer(vertices, layout);
    vertex_array->add_buffer(ids, layout2);
    vertex_array->hold_index_buffer(index_buffer);

    VertexArray::unbind();

    auto& transform = registry.emplace<TransformComponent>(node);
    transform.position = position;
    transform.scale = 20.0f;

    registry.emplace<MeshComponent>(node, vertex_array, assets->node_mesh->indices.size());

    registry.emplace<NodeComponent>(node, index);

    SPDLOG_DEBUG("Built node entity {}", node);
}

void GameLayer::build_turn_indicator() {
    entt::entity turn_indicator = registry.create();

    if (!storage->white_indicator_texture) {
        storage->white_indicator_texture = Texture::create(assets->white_indicator_data, false);
        storage->black_indicator_texture = Texture::create(assets->black_indicator_data, false);
    }

    auto& transform = registry.emplace<TransformComponent>(turn_indicator);
    transform.position = glm::vec3(application->data.width - 90, application->data.height - 115, 0.0f);

    registry.emplace<TurnIndicatorComponent>(turn_indicator);

    SPDLOG_DEBUG("Built turn indicator entity {}", turn_indicator);
}

void GameLayer::rebuild_board_after_load() {
    Rc<VertexArray> vertex_array = create_entity_vertex_array(assets->board_mesh, board);

    registry.emplace<MeshComponent>(board, vertex_array, assets->board_mesh->indices.size());
    registry.emplace<MaterialComponent>(board, glm::vec3(0.25f), 8.0f);
    registry.emplace<ShadowComponent>(board);
}

void GameLayer::rebuild_camera_after_load() {
    registry.emplace<CameraMoveComponent>(camera);
}

void GameLayer::rebuild_piece_after_load(entt::entity piece, Rc<model::Mesh<FullVertex>> mesh,
                                         Rc<Texture> diffuse_texture) {
    Rc<VertexArray> vertex_array = create_entity_vertex_array(mesh, piece);

    registry.emplace<MeshComponent>(piece, vertex_array, mesh->indices.size());
    registry.emplace<MaterialComponent>(piece, glm::vec3(0.25f), 8.0f);
    registry.emplace<PieceTextureComponent>(piece, diffuse_texture);
    registry.emplace<OutlineComponent>(piece, storage->outline_shader, glm::vec3(1.0f, 0.0f, 0.0f));
    registry.emplace<ShadowComponent>(piece);

    registry.emplace<MoveComponent>(piece);
}

void GameLayer::rebuild_node_after_load(entt::entity node) {
    Rc<Buffer> vertices = Buffer::create(assets->node_mesh->vertices.data(),
                                         assets->node_mesh->vertices.size() * sizeof(glm::vec3));

    Rc<Buffer> ids = create_ids_buffer(assets->node_mesh->vertices.size(), node);

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    BufferLayout layout2;
    layout2.add(1, BufferLayout::Type::Int, 1);

    Rc<Buffer> index_buffer = Buffer::create_index(assets->node_mesh->indices.data(),
                                                   assets->node_mesh->indices.size() * sizeof(unsigned int));

    Rc<VertexArray> vertex_array = VertexArray::create();
    index_buffer->bind();
    vertex_array->add_buffer(vertices, layout);
    vertex_array->add_buffer(ids, layout2);
    vertex_array->hold_index_buffer(index_buffer);

    VertexArray::unbind();

    registry.emplace<MeshComponent>(node, vertex_array, assets->node_mesh->indices.size());
}
