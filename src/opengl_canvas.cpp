#include <cassert>
#include <chrono>
#include <memory>
#include <vector>
#include <string.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "opengl_canvas.h"
#include "other/logging.h"
#include "opengl/debug_opengl.h"
#include "opengl/renderer/renderer.h"
#include "opengl/renderer/texture.h"
#include "ecs/components.h"
#include "ecs/systems.h"
#include "other/model.h"

static void update_game(void* data);

OpenGLCanvas::OpenGLCanvas(int x, int y, int w, int h, const char* t)
        : Fl_Gl_Window(x, y, w, h, t) {
    mode(FL_OPENGL3 | FL_DOUBLE | FL_DEPTH /* | FL_RGB8 | FL_ALPHA*/);
    Fl::add_idle(update_game, this);

    logging::init();
}

void OpenGLCanvas::draw() {
    if (!context_valid()) {
        make_current();
        gladLoadGL();
        start_program();
    }

    if (!valid()) {
        resize();
    }

    framebuffer->bind();

    renderer::set_clear_color(0.5f, 0.0f, 0.5f);
    renderer::clear(renderer::Color | renderer::Depth | renderer::Stencil);
    renderer::set_stencil_mask_zero();

    framebuffer->clear_red_integer_attachment(1, -1);

    cube_map_render_system(registry, camera);
    render_system(registry, camera);
    pieces_render_system(registry, camera, hovered_entity);

    if (mouse_x == 0 && mouse_y == 0)
        hovered_entity = entt::null;
    else
        hovered_entity = (entt::entity) framebuffer->read_pixel(1, mouse_x, height - mouse_y);

    Framebuffer::bind_default();

    renderer::clear(renderer::Color);
    storage->quad_shader->bind();
    storage->quad_shader->set_uniform_int("u_screen_texture", 0);
    storage->quad_vertex_array->bind();
    renderer::bind_texture(framebuffer->get_color_attachment(0));
    renderer::disable_depth();
    renderer::draw_quad();
    renderer::enable_depth();
}

int OpenGLCanvas::handle(int event) {
    switch(event) {
        case FL_PUSH:
            SPDLOG_DEBUG("Mouse down [ {}, {} ]", Fl::event_x(), Fl::event_y());
            switch (Fl::event_button()) {
                case FL_LEFT_MOUSE:
                    left_mouse_pressed = true;
                    break;
                case FL_RIGHT_MOUSE:
                    right_mouse_pressed = true;
                    break;
            }
            return 1;
        case FL_DRAG: {
            int x = Fl::event_x();
            int y = Fl::event_y();

            mouse_dt_x = (x - mouse_x) * mouse_sensitivity;
            mouse_dt_y = (mouse_y - y) * mouse_sensitivity;

            mouse_x = x;
            mouse_y = y;
            return 1;
        }
        case FL_RELEASE:
            switch (Fl::event_button()) {
                case FL_LEFT_MOUSE:
                    left_mouse_pressed = false;
                    break;
                case FL_RIGHT_MOUSE:
                    right_mouse_pressed = false;
                    break;
            }
            return 1;
        case FL_ENTER:
            return 1;
        case FL_MOVE:
            mouse_x = Fl::event_x();
            mouse_y = Fl::event_y();
            return 1;
        case FL_LEAVE:
            return 1;
        case FL_FOCUS:
        case FL_UNFOCUS:
            // ... Return 1 if you want keyboard events, 0 otherwise
            return 1;
        case FL_KEYBOARD: {
            // ... keypress, key is in Fl::event_key(), ascii in Fl::event_text()
            // ... Return 1 if you understand/use the keyboard event, 0 otherwise...
            int key = Fl::event_key();
            char keycode = (char) key;
            // Do something based on keycode
            return 1;
        }
        case FL_MOUSEWHEEL: {
            mouse_wheel = Fl::event_dy() * scroll_sensitivity;
            return 1;
        }
        default:
            // pass other events to the base class...
            return Fl_Gl_Window::handle(event);
    }
}

void OpenGLCanvas::start_program() {
    logging::log_opengl_info(logging::LogTarget::Console);
    debug_opengl::maybe_init_debugging();
    storage = renderer::init();

    auto [version_major, version_minor] = debug_opengl::get_version();
    assert(version_major == 4 && version_minor >= 3);

    basic_shader = Shader::create("data/shaders/basic.vert", "data/shaders/basic.frag");

    Specification specification;
    specification.width = 1024;
    specification.height = 576;
    specification.attachments = { TextureFormat::RGB8, TextureFormat::RedInteger,
                                  TextureFormat::Depth24Stencil8 };
    framebuffer = Framebuffer::create(specification);

    build_board();
    build_camera();
    build_skybox();
    // build_box();

    build_piece(glm::vec3(0.0f, 1.4f, 0.0f));
    build_piece(glm::vec3(2.0f, 1.4f, 0.0f));
    build_piece(glm::vec3(2.0f, 1.4f, 1.3f));
    build_piece(glm::vec3(-1.2f, 1.4f, -1.1f));

    SPDLOG_DEBUG("Finished initializing program");
}

void OpenGLCanvas::resize() {
    width = w();
    height = h();
    framebuffer->resize(width, height);
    renderer::set_viewport(width, height);
}

void OpenGLCanvas::reset() {
    SPDLOG_DEBUG("Resetting");
    start_program();
}

void OpenGLCanvas::end_program() {
    // Do ending stuff
}

std::shared_ptr<VertexBuffer> OpenGLCanvas::create_ids_buffer(unsigned int vertices_size,
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

std::shared_ptr<VertexArray> OpenGLCanvas::create_entity_vertex_buffer(model::Mesh mesh,
                                                                       entt::entity entity) {
    std::shared_ptr<VertexBuffer> vertices =
        VertexBuffer::create_with_data(mesh.vertices.data(),
                                       mesh.vertices.size() * sizeof(model::Vertex));

    std::shared_ptr<VertexBuffer> ids = create_ids_buffer(mesh.vertices.size(), entity);

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    layout.add(1, BufferLayout::Type::Float, 2);

    BufferLayout layout2;
    layout2.add(2, BufferLayout::Type::Int, 1);

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

static float update_fps_counter() {
    using namespace std::chrono;
    using clock = high_resolution_clock;

    static clock::time_point previous_seconds = clock::now();
    static int frame_count = 0;

    clock::time_point current_seconds = clock::now();
    duration<double> elapsed_seconds =
        duration_cast<duration<double>>(current_seconds - previous_seconds);

    if (elapsed_seconds.count() > 0.25) {
        previous_seconds = current_seconds;
        double fps = (double) frame_count / elapsed_seconds.count();
        SPDLOG_DEBUG("{}", fps);
        frame_count = 0;
    }
    frame_count++;

    return (float) elapsed_seconds.count();
}

static void update_game(void* data) {
    OpenGLCanvas* canvas = (OpenGLCanvas*) data;

    static float dt;
    dt = update_fps_counter();

    camera_system(canvas->registry, { canvas->mouse_x, canvas->mouse_y,
                                      canvas->mouse_wheel, canvas->left_mouse_pressed,
                                      canvas->right_mouse_pressed, canvas->mouse_dt_x,
                                      canvas->mouse_dt_y });
    
    canvas->mouse_wheel = 0.0f;
    canvas->mouse_dt_x = 0.0f;
    canvas->mouse_dt_y = 0.0f;

    canvas->redraw();
}

void OpenGLCanvas::build_board() {
    board = registry.create();

    model::Mesh mesh = model::load_model("data/models/board.obj");

    std::shared_ptr<Texture> diffuse_texture =
        Texture::create("data/textures/board_texture.png", Texture::Type::Diffuse);

    std::shared_ptr<VertexArray> vertex_array = create_entity_vertex_buffer(mesh, board);

    registry.emplace<TransformComponent>(board);
    registry.emplace<MeshComponent>(board, vertex_array, mesh.indices.size());
    registry.emplace<MaterialComponent>(board, basic_shader,
                                        std::unordered_map<std::string, int>());
    registry.emplace<TextureComponent>(board, diffuse_texture);

    SPDLOG_DEBUG("Built board entity {}", board);
}

void OpenGLCanvas::build_camera() {
    camera = registry.create();
    registry.emplace<TransformComponent>(camera, glm::vec3(25.0f, 0.0f, 0.0f));
    registry.emplace<CameraComponent>(camera,
            glm::perspective(glm::radians(45.0f), 1600.0f / 900.0f, 0.1f, 1000.0f),
            glm::vec3(0.0f), 12.0f);

    SPDLOG_DEBUG("Built camera entity {}", camera);
}

void OpenGLCanvas::build_skybox() {
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
    registry.emplace<MaterialComponent>(skybox, shader,
                                        std::unordered_map<std::string, int>());
    registry.emplace<SkyboxTextureComponent>(skybox, texture);

    SPDLOG_DEBUG("Built skybox entity {}", skybox);
}

void OpenGLCanvas::build_box() {
    box = registry.create();

    model::Mesh mesh = model::load_model("data/models/box.obj");

    std::shared_ptr<Texture> diffuse_texture =
        Texture::create("data/textures/box.png", Texture::Type::Diffuse);

    std::shared_ptr<VertexArray> vertex_array = create_entity_vertex_buffer(mesh, box);

    registry.emplace<TransformComponent>(box);
    registry.emplace<MeshComponent>(box, vertex_array, mesh.indices.size());
    registry.emplace<MaterialComponent>(box, basic_shader,
                                        std::unordered_map<std::string, int>());
    registry.emplace<TextureComponent>(box, diffuse_texture);

    SPDLOG_DEBUG("Built box entity {}", box);
}

void OpenGLCanvas::build_piece(const glm::vec3& position) {
    piece = registry.create();

    model::Mesh mesh = model::load_model("data/models/piece.obj");

    std::shared_ptr<Texture> diffuse_texture =
        Texture::create("data/textures/black_piece_texture.png", Texture::Type::Diffuse);

    std::shared_ptr<VertexArray> vertex_array = create_entity_vertex_buffer(mesh, piece);
    
    registry.emplace<TransformComponent>(piece, position, glm::vec3(0.0f), 0.3f);
    registry.emplace<MeshComponent>(piece, vertex_array, mesh.indices.size());
    registry.emplace<MaterialComponent>(piece, basic_shader,
                                        std::unordered_map<std::string, int>());
    registry.emplace<TextureComponent>(piece, diffuse_texture);
    registry.emplace<OutlineComponent>(piece, storage->outline_shader,
                                       glm::vec3(1.0f, 0.0f, 0.0f));

    SPDLOG_DEBUG("Built piece entity {}", piece);
}
