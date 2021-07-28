#include <string>
#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <cassert>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "application/application.h"
#include "application/window.h"
#include "application/events.h"
#include "application/input.h"
#include "opengl/debug_opengl.h"
#include "opengl/renderer/renderer.h"
#include "opengl/renderer/texture.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/vertex_buffer.h"
#include "ecs/components.h"
#include "ecs/systems.h"
#include "ecs/game.h"
#include "other/model.h"
#include "other/logging.h"

#define BIND(function) std::bind(&function, this, std::placeholders::_1)

Application::Application(int width, int height) {
    window = std::make_unique<Window>(width, height, &data);

    data.width = width;
    data.height = height;
    data.event_function = BIND(Application::on_event);

    start();
    imgui_start();
}

Application::~Application() {
    imgui_end();
    end();
}

void Application::run() {
    float dt;

    while (running) {
        dt = update_frame_counter();
        update(dt);
        draw();
        imgui_update(dt);

        window->update();
    }
}

void Application::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher = Dispatcher(event);
    dispatcher.dispatch<WindowClosedEvent>(WindowClosed, BIND(Application::on_window_closed));
    dispatcher.dispatch<WindowResizedEvent>(WindowResized, BIND(Application::on_window_resized));
    dispatcher.dispatch<MouseScrolledEvent>(MouseScrolled, BIND(Application::on_mouse_scrolled));
    dispatcher.dispatch<MouseMovedEvent>(MouseMoved, BIND(Application::on_mouse_moved));
    dispatcher.dispatch<MouseButtonPressedEvent>(MouseButtonPressed, BIND(Application::on_mouse_button_pressed));
    dispatcher.dispatch<MouseButtonReleasedEvent>(MouseButtonReleased, BIND(Application::on_mouse_button_released));
}

void Application::update(float dt) {
    systems::camera(registry, mouse_wheel, dx, dy, dt);
    systems::lighting_move(registry, dt);
    systems::move_piece(registry, dt);
    // systems::game_update(registry, board, hovered_entity);

    mouse_wheel = 0.0f;
    dx = 0.0f;
    dy = 0.0f;
}

void Application::draw() {
    storage->framebuffer->bind();

    renderer::set_clear_color(0.5f, 0.0f, 0.5f);
    renderer::clear(renderer::Color | renderer::Depth | renderer::Stencil);
    renderer::set_stencil_mask_zero();

    storage->framebuffer->clear_red_integer_attachment(1, -1);

    systems::cube_map_render(registry, camera);
    systems::lighting(registry, camera);
    systems::board_render(registry, camera);
    systems::piece_render(registry, camera, hovered_entity);
    systems::node_render(registry, camera, hovered_entity);
    systems::origin_render(registry, camera);
    systems::lighting_render(registry, camera);

    hovered_entity = (entt::entity) storage->framebuffer->read_pixel(
                          1,
                          input::get_mouse_x(),
                          data.height - input::get_mouse_y()
                     );

    Framebuffer::bind_default();

    renderer::clear(renderer::Color);
    storage->quad_shader->bind();
    storage->quad_shader->set_uniform_int("u_screen_texture", 0);
    storage->quad_vertex_array->bind();
    renderer::bind_texture(storage->framebuffer->get_color_attachment(0));
    renderer::disable_depth();
    renderer::draw_quad();
    renderer::enable_depth();
}

void Application::start() {
    logging::init();
    logging::log_opengl_info(logging::LogTarget::Console);
    debug_opengl::maybe_init_debugging();
    input::init(window->get_handle());
    storage = renderer::init();

    auto [version_major, version_minor] = debug_opengl::get_version();
    assert(version_major == 4 && version_minor >= 3);

    using namespace model;
    std::tuple<Mesh, Mesh, Mesh, Mesh> meshes = load_model("data/models/board.obj");

    std::shared_ptr<Texture> white_piece_diffuse = Texture::create("data/textures/white_piece.png");
    std::shared_ptr<Texture> black_piece_diffuse = Texture::create("data/textures/black_piece.png");

    build_camera();
    build_skybox();

    for (int i = 0; i < 9; i++) {
        build_piece(i, Piece::White, std::get<1>(meshes), white_piece_diffuse,
                    glm::vec3(4.0f, 0.3f, -2.0f + i * 0.5f));
    }
    
    for (int i = 9; i < 18; i++) {
        build_piece(i, Piece::Black, std::get<2>(meshes), black_piece_diffuse,
                    glm::vec3(-4.0f, 0.3f, -2.0f + (i - 9) * 0.5f));
    }

    for (int i = 0; i < 24; i++) {
        build_node(i, std::get<3>(meshes), NODE_POSITIONS[i]);
    }

    build_board(std::get<0>(meshes));

    build_directional_light();
    build_origin();

    SPDLOG_INFO("Finished initializing program");
}

void Application::end() {
    SPDLOG_INFO("Closing program");
    renderer::terminate();
}

float Application::update_frame_counter() {
    constexpr double MAX_DT = 1.0 / 20.0;

    static double previous_seconds = glfwGetTime();
    static int frame_count = 0;
    static double total_time = 0.0;

    double current_seconds = glfwGetTime();
    double elapsed_seconds = current_seconds - previous_seconds;
    previous_seconds = current_seconds;

    total_time += elapsed_seconds;

    if (total_time > 0.25) {
        fps = (double) frame_count / total_time;
        frame_count = 0;
        total_time = 0.0;
    }
    frame_count++;

    double delta_time = std::min(elapsed_seconds, MAX_DT);

    return (float) delta_time;
}

void Application::imgui_start() {
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    ImGui_ImplOpenGL3_Init("#version 430");
    ImGui_ImplGlfw_InitForOpenGL(window->get_handle(), false);
}

void Application::imgui_update(float dt) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(data.width, data.height);
    io.DeltaTime = dt;

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Game")) {
            ImGui::MenuItem("New", nullptr, false);
            if (ImGui::MenuItem("Exit", nullptr, false)) {
                running = false;
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options")) {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("About", nullptr, false);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Debug");
    ImGui::Text("FPS: %f", fps);
    ImGui::Text("Frame time (ms): %f", dt * 1000);
    if (ImGui::Button("VSync")) {
        static bool on = true;
        if (on) {
            window->set_vsync(0);
            on = false;
        } else {
            window->set_vsync(1);
            on = true;
        }
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::imgui_end() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool Application::on_window_closed(events::WindowClosedEvent& event) {
    running = false;
    return true;
}

bool Application::on_window_resized(events::WindowResizedEvent& event) {
    renderer::set_viewport(event.width, event.height);
    storage->framebuffer->resize(event.width, event.height);  // TODO should glViewport be called?

    return true;
}

bool Application::on_mouse_scrolled(events::MouseScrolledEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel = event.scroll;

    mouse_wheel = event.scroll;
    return true;
}

bool Application::on_mouse_moved(events::MouseMovedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(event.mouse_x, event.mouse_y);

    dx = last_mouse_x - event.mouse_x;
    dy = last_mouse_y - event.mouse_y;
    last_mouse_x = event.mouse_x;
    last_mouse_y = event.mouse_y;

    return true;
}

bool Application::on_mouse_button_pressed(events::MouseButtonPressedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[event.button] = true;

    systems::press(registry, board, hovered_entity);

    return false;
}

bool Application::on_mouse_button_released(events::MouseButtonReleasedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[event.button] = false;

    auto& state = registry.get<GameStateComponent>(board);

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

std::shared_ptr<VertexBuffer> Application::create_ids_buffer(unsigned int vertices_size,
                                                             entt::entity entity) {
    std::vector<int> array;
    array.resize(vertices_size);
    for (unsigned int i = 0; i < array.size(); i++) {
        array[i] = (int) entt::to_integral(entity);
    }
    std::shared_ptr<VertexBuffer> buffer =
        VertexBuffer::create_with_data(array.data(), array.size() * sizeof(int));

    return buffer;
}

std::shared_ptr<VertexArray> Application::create_entity_vertex_buffer(model::Mesh mesh,
                                                                      entt::entity entity) {
    std::shared_ptr<VertexBuffer> vertices =
        VertexBuffer::create_with_data(mesh.vertices.data(),
                                       mesh.vertices.size() * sizeof(model::Vertex));

    std::shared_ptr<VertexBuffer> ids = create_ids_buffer(mesh.vertices.size(), entity);

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    layout.add(1, BufferLayout::Type::Float, 2);
    layout.add(2, BufferLayout::Type::Float, 3);

    BufferLayout layout2;
    layout2.add(3, BufferLayout::Type::Int, 1);

    std::shared_ptr<VertexBuffer> index_buffer =
        VertexBuffer::create_index(mesh.indices.data(),
                                   mesh.indices.size() * sizeof(unsigned int));

    std::shared_ptr<VertexArray> vertex_array = VertexArray::create();
    index_buffer->bind();
    vertex_array->add_buffer(vertices, layout);
    vertex_array->add_buffer(ids, layout2);
    vertex_array->hold_index_buffer(index_buffer);

    VertexArray::unbind();

    return vertex_array;
}

void Application::build_board(const model::Mesh& mesh) {
    board = registry.create();

    std::shared_ptr<Texture> diffuse_texture = Texture::create("data/textures/board.png");

    std::shared_ptr<VertexArray> vertex_array = create_entity_vertex_buffer(mesh, board);

    auto& transform = registry.emplace<TransformComponent>(board);
    transform.scale = 20.0f;
    
    registry.emplace<MeshComponent>(board, vertex_array, mesh.indices.size());
    registry.emplace<MaterialComponent>(board, storage->basic_shader, glm::vec3(0.25f), 8.0f);
    registry.emplace<TextureComponent>(board, diffuse_texture);

    registry.emplace<GameStateComponent>(board, nodes);

    SPDLOG_DEBUG("Built board entity {}", board);
}

void Application::build_camera() {
    camera = registry.create();
    auto& transform = registry.emplace<TransformComponent>(camera);
    transform.rotation = glm::vec3(40.0f, 0.0f, 0.0f);

    registry.emplace<CameraComponent>(camera,
            glm::perspective(glm::radians(45.0f), 1600.0f / 900.0f, 0.08f, 100.0f),
            glm::vec3(0.0f), 8.0f);

    SPDLOG_DEBUG("Built camera entity {}", camera);
}

void Application::build_skybox() {
    std::shared_ptr<Shader> shader = Shader::create("data/shaders/cubemap.vert",
                                                    "data/shaders/cubemap.frag");

    const char* images[6] = {
        "data/textures/skybox/right.jpg",
        "data/textures/skybox/left.jpg",
        "data/textures/skybox/top.jpg",
        "data/textures/skybox/bottom.jpg",
        "data/textures/skybox/front.jpg",
        "data/textures/skybox/back.jpg"
    };
    std::shared_ptr<Texture3D> texture = Texture3D::create(images);

    std::shared_ptr<VertexBuffer> positions =
        VertexBuffer::create_with_data(cube_map_points, 108 * sizeof(float));

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);

    std::shared_ptr<VertexArray> vertex_array = VertexArray::create();
    vertex_array->add_buffer(positions, layout);
    VertexArray::unbind();

    skybox = registry.create();
    registry.emplace<SkyboxMeshComponent>(skybox, vertex_array);
    registry.emplace<SkyboxMaterialComponent>(skybox, shader);
    registry.emplace<SkyboxTextureComponent>(skybox, texture);

    SPDLOG_DEBUG("Built skybox entity {}", skybox);
}

void Application::build_piece(int index, Piece type, const model::Mesh& mesh,
                              std::shared_ptr<Texture> diffuse_texture,
                              const glm::vec3& position) {
    pieces[index] = registry.create();
    entt::entity piece = pieces[index];

    std::shared_ptr<VertexArray> vertex_array = create_entity_vertex_buffer(mesh, piece);
    
    auto& transform = registry.emplace<TransformComponent>(piece);
    transform.position = position;
    transform.scale = 20.0f;

    registry.emplace<MeshComponent>(piece, vertex_array, mesh.indices.size());
    registry.emplace<MaterialComponent>(piece, storage->basic_shader, glm::vec3(0.25f), 8.0f);
    registry.emplace<TextureComponent>(piece, diffuse_texture);
    registry.emplace<OutlineComponent>(piece, storage->outline_shader,
                                       glm::vec3(1.0f, 0.0f, 0.0f));

    registry.emplace<PieceComponent>(piece, type);
    registry.emplace<MoveComponent>(piece);

    SPDLOG_DEBUG("Built piece entity {}", piece);
}

void Application::build_directional_light() {
    directional_light = registry.create();
    auto& transform = registry.emplace<TransformComponent>(directional_light);
    transform.position = glm::vec3(10.0f, 15.0f, -15.0f);

    registry.emplace<LightComponent>(directional_light, glm::vec3(0.15f), glm::vec3(0.8f),
                                     glm::vec3(1.0f));
    registry.emplace<ShaderComponent>(directional_light, storage->basic_shader);
    registry.emplace<LightMeshComponent>(directional_light, storage->light_shader);

    SPDLOG_DEBUG("Built directional light entity {}", directional_light);
}

void Application::build_origin() {
    origin = registry.create();
    registry.emplace<OriginComponent>(origin, storage->origin_shader);

    SPDLOG_DEBUG("Built origin entity {}", origin);   
}

void Application::build_node(int index, const model::Mesh& mesh, const glm::vec3& position) {
    nodes[index] = registry.create();
    entt::entity node = nodes[index];

    std::vector<glm::vec3> data;
    for (const model::Vertex& vertex : mesh.vertices) {
        data.push_back(vertex.position);
    }
    std::shared_ptr<VertexBuffer> vertices =
        VertexBuffer::create_with_data(data.data(), data.size() * sizeof(glm::vec3));

    std::shared_ptr<VertexBuffer> ids = create_ids_buffer(mesh.vertices.size(), node);

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    BufferLayout layout2;
    layout2.add(1, BufferLayout::Type::Int, 1);

    std::shared_ptr<VertexBuffer> index_buffer =
        VertexBuffer::create_index(mesh.indices.data(),
                                   mesh.indices.size() * sizeof(unsigned int));

    std::shared_ptr<VertexArray> vertex_array = VertexArray::create();
    index_buffer->bind();
    vertex_array->add_buffer(vertices, layout);
    vertex_array->add_buffer(ids, layout2);
    vertex_array->hold_index_buffer(index_buffer);

    VertexArray::unbind();

    auto& transform = registry.emplace<TransformComponent>(node);
    transform.position = position;
    transform.scale = 20.0f;

    registry.emplace<MeshComponent>(node, vertex_array, mesh.indices.size());
    registry.emplace<NodeMaterialComponent>(node, storage->node_shader);

    registry.emplace<NodeComponent>(node, index);

    SPDLOG_DEBUG("Built node entity {}", node);
}
