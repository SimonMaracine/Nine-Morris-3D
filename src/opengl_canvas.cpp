#include <cassert>
#include <chrono>
#include <memory>
#include <vector>

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
    mode(FL_OPENGL3 | FL_DOUBLE | FL_DEPTH);
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

    renderer::set_clear_color(0.5f, 0.0f, 0.5f);
    renderer::clear();

    cube_map_render_system(registry, camera);
    render_system(registry, camera);
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
            mouse_wheel = Fl::event_dy();
            return 1;
        }
        default:
            // pass other events to the base class...
            return Fl_Gl_Window::handle(event);
    }
}

void OpenGLCanvas::start_program() {
    logging::log_opengl_info(true);
    debug_opengl::maybe_init_debugging();
    renderer::init();
    auto [version_major, version_minor] = debug_opengl::get_version();
    assert(version_major >= 4 && version_minor >= 3);

    basic_shader = Shader::create("data/shaders/basic.vert",
                                  "data/shaders/basic.frag");

    build_board();
    build_camera();
    build_skybox();
    // build_box();
    build_piece();

    SPDLOG_DEBUG("Finished initializing program");
}

void OpenGLCanvas::resize() {
    int width = w();
    int height = h();
    renderer::set_viewport(width, height);
}

void OpenGLCanvas::reset() {
    SPDLOG_DEBUG("Resetting");
    start_program();
}

void OpenGLCanvas::end_program() {
    // Do ending stuff
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
    model::Mesh mesh = model::load_model("data/models/board.obj");

    std::shared_ptr<Texture> diffuse_texture =
        Texture::create("data/textures/board_texture.png", Texture::Type::Diffuse);

    std::shared_ptr<VertexBuffer> vertices =
        VertexBuffer::create_with_data(mesh.vertices.data(),
                                       mesh.vertices.size() * sizeof(model::Vertex));

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    layout.add(1, BufferLayout::Type::Float, 2);

    std::shared_ptr<VertexBuffer> index_buffer =
        VertexBuffer::create_index(mesh.indices.data(),
                                   mesh.indices.size() * sizeof(unsigned int));

    std::shared_ptr<VertexArray> vertex_array = VertexArray::create();
    index_buffer->bind();
    vertex_array->add_buffer(vertices, layout);
    
    VertexArray::unbind();

    board = registry.create();
    registry.emplace<TransformComponent>(board);
    std::vector<std::shared_ptr<VertexBuffer>> buffers = { vertices };
    registry.emplace<MeshComponent>(board, vertex_array, buffers, index_buffer,
                                    mesh.indices.size());
    registry.emplace<MaterialComponent>(board, basic_shader,
                                        std::unordered_map<std::string, int>());
    registry.emplace<TextureComponent>(board, diffuse_texture);
}

void OpenGLCanvas::build_camera() {
    camera = registry.create();
    registry.emplace<TransformComponent>(camera, glm::vec3(25.0f, 0.0f, 0.0f));
    registry.emplace<CameraComponent>(camera,
            glm::perspective(glm::radians(45.0f), 1600.0f / 900.0f, 0.1f, 1000.0f),
            glm::vec3(0.0f), 12.0f);
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
    std::vector<std::shared_ptr<VertexBuffer>> buffers = { positions };
    registry.emplace<SkyboxMeshComponent>(skybox, vertex_array, buffers);
    registry.emplace<MaterialComponent>(skybox, shader,
                                        std::unordered_map<std::string, int>());
    registry.emplace<SkyboxTextureComponent>(skybox, texture);
}

void OpenGLCanvas::build_box() {
    model::Mesh mesh = model::load_model("data/models/box.obj");

    std::shared_ptr<Texture> diffuse_texture =
        Texture::create("data/textures/box.png", Texture::Type::Diffuse);

    std::shared_ptr<VertexBuffer> vertices =
        VertexBuffer::create_with_data(mesh.vertices.data(),
                                       mesh.vertices.size() * sizeof(model::Vertex));

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    layout.add(1, BufferLayout::Type::Float, 2);

    std::shared_ptr<VertexBuffer> index_buffer =
        VertexBuffer::create_index(mesh.indices.data(),
                                   mesh.indices.size() * sizeof(unsigned int));

    std::shared_ptr<VertexArray> vertex_array = VertexArray::create();
    index_buffer->bind();
    vertex_array->add_buffer(vertices, layout);
    
    VertexArray::unbind();

    box = registry.create();
    registry.emplace<TransformComponent>(box);
    std::vector<std::shared_ptr<VertexBuffer>> buffers = { vertices };
    registry.emplace<MeshComponent>(box, vertex_array, buffers, index_buffer,
                                    mesh.indices.size());
    registry.emplace<MaterialComponent>(box, basic_shader,
                                        std::unordered_map<std::string, int>());
    registry.emplace<TextureComponent>(box, diffuse_texture);
}

void OpenGLCanvas::build_piece() {
    model::Mesh mesh = model::load_model("data/models/piece.obj");

    std::shared_ptr<Texture> diffuse_texture =
        Texture::create("data/textures/black_piece_texture.png", Texture::Type::Diffuse);

    std::shared_ptr<VertexBuffer> vertices =
        VertexBuffer::create_with_data(mesh.vertices.data(),
                                       mesh.vertices.size() * sizeof(model::Vertex));

    BufferLayout layout;
    layout.add(0, BufferLayout::Type::Float, 3);
    layout.add(1, BufferLayout::Type::Float, 2);

    std::shared_ptr<VertexBuffer> index_buffer =
        VertexBuffer::create_index(mesh.indices.data(),
                                   mesh.indices.size() * sizeof(unsigned int));

    std::shared_ptr<VertexArray> vertex_array = VertexArray::create();
    index_buffer->bind();
    vertex_array->add_buffer(vertices, layout);
    
    VertexArray::unbind();

    piece = registry.create();
    registry.emplace<TransformComponent>(piece, glm::vec3(0.0f, 2.0f, 0.0f),
                                         glm::vec3(0.0f), 0.3f);
    std::vector<std::shared_ptr<VertexBuffer>> buffers = { vertices };
    registry.emplace<MeshComponent>(piece, vertex_array, buffers, index_buffer,
                                    mesh.indices.size());
    registry.emplace<MaterialComponent>(piece, basic_shader,
                                        std::unordered_map<std::string, int>());
    registry.emplace<TextureComponent>(piece, diffuse_texture);
}
