#include "scenes/standard_game_scene.hpp"

#include <nine_morris_3d_engine/external/resmanager.h++>

#include "game/ray.hpp"
#include "global.hpp"
#include "game.hpp"
#include "constants.hpp"

void StandardGameScene::on_start() {
    GameScene::on_start();

    ctx.connect_event<sm::KeyReleasedEvent, &StandardGameScene::on_key_released>(this);
    ctx.connect_event<sm::MouseButtonPressedEvent, &StandardGameScene::on_mouse_button_pressed>(this);
    ctx.connect_event<sm::MouseButtonReleasedEvent, &StandardGameScene::on_mouse_button_released>(this);

    setup_turn_indicator();
    setup_timer();
    setup_renderables();
}

void StandardGameScene::on_stop() {
    GameScene::on_stop();
}

void StandardGameScene::on_update() {
    GameScene::on_update();

    // sm::listener::set_position(cam_controller.get_position());  // TODO

    m_board.update(ctx, cast_mouse_ray(ctx, m_cam), m_cam_controller.get_position());

    const auto& g {ctx.global<Global>()};

    if (!g.options.hide_turn_indicator) {
        m_turn_indicator.update(ctx, static_cast<TurnIndicatorType>(m_board.get_turn()));
    }

    m_timer.update();

    if (!g.options.hide_timer) {
        m_timer.render(ctx);
    }

    ctx.shadow(m_shadow_box);
}

void StandardGameScene::on_fixed_update() {
    GameScene::on_fixed_update();

    m_board.update_movement();
}

void StandardGameScene::on_imgui_update() {
    GameScene::on_imgui_update();

    m_board.debug();
}

Board& StandardGameScene::get_board() {
    return m_board;
}

void StandardGameScene::on_key_released(const sm::KeyReleasedEvent& event) {
    if (event.key == sm::Key::Space) {
        m_cam_controller.go_towards_position(m_default_camera_position);
    }
}

void StandardGameScene::on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event) {
    if (event.button == sm::MouseButton::Left) {
        m_board.user_click_press();
    }
}

void StandardGameScene::on_mouse_button_released(const sm::MouseButtonReleasedEvent& event) {
    if (event.button == sm::MouseButton::Left) {
        m_board.user_click_release();
    }
}

void StandardGameScene::setup_turn_indicator() {
    sm::TextureSpecification specification;
    specification.format = sm::TextureFormat::Rgba8;

    const auto white_texture {ctx.load_texture("white_indicator"_H, ctx.get_texture_data("white_indicator.png"_H), specification)};
    const auto black_texture {ctx.load_texture("black_indicator"_H, ctx.get_texture_data("black_indicator.png"_H), specification)};

    m_turn_indicator = TurnIndicator(white_texture, black_texture);
}

void StandardGameScene::setup_timer() {
    sm::FontSpecification specification;
    specification.size_height = 90.0f;
    specification.bitmap_size = 512;

    const auto font {ctx.load_font(
        "open_sans"_H,
        ctx.path_assets("fonts/OpenSans/OpenSans-Regular.ttf"),
        specification,
        [](sm::Font* font) {
            font->begin_baking();
            font->bake_ascii();
            font->end_baking();
        }
    )};

    m_timer = Timer(font);
}

void StandardGameScene::setup_renderables() {
    const auto renderable_board {setup_board()};
    const auto renderable_board_paint {setup_board_paint()};
    const auto renderable_nodes {setup_nodes()};
    const auto renderable_white_pieces {setup_white_pieces()};
    const auto renderable_black_pieces {setup_black_pieces()};

    m_board = StandardGameBoard(
        renderable_board,
        renderable_board_paint,
        renderable_nodes,
        renderable_white_pieces,
        renderable_black_pieces,
        [this](const StandardGameBoard::Move&) {
            if (!m_game_started) {
                m_timer.start();

                m_game_started = true;
            }

            if (m_board.get_game_over() != GameOver::None) {
                m_timer.stop();
            }
        }
    );
}

sm::Renderable StandardGameScene::setup_board() const {
    const auto mesh {ctx.get_mesh("board.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("board"_H, mesh)};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;

    const auto diffuse {ctx.load_texture(
        "board_diffuse"_H,
        ctx.get_texture_data("board_diffuse.png"_H),
        specification
    )};

    specification.format = sm::TextureFormat::Rgba8;

    const auto normal {ctx.load_texture(
        "board_normal"_H,
        ctx.get_texture_data("board_normal.png"_H),
        specification
    )};

    const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseNormalShadow, sm::Material::CastShadow)};

    const auto material_instance {ctx.load_material_instance("board"_H, material)};
    material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 8.0f);
    material_instance->set_texture("u_material.normal"_H, normal, 1);

    return sm::Renderable(mesh, vertex_array, material_instance);
}

sm::Renderable StandardGameScene::setup_board_paint() const {
    const auto mesh {ctx.get_mesh("board_paint.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("board_paint"_H, mesh)};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;

    const auto diffuse {get_board_paint_texture(specification)};

    specification.format = sm::TextureFormat::Rgba8;

    const auto normal {ctx.load_texture(
        "board_normal"_H,
        ctx.get_texture_data("board_normal.png"_H),
        specification
    )};

    const auto material {ctx.load_material(
        "board_paint"_H,
        ctx.path_engine_assets("shaders/phong_diffuse_normal_shadow.vert"),
        ctx.path_assets("shaders/board/phong_diffuse_normal_shadow.frag"),
        sm::MaterialType::PhongDiffuseNormalShadow
    )};

    const auto material_instance {ctx.load_material_instance("board_paint"_H, material)};
    material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 8.0f);
    material_instance->set_texture("u_material.normal"_H, normal, 1);

    return sm::Renderable(mesh, vertex_array, material_instance);
}

std::vector<sm::Renderable> StandardGameScene::setup_nodes() const {
    const auto mesh {ctx.get_mesh("node.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("node"_H, mesh)};

    const auto material {ctx.load_material(sm::MaterialType::Phong)};

    std::vector<sm::Renderable> renderables;

    for (unsigned int i {0}; i < 24; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("node" + std::to_string(i)), material)};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(0.075f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.3f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}

std::vector<sm::Renderable> StandardGameScene::setup_white_pieces() const {
    const auto mesh {ctx.get_mesh("piece_white.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("piece_white"_H, mesh)};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;

    const auto diffuse {ctx.load_texture(
        "piece_white_diffuse.png"_H,
        ctx.get_texture_data("piece_white_diffuse.png"_H),
        specification
    )};

    specification.format = sm::TextureFormat::Rgba8;

    const auto normal {ctx.load_texture(
        "piece_normal"_H,
        ctx.get_texture_data("piece_normal.png"_H),
        specification
    )};

    const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseNormalShadow, sm::Material::CastShadow)};

    std::vector<sm::Renderable> renderables;

    for (unsigned int i {0}; i < 9; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("piece_white" + std::to_string(i)), material)};
        material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);
        material_instance->set_texture("u_material.normal"_H, normal, 1);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}

std::vector<sm::Renderable> StandardGameScene::setup_black_pieces() const {
    const auto mesh {ctx.get_mesh("piece_black.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("piece_black"_H, mesh)};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;

    const auto diffuse {ctx.load_texture(
        "piece_black_diffuse.png"_H,
        ctx.get_texture_data("piece_black_diffuse.png"_H),
        specification
    )};

    specification.format = sm::TextureFormat::Rgba8;

    const auto normal {ctx.load_texture(
        "piece_normal"_H,
        ctx.get_texture_data("piece_normal.png"_H),
        specification
    )};

    const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseNormalShadow, sm::Material::CastShadow)};

    std::vector<sm::Renderable> renderables;

    for (unsigned int i {0}; i < 9; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("piece_black" + std::to_string(i)), material)};
        material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);
        material_instance->set_texture("u_material.normal"_H, normal, 1);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}
