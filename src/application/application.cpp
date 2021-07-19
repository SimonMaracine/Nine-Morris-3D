#include <string>
#include <functional>
#include <memory>
#include <cstdlib>
#include <utility>
#include <vector>
#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "application/application.h"
#include "application/events.h"
#include "application/input.h"
#include "opengl/debug_opengl.h"
#include "opengl/renderer/renderer.h"
#include "opengl/renderer/texture.h"
#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/vertex_buffer.h"
#include "ecs/components.h"
#include "ecs/systems.h"
#include "other/model.h"
#include "other/logging.h"

#define BIND(function) std::bind(&function, this, std::placeholders::_1)

Application::Application(int width, int height) {
    if (!glfwInit()) {
        spdlog::critical("Could not initialize GLFW");
        std::exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, "Nine Morris 3D", nullptr, nullptr);
    if (!window) {
        spdlog::critical("Could not create window");
        std::exit(1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        spdlog::critical("Could not initialize GLAD");
        std::exit(1);
    }

    glfwSwapInterval(1);
    glfwSetWindowUserPointer(window, &data);

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        ApplicationData* data = (ApplicationData*) glfwGetWindowUserPointer(window);

        events::WindowClosedEvent event = events::WindowClosedEvent();
        data->event_function(event);
    });

    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        ApplicationData* data = (ApplicationData*) glfwGetWindowUserPointer(window);

        events::WindowResizedEvent event = events::WindowResizedEvent(width, height);
        data->width = width;
        data->height = height;
        data->event_function(event);
    });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        ApplicationData* data = (ApplicationData*) glfwGetWindowUserPointer(window);

        switch (action) {
            case GLFW_PRESS: {
                events::KeyPressedEvent event = events::KeyPressedEvent(key);
                data->event_function(event);
                break;
            }
            case GLFW_RELEASE: {
                events::KeyReleasedEvent event = events::KeyReleasedEvent(key);
                data->event_function(event);
                break;
            }
            case GLFW_REPEAT: {
                events::KeyPressedEvent event = events::KeyPressedEvent(key);
                data->event_function(event);
                break;
            }
        }
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        ApplicationData* data = (ApplicationData*) glfwGetWindowUserPointer(window);

        if (action == GLFW_PRESS) {
            events::MouseButtonPressedEvent event = events::MouseButtonPressedEvent(button);
            data->event_function(event);
        } else if (action == GLFW_RELEASE) {
            events::MouseButtonReleasedEvent event = events::MouseButtonReleasedEvent(button);
            data->event_function(event);
        }
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        ApplicationData* data = (ApplicationData*) glfwGetWindowUserPointer(window);

        events::MouseScrolledEvent event = events::MouseScrolledEvent((float) yoffset);
        data->event_function(event);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        ApplicationData* data = (ApplicationData*) glfwGetWindowUserPointer(window);

        events::MouseMovedEvent event = events::MouseMovedEvent((float) xpos, (float) ypos);
        data->event_function(event);
    });

    data.width = width;
    data.height = height;
    data.event_function = BIND(Application::on_event);

    std::atexit(glfwTerminate);
}

Application::~Application() {
    glfwDestroyWindow(window);
}

void Application::run() {
    float dt;
    start();

    while (running) {
        dt = update_fps_counter();
        update(dt);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    end();
}

void Application::on_event(events::Event& event) {
    events::Dispatcher dispatcher = events::Dispatcher(event);
    dispatcher.dispatch<events::WindowClosedEvent>(events::WindowClosed,
                                                   BIND(Application::on_window_closed));
    dispatcher.dispatch<events::WindowResizedEvent>(events::WindowResized,
                                                    BIND(Application::on_window_resized));
    dispatcher.dispatch<events::MouseScrolledEvent>(events::MouseScrolled,
                                                    BIND(Application::on_mouse_scrolled));
    dispatcher.dispatch<events::MouseMovedEvent>(events::MouseMoved,
                                                    BIND(Application::on_mouse_moved));
}

void Application::update(float dt) {
    camera_system(registry, mouse_wheel, dx, dy, dt);
    lighting_move_system(registry, dt);

    mouse_wheel = 0.0f;
    dx = 0.0f;
    dy = 0.0f;
    
    draw();
}

void Application::draw() {
    storage->framebuffer->bind();

    renderer::set_clear_color(0.5f, 0.0f, 0.5f);
    renderer::clear(renderer::Color | renderer::Depth | renderer::Stencil);
    renderer::set_stencil_mask_zero();

    storage->framebuffer->clear_red_integer_attachment(1, -1);

    cube_map_render_system(registry, camera);
    lighting_system(registry, camera);
    render_system(registry, camera);
    with_outline_render_system(registry, camera, hovered_entity);
    origin_render_system(registry, camera);
    lighting_render_system(registry, camera);

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
    input::init(window);
    storage = renderer::init();

    auto [version_major, version_minor] = debug_opengl::get_version();
    assert(version_major == 4 && version_minor >= 3);

    std::tuple<model::Mesh, model::Mesh> meshes = model::load_models("data/models/board.obj");

    std::shared_ptr<Texture> white_piece_diffuse = Texture::create("data/textures/white_piece.png");

    build_board(std::get<0>(meshes));
    build_camera();
    build_skybox();

    build_piece(std::get<1>(meshes), white_piece_diffuse, glm::vec3(0.0f, 0.135f, 0.0f));
    build_piece(std::get<1>(meshes), white_piece_diffuse, glm::vec3(-1.0f, 0.135f, -1.5f));
    build_piece(std::get<1>(meshes), white_piece_diffuse, glm::vec3(1.0f, 0.135f, 2.3f));
    build_piece(std::get<1>(meshes), white_piece_diffuse, glm::vec3(-1.2f, 0.135f, 2.1f));

    build_directional_light();
    build_origin();

    SPDLOG_INFO("Finished initializing program");
}

void Application::end() {
    SPDLOG_INFO("Closing program");
    renderer::terminate();
}

float Application::update_fps_counter() {
    static double fps = 0;
    static double previous_seconds = glfwGetTime();
    static int frame_count = 0;

    double current_seconds = glfwGetTime();
    double elapsed_seconds = current_seconds - previous_seconds;

    if (elapsed_seconds > 0.25) {
        previous_seconds = current_seconds;
        fps = (double) frame_count / elapsed_seconds;
        spdlog::debug("FPS: {}", fps);
        frame_count = 0;
    }
    frame_count++;

    // SPDLOG_DEBUG("Delta: {}", elapsed_seconds.count() * 1000.0f);

    return (float) elapsed_seconds;
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

bool Application::on_window_closed(events::WindowClosedEvent& event) {
    running = false;
    return true;
}

bool Application::on_window_resized(events::WindowResizedEvent& event) {
    renderer::set_viewport(event.width, event.height);
    storage->framebuffer->resize(event.width, event.height);
    return true;
}

bool Application::on_mouse_scrolled(events::MouseScrolledEvent& event) {
    mouse_wheel = event.scroll;
    return true;
}

bool Application::on_mouse_moved(events::MouseMovedEvent& event) {
    dx = last_mouse_x - event.mouse_x;
    dy = last_mouse_y - event.mouse_y;
    last_mouse_x = event.mouse_x;
    last_mouse_y = event.mouse_y;
    return true;
}

void Application::build_board(const model::Mesh& mesh) {
    board = registry.create();

    std::shared_ptr<Texture> diffuse_texture = Texture::create("data/textures/board.png");

    std::shared_ptr<VertexArray> vertex_array = create_entity_vertex_buffer(mesh, board);

    registry.emplace<TransformComponent>(board, 20.0f);
    registry.emplace<MeshComponent>(board, vertex_array, mesh.indices.size());
    registry.emplace<MaterialComponent>(board, storage->basic_shader, glm::vec3(0.25f), 8.0f);
    registry.emplace<TextureComponent>(board, diffuse_texture);

    SPDLOG_DEBUG("Built board entity {}", board);
}

void Application::build_camera() {
    camera = registry.create();
    registry.emplace<TransformComponent>(camera, glm::vec3(25.0f, 0.0f, 0.0f));
    registry.emplace<CameraComponent>(camera,
            glm::perspective(glm::radians(45.0f), 1600.0f / 900.0f, 0.08f, 1000.0f),
            glm::vec3(0.0f), 12.0f);

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

void Application::build_piece(const model::Mesh& mesh, std::shared_ptr<Texture> diffuse_texture,
                               const glm::vec3& position) {
    piece = registry.create();

    std::shared_ptr<VertexArray> vertex_array = create_entity_vertex_buffer(mesh, piece);
    
    registry.emplace<TransformComponent>(piece, position, glm::vec3(0.0f), 20.0f);
    registry.emplace<MeshComponent>(piece, vertex_array, mesh.indices.size());
    registry.emplace<MaterialComponent>(piece, storage->basic_shader, glm::vec3(0.25f), 8.0f);
    registry.emplace<TextureComponent>(piece, diffuse_texture);
    registry.emplace<OutlineComponent>(piece, storage->outline_shader, glm::vec3(1.0f, 0.0f, 0.0f));

    SPDLOG_DEBUG("Built piece entity {}", piece);
}

void Application::build_directional_light() {
    directional_light = registry.create();
    registry.emplace<TransformComponent>(directional_light, glm::vec3(10.0f, 15.0f, -15.0f),
                                         glm::vec3(0.0f));
    registry.emplace<LightComponent>(directional_light, glm::vec3(0.15f), glm::vec3(0.8f),
                                     glm::vec3(1.0f));
    registry.emplace<ShaderComponent>(directional_light, storage->basic_shader);
    registry.emplace<LightMeshComponent>(directional_light, storage->light_shader);

    SPDLOG_DEBUG("Built directional light entity {}", piece);
}

void Application::build_origin() {
    origin = registry.create();
    registry.emplace<OriginComponent>(origin, storage->origin_shader);

    SPDLOG_DEBUG("Built origin entity {}", origin);   
}
