#include "scenes/nine_mens_morris_base_scene.hpp"

#include <nine_morris_3d_engine/external/resmanager.h++>
#include <nine_morris_3d_engine/external/imgui.h++>

#include "game/ray.hpp"
#include "global.hpp"
#include "muhle_engine.hpp"

void NineMensMorrisBaseScene::connect_events() {
    ctx.connect_event<sm::KeyReleasedEvent, &NineMensMorrisBaseScene::on_key_released>(this);
    ctx.connect_event<sm::MouseButtonPressedEvent, &NineMensMorrisBaseScene::on_mouse_button_pressed>(this);
    ctx.connect_event<sm::MouseButtonReleasedEvent, &NineMensMorrisBaseScene::on_mouse_button_released>(this);
}

void NineMensMorrisBaseScene::scene_setup() {
    m_board = setup_renderables();
    m_game_options.time = NineMensMorrisTime10min;
}

void NineMensMorrisBaseScene::scene_update() {
    m_board.update(
        ctx,
        cast_mouse_ray(ctx, m_camera),
        m_camera_controller.get_position(),
        m_game_state == GameState::HumanThinking && m_board.get_game_over() == GameOver::None
    );
}

void NineMensMorrisBaseScene::scene_fixed_update() {
    m_board.update_movement();
}

void NineMensMorrisBaseScene::scene_imgui_update() {
    m_board.debug();
}

BoardObj& NineMensMorrisBaseScene::get_board() {
    return m_board;
}

GamePlayer NineMensMorrisBaseScene::get_player_type() const {
    switch (m_game_options.game_type) {
        case GameTypeLocalHumanVsHuman:
            return GamePlayer::Human;
        case GameTypeLocalHumanVsComputer:
            return (
                m_game_options.local_human_vs_computer.computer_color == m_board.if_player_white(PlayerColorWhite, PlayerColorBlack)
                ?
                GamePlayer::Computer
                :
                GamePlayer::Human
            );
        case GameTypeOnline:
            return (
                m_game_options.online.remote_color == m_board.if_player_white(PlayerColorWhite, PlayerColorBlack)
                ?
                GamePlayer::Computer
                :
                GamePlayer::Human
            );
    }

    assert(false);
    return {};
}

void NineMensMorrisBaseScene::reset() {
    reset("w:w:b:1");
}

void NineMensMorrisBaseScene::reset(const std::string& string) {
    try {
        m_board.reset(NineMensMorrisBoard::position_from_string(string));
    } catch (const BoardError& e) {
        SM_THROW_ERROR(sm::ApplicationError, "Invalid input: {}", e.what());  // TODO fail gracefully
    }

    const auto clock_time {[](int time) -> unsigned int {
        switch (time) {
            case NineMensMorrisTime1min:
                return Clock::as_centiseconds(1);
            case NineMensMorrisTime3min:
                return Clock::as_centiseconds(3);
            case NineMensMorrisTime10min:
                return Clock::as_centiseconds(10);
            case NineMensMorrisTime60min:
                return Clock::as_centiseconds(60);
            case NineMensMorrisTimeCustom:
                // FIXME
                break;
        }

        assert(false);
        return {};
    }};

    m_game_state = GameState::Ready;
    m_clock.reset(clock_time(m_game_options.time));
    m_move_list.clear();

    if (m_board.get_setup_position().player == NineMensMorrisBoard::Player::Black) {
        m_clock.switch_turn();
        m_move_list.skip_first(true);
    }

    m_camera_controller.go_towards_position(m_default_camera_position);
}

void NineMensMorrisBaseScene::play_move(const std::string& string) {
    try {
        m_board.play_move(NineMensMorrisBoard::move_from_string(string));
    } catch (const BoardError& e) {
        SM_THROW_ERROR(sm::ApplicationError, "Invalid input: {}", e.what());  // TODO fail gracefully
    }
}

void NineMensMorrisBaseScene::timeout(PlayerColor color) {
    switch (color) {
        case PlayerColorWhite:
            m_board.timeout(NineMensMorrisBoard::Player::White);
            break;
        case PlayerColorBlack:
            m_board.timeout(NineMensMorrisBoard::Player::Black);
            break;
    }

    m_ui.set_popup_window(PopupWindow::GameOver);
    m_game_state = GameState::Stop;
}

void NineMensMorrisBaseScene::resign(PlayerColor color) {
    switch (color) {
        case PlayerColorWhite:
            m_board.resign(NineMensMorrisBoard::Player::White);
            break;
        case PlayerColorBlack:
            m_board.resign(NineMensMorrisBoard::Player::Black);
            break;
    }

    m_ui.set_popup_window(PopupWindow::GameOver);
    m_game_state = GameState::Stop;
}

void NineMensMorrisBaseScene::accept_draw_offer() {
    m_board.accept_draw_offer();

    m_ui.set_popup_window(PopupWindow::GameOver);
    m_game_state = GameState::Stop;
}

void NineMensMorrisBaseScene::time_control_options_window() {
    if (ImGui::RadioButton("1 min", &m_game_options.time, NineMensMorrisTime1min)) {
        m_clock.reset(Clock::as_centiseconds(1));
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("3 min", &m_game_options.time, NineMensMorrisTime3min)) {
        m_clock.reset(Clock::as_centiseconds(3));
    }

    if (ImGui::RadioButton("10 min", &m_game_options.time, NineMensMorrisTime10min)) {
        m_clock.reset(Clock::as_centiseconds(10));
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("60 min", &m_game_options.time, NineMensMorrisTime60min)) {
        m_clock.reset(Clock::as_centiseconds(60));
    }

    ImGui::RadioButton("Custom", &m_game_options.time, NineMensMorrisTimeCustom);

    if (m_game_options.time == NineMensMorrisTimeCustom) {
        // TODO
    }
}

void NineMensMorrisBaseScene::reload_scene_texture_data() const {
    // Global options must have been set to the desired textures

    const auto& g {ctx.global<Global>()};

    {
        sm::TexturePostProcessing post_processing;

        if (g.options.texture_quality == TextureQualityHalf) {
            post_processing.size = sm::TextureSize::Half;
        }

        ctx.reload_texture_data(ctx.path_assets("textures/board/board_diffuse.png"), post_processing);
        ctx.reload_texture_data(ctx.path_assets("textures/board/board_normal.png"), post_processing);
    }

    {
        sm::TexturePostProcessing post_processing;

        if (g.options.texture_quality == TextureQualityHalf) {
            post_processing.size = sm::TextureSize::Half;
        }

        ctx.reload_texture_data(ctx.path_assets("textures/board/paint_diffuse.png"), post_processing);
    }

    {
        sm::TexturePostProcessing post_processing;

        if (g.options.texture_quality == TextureQualityHalf) {
            post_processing.size = sm::TextureSize::Half;
        }

        ctx.reload_texture_data(ctx.path_assets("textures/piece/piece_white_diffuse.png"), post_processing);
        ctx.reload_texture_data(ctx.path_assets("textures/piece/piece_black_diffuse.png"), post_processing);
        ctx.reload_texture_data(ctx.path_assets("textures/piece/piece_normal.png"), post_processing);
    }
}

void NineMensMorrisBaseScene::reload_and_set_scene_textures() {
    const auto board_normal {load_board_normal_texture(true)};

    m_board.get_board_renderable().get_material()->set_texture(
        "u_material.ambient_diffuse"_H,
        load_board_diffuse_texture(true),
        0
    );

    m_board.get_board_renderable().get_material()->set_texture(
        "u_material.normal"_H,
        board_normal,
        1
    );

    m_board.get_paint_renderable().get_material()->set_texture(
        "u_material.ambient_diffuse"_H,
        load_paint_diffuse_texture(true),
        0
    );

    m_board.get_paint_renderable().get_material()->set_texture(
        "u_material.normal"_H,
        board_normal,
        1
    );

    const auto white_piece_diffuse {load_piece_white_diffuse_texture(true)};
    const auto black_piece_diffuse {load_piece_black_diffuse_texture(true)};
    const auto piece_normal {load_piece_normal_texture(true)};

    for (PieceObj& piece : m_board.get_pieces()) {
        switch (piece.get_type()) {
            case PieceType::White:
                piece.get_renderable().get_material()->set_texture(
                    "u_material.ambient_diffuse"_H,
                    white_piece_diffuse,
                    0
                );
                break;
            case PieceType::Black:
                piece.get_renderable().get_material()->set_texture(
                    "u_material.ambient_diffuse"_H,
                    black_piece_diffuse,
                    0
                );
                break;
        }

        piece.get_renderable().get_material()->set_texture(
            "u_material.normal"_H,
            piece_normal,
            1
        );
    }
}

sm::Renderable NineMensMorrisBaseScene::setup_board() const {
    const auto mesh {ctx.get_mesh("board.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("board"_H, mesh)};

    const auto diffuse {load_board_diffuse_texture()};
    const auto normal {load_board_normal_texture()};

    const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseNormalShadow, sm::Material::CastShadow)};

    const auto material_instance {ctx.load_material_instance("board"_H, material)};
    material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 8.0f);
    material_instance->set_texture("u_material.normal"_H, normal, 1);

    return sm::Renderable(mesh, vertex_array, material_instance);
}

sm::Renderable NineMensMorrisBaseScene::setup_paint() const {
    const auto mesh {ctx.get_mesh("paint.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("paint"_H, mesh)};

    const auto diffuse {load_paint_diffuse_texture()};
    const auto normal {load_board_normal_texture()};

    const auto material {ctx.load_material(
        "paint"_H,
        ctx.path_engine_assets("shaders/phong_diffuse_normal_shadow.vert"),
        ctx.path_assets("shaders/board/phong_diffuse_normal_shadow.frag"),
        sm::MaterialType::PhongDiffuseNormalShadow
    )};

    const auto material_instance {ctx.load_material_instance("paint"_H, material)};
    material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 8.0f);
    material_instance->set_texture("u_material.normal"_H, normal, 1);

    return sm::Renderable(mesh, vertex_array, material_instance);
}

std::vector<sm::Renderable> NineMensMorrisBaseScene::setup_nodes() const {
    const auto mesh {ctx.get_mesh("node.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("node"_H, mesh)};

    const auto material {ctx.load_material(sm::MaterialType::Phong)};

    std::vector<sm::Renderable> renderables;

    for (unsigned int i {0}; i < NineMensMorrisBoard::NODES; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("node" + std::to_string(i)), material)};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(0.075f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.3f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}

std::vector<sm::Renderable> NineMensMorrisBaseScene::setup_white_pieces() const {
    const auto mesh {ctx.get_mesh("piece_white.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("piece_white"_H, mesh)};

    const auto diffuse {load_piece_white_diffuse_texture()};
    const auto normal {load_piece_normal_texture()};

    const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseNormalShadow, sm::Material::CastShadow)};

    std::vector<sm::Renderable> renderables;

    for (unsigned int i {0}; i < NineMensMorrisBoard::PIECES / 2; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("piece_white" + std::to_string(i)), material)};
        material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);
        material_instance->set_texture("u_material.normal"_H, normal, 1);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}

std::vector<sm::Renderable> NineMensMorrisBaseScene::setup_black_pieces() const {
    const auto mesh {ctx.get_mesh("piece_black.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("piece_black"_H, mesh)};

    const auto diffuse {load_piece_black_diffuse_texture()};
    const auto normal {load_piece_normal_texture()};

    const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseNormalShadow, sm::Material::CastShadow)};

    std::vector<sm::Renderable> renderables;

    for (unsigned int i {0}; i < NineMensMorrisBoard::PIECES / 2; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("piece_black" + std::to_string(i)), material)};
        material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);
        material_instance->set_texture("u_material.normal"_H, normal, 1);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}

std::shared_ptr<sm::GlTexture> NineMensMorrisBaseScene::load_board_diffuse_texture(bool reload) const {
    const auto& g {ctx.global<Global>()};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;
    specification.mipmapping->anisotropic_filtering = g.options.anisotropic_filtering;

    if (reload) {
        return ctx.reload_texture("board_diffuse"_H, ctx.get_texture_data("board_diffuse.png"_H), specification);
    } else {
        return ctx.load_texture("board_diffuse"_H, ctx.get_texture_data("board_diffuse.png"_H), specification);
    }
}

std::shared_ptr<sm::GlTexture> NineMensMorrisBaseScene::load_paint_diffuse_texture(bool reload) const {
    const auto& g {ctx.global<Global>()};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;
    specification.mipmapping->anisotropic_filtering = g.options.anisotropic_filtering;

    if (reload) {
        return ctx.reload_texture("paint_diffuse"_H, ctx.get_texture_data("paint_diffuse.png"_H), specification);
    } else {
        return ctx.load_texture("paint_diffuse"_H, ctx.get_texture_data("paint_diffuse.png"_H), specification);
    }
}

std::shared_ptr<sm::GlTexture> NineMensMorrisBaseScene::load_board_normal_texture(bool reload) const {
    const auto& g {ctx.global<Global>()};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;
    specification.mipmapping->anisotropic_filtering = g.options.anisotropic_filtering;
    specification.format = sm::TextureFormat::Rgba8;

    if (reload) {
        return ctx.reload_texture("board_normal"_H, ctx.get_texture_data("board_normal.png"_H), specification);
    } else {
        return ctx.load_texture("board_normal"_H, ctx.get_texture_data("board_normal.png"_H), specification);
    }
}

std::shared_ptr<sm::GlTexture> NineMensMorrisBaseScene::load_piece_white_diffuse_texture(bool reload) const {
    const auto& g {ctx.global<Global>()};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;
    specification.mipmapping->anisotropic_filtering = g.options.anisotropic_filtering;

    if (reload) {
        return ctx.reload_texture("piece_white_diffuse.png"_H, ctx.get_texture_data("piece_white_diffuse.png"_H), specification);
    } else {
        return ctx.load_texture("piece_white_diffuse.png"_H, ctx.get_texture_data("piece_white_diffuse.png"_H), specification);
    }
}

std::shared_ptr<sm::GlTexture> NineMensMorrisBaseScene::load_piece_black_diffuse_texture(bool reload) const {
    const auto& g {ctx.global<Global>()};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;
    specification.mipmapping->anisotropic_filtering = g.options.anisotropic_filtering;

    if (reload) {
        return ctx.reload_texture("piece_black_diffuse.png"_H, ctx.get_texture_data("piece_black_diffuse.png"_H), specification);
    } else {
        return ctx.load_texture("piece_black_diffuse.png"_H, ctx.get_texture_data("piece_black_diffuse.png"_H), specification);
    }
}

std::shared_ptr<sm::GlTexture> NineMensMorrisBaseScene::load_piece_normal_texture(bool reload) const {
    const auto& g {ctx.global<Global>()};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;
    specification.mipmapping->anisotropic_filtering = g.options.anisotropic_filtering;
    specification.format = sm::TextureFormat::Rgba8;

    if (reload) {
        return ctx.reload_texture("piece_normal"_H, ctx.get_texture_data("piece_normal.png"_H), specification);
    } else {
        return ctx.load_texture("piece_normal"_H, ctx.get_texture_data("piece_normal.png"_H), specification);
    }
}

NineMensMorrisBoard NineMensMorrisBaseScene::setup_renderables() {
    return NineMensMorrisBoard(
        setup_board(),
        setup_paint(),
        setup_nodes(),
        setup_white_pieces(),
        setup_black_pieces(),
        [this](const NineMensMorrisBoard::Move& move) {
            m_move_list.push(NineMensMorrisBoard::move_to_string(move));

            m_clock.switch_turn();

            if (m_board.get_game_over() != GameOver::None) {  // TODO assert game over
                m_ui.set_popup_window(PopupWindow::GameOver);
                m_game_state = GameState::Stop;
                return;
            }

            m_game_state = GameState::NextTurn;
        }
    );
}
