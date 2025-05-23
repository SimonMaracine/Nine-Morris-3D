#include "scenes/nine_mens_morris_base_scene.hpp"

#include <algorithm>
#include <cassert>

#include <nine_morris_3d_engine/external/resmanager.h++>
#include <nine_morris_3d_engine/external/imgui.h++>

#include "engines/gbgp_engine.hpp"
#include "game/ray.hpp"
#include "global.hpp"

using namespace sm::localization_literals;

void NineMensMorrisBaseScene::scene_setup() {
    m_board = initialize_board();
    m_game_options.time_enum = NineMensMorrisTime10min;
}

void NineMensMorrisBaseScene::scene_update() {
    m_board.update(
        ctx,
        cast_mouse_ray(ctx, ctx.render_3d()->camera),
        ctx.render_3d()->get_camera_position(),
        m_game_state == GameState::HumanThinking && m_board.get_game_over() == GameOver::None
    );
}

void NineMensMorrisBaseScene::scene_fixed_update() {
    m_board.update_movement();
}

void NineMensMorrisBaseScene::scene_imgui_update() {
    m_board.debug_window();
}

BoardObj& NineMensMorrisBaseScene::board() {
    return m_board;
}

const BoardObj& NineMensMorrisBaseScene::board() const {
    return m_board;
}

GamePlayer NineMensMorrisBaseScene::player_type() const {
    const auto& g {ctx.global<Global>()};

    GamePlayer player {};

    switch (g.options.game_type) {
        case GameTypeLocal:
            player = GamePlayer::Human;
            break;
        case GameTypeLocalVsComputer:
            player = (
                m_game_options.computer_color == m_board.if_player_white(PlayerColorWhite, PlayerColorBlack)
                ?
                GamePlayer::Computer
                :
                GamePlayer::Human
            );
            break;
        case GameTypeOnline:
            player = (
                m_game_options.remote_color == m_board.if_player_white(PlayerColorWhite, PlayerColorBlack)
                ?
                GamePlayer::Remote
                :
                GamePlayer::Human
            );
            break;
    }

    return player;
}

std::string NineMensMorrisBaseScene::setup_position() const {
    return NineMensMorrisBoard::position_to_string(m_board.setup_position());
}

void NineMensMorrisBaseScene::reset(const TimedMoves& moves) {
    GameScene::reset("w:w:b:1", moves);
}

void NineMensMorrisBaseScene::reset_board(const std::string& string) {
    try {
        m_board.reset(NineMensMorrisBoard::position_from_string(string));
    } catch (const BoardError& e) {
        SM_THROW_ERROR(sm::ApplicationError, "Invalid input: {}", e.what());  // TODO fail gracefully
    }
}

bool NineMensMorrisBaseScene::second_player_starting() {
    return m_board.setup_position().player == NineMensMorrisBoard::Player::Black;
}

Clock::Time NineMensMorrisBaseScene::clock_time(int time_enum) {
    Clock::Time time {};

    switch (time_enum) {
        case NineMensMorrisTime1min:
            time = Clock::as_milliseconds(1);
            break;
        case NineMensMorrisTime3min:
            time = Clock::as_milliseconds(3);
            break;
        case NineMensMorrisTime10min:
            time = Clock::as_milliseconds(10);
            break;
        case NineMensMorrisTime60min:
            time = Clock::as_milliseconds(60);
            break;
        case NineMensMorrisTimeCustom:
            time = Clock::as_milliseconds(m_game_options.custom_time);
            break;
    }

    return time;
}

void NineMensMorrisBaseScene::set_time_control_options(Clock::Time time) {
    switch (time) {
        case Clock::as_milliseconds(1):
            m_game_options.time_enum = NineMensMorrisTime1min;
            break;
        case Clock::as_milliseconds(3):
            m_game_options.time_enum = NineMensMorrisTime3min;
            break;
        case Clock::as_milliseconds(10):
            m_game_options.time_enum = NineMensMorrisTime10min;
            break;
        case Clock::as_milliseconds(60):
            m_game_options.time_enum = NineMensMorrisTime60min;
            break;
        default:
            m_game_options.time_enum = NineMensMorrisTimeCustom;
            m_game_options.custom_time = Clock::as_minutes(time);
            break;
    }
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

    m_game_state = GameState::Stop;
}

void NineMensMorrisBaseScene::accept_draw() {
    m_board.accept_draw();

    m_game_state = GameState::Stop;
}

void NineMensMorrisBaseScene::time_control_options_window() {
    if (ImGui::RadioButton("1 min"_L, &m_game_options.time_enum, NineMensMorrisTime1min)) {
        m_clock.reset(Clock::as_milliseconds(1));
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("3 min"_L, &m_game_options.time_enum, NineMensMorrisTime3min)) {
        m_clock.reset(Clock::as_milliseconds(3));
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("10 min"_L, &m_game_options.time_enum, NineMensMorrisTime10min)) {
        m_clock.reset(Clock::as_milliseconds(10));
    }

    if (ImGui::RadioButton("60 min"_L, &m_game_options.time_enum, NineMensMorrisTime60min)) {
        m_clock.reset(Clock::as_milliseconds(60));
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("Custom"_L, &m_game_options.time_enum, NineMensMorrisTimeCustom)) {
        m_game_options.custom_time = std::clamp(m_game_options.custom_time, 0, 999);
        m_clock.reset(Clock::as_milliseconds(m_game_options.custom_time));
    }

    if (m_game_options.time_enum == NineMensMorrisTimeCustom) {
        ImGui::PushItemWidth(Ui::rem(6.0f));
        if (ImGui::InputInt("##", &m_game_options.custom_time)) {
            m_game_options.custom_time = std::clamp(m_game_options.custom_time, 0, 999);
            m_clock.reset(Clock::as_milliseconds(m_game_options.custom_time));
        }
        ImGui::PopItemWidth();
    }
}

void NineMensMorrisBaseScene::start_engine() {
    assert(!m_engine);

    m_engine = std::make_shared<GbgpEngine>();
#ifndef SM_BUILD_DISTRIBUTION
    m_engine->set_log_output(true, "nine_mens_morris_engine.log");
#endif

#if defined(SM_BUILD_DISTRIBUTION) && defined(SM_PLATFORM_LINUX)
    const bool search_executable {true};
#else
    const bool search_executable {false};
#endif

    try {
#ifdef SM_PLATFORM_WINDOWS
        m_engine->initialize("nine_morris_3d_engine_muhle_intelligence.exe", search_executable);
#else
        m_engine->initialize("nine_morris_3d_engine_muhle_intelligence", search_executable);
#endif

#ifndef SM_BUILD_DISTRIBUTION
        m_engine->set_debug(true);
#endif
        m_engine->new_game();
        m_engine->synchronize();
    } catch (const EngineError& e) {
        engine_error(e);
        return;
    }

    const auto iter {std::find_if(m_engine->get_options().cbegin(), m_engine->get_options().cend(), [](const auto& option) {
        return option.name == "TwelveMensMorris";
    })};

    if (iter == m_engine->get_options().cend()) {
        SM_THROW_ERROR(sm::ApplicationError, "Engine doesn't support twelve men's morris");
    }

    try {
        m_engine->set_option("TwelveMensMorris", twelve_mens_morris() ? "true" : "false");
    } catch (const EngineError& e) {
        engine_error(e);
    }
}

void NineMensMorrisBaseScene::load_game_icons() {
    sm::TextureSpecification specification;
    specification.format = sm::TextureFormat::Rgba8;

    sm::TexturePostProcessing post_processing;
    post_processing.flip = false;

    m_icon_white = ctx.load_texture(
        "icon_white"_H,
        ctx.load_texture_data(ctx.path_assets("icons/nine_mens_morris/icon_white.png"), post_processing),
        specification
    );

    m_icon_black = ctx.load_texture(
        "icon_black"_H,
        ctx.load_texture_data(ctx.path_assets("icons/nine_mens_morris/icon_black.png"), post_processing),
        specification
    );
}

void NineMensMorrisBaseScene::reload_scene_texture_data() const {
    // Global options must have been set to the desired textures

    const auto& g {ctx.global<Global>()};

    sm::TexturePostProcessing post_processing;

    if (g.options.texture_quality == TextureQualityHalf) {
        post_processing.size = sm::TextureSize::Half;
    }

    {
        ctx.reload_texture_data(ctx.path_assets("textures/board/board_diffuse.png"), post_processing);
        ctx.reload_texture_data(ctx.path_assets("textures/board/board_normal.png"), post_processing);
    }

    {
        ctx.reload_texture_data(ctx.path_assets("textures/board/paint_diffuse.png"), post_processing);
    }

    {
        ctx.reload_texture_data(ctx.path_assets("textures/piece/piece_white_diffuse.png"), post_processing);
        ctx.reload_texture_data(ctx.path_assets("textures/piece/piece_black_diffuse.png"), post_processing);
        ctx.reload_texture_data(ctx.path_assets("textures/piece/piece_normal.png"), post_processing);
    }
}

void NineMensMorrisBaseScene::reload_and_set_scene_textures() {
    const auto board_normal {load_board_normal_texture(true)};

    m_board.get_board_model()->get_material()->set_texture(
        "u_material.ambient_diffuse"_H,
        load_board_diffuse_texture(true),
        0
    );

    m_board.get_board_model()->get_material()->set_texture(
        "u_material.normal"_H,
        board_normal,
        1
    );

    m_board.get_paint_model()->get_material()->set_texture(
        "u_material.ambient_diffuse"_H,
        load_paint_diffuse_texture(true),
        0
    );

    m_board.get_paint_model()->get_material()->set_texture(
        "u_material.normal"_H,
        board_normal,
        1
    );

    const auto piece_white_diffuse {load_piece_white_diffuse_texture(true)};
    const auto piece_black_diffuse {load_piece_black_diffuse_texture(true)};
    const auto piece_normal {load_piece_normal_texture(true)};

    for (PieceObj& piece : m_board.get_pieces()) {
        piece.get_model()->get_material()->set_texture(
            "u_material.ambient_diffuse"_H,
            piece.get_type() == PieceType::White ? piece_white_diffuse : piece_black_diffuse,
            0
        );

        piece.get_model()->get_material()->set_texture(
            "u_material.normal"_H,
            piece_normal,
            1
        );
    }
}

int NineMensMorrisBaseScene::score_bound() const {
    return 150;
}

unsigned int NineMensMorrisBaseScene::white_color() const {
    return IM_COL32(217, 175, 116, 255);
}

unsigned int NineMensMorrisBaseScene::black_color() const {
    return IM_COL32(81, 81, 79, 255);
}

std::shared_ptr<sm::ModelNode> NineMensMorrisBaseScene::setup_board() const {
    const auto mesh {ctx.get_mesh("board.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("board"_H, mesh)};

    const auto diffuse {load_board_diffuse_texture()};
    const auto normal {load_board_normal_texture()};

    const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseNormalShadow)};

    const auto material_instance {ctx.load_material_instance("board"_H, material)};
    material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.05f));
    material_instance->set_float("u_material.shininess"_H, 8.0f);
    material_instance->set_texture("u_material.normal"_H, normal, 1);

    const auto model {std::make_shared<sm::ModelNode>(mesh, vertex_array, material_instance)};
    model->id = "board"_H;
    model->cast_shadow = sm::NodeFlag::Enabled;

    return model;
}

std::shared_ptr<sm::ModelNode> NineMensMorrisBaseScene::setup_paint() const {
    const auto mesh {ctx.get_mesh("paint.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("paint"_H, mesh)};

    const auto diffuse {load_paint_diffuse_texture()};
    const auto normal {load_board_normal_texture()};

    const auto material {ctx.load_material(
        "paint"_H,
        ctx.path_engine_assets("shaders/phong_diffuse_normal_shadow.vert"),
        ctx.path_assets("shaders/nine_mens_morris/board/phong_diffuse_normal_shadow.frag"),
        sm::MaterialType::PhongDiffuseNormalShadow
    )};

    const auto material_instance {ctx.load_material_instance("paint"_H, material)};
    material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.05f));
    material_instance->set_float("u_material.shininess"_H, 8.0f);
    material_instance->set_texture("u_material.normal"_H, normal, 1);

    const auto model {std::make_shared<sm::ModelNode>(mesh, vertex_array, material_instance)};
    model->cast_shadow = sm::NodeFlag::Disabled;

    return model;
}

NineMensMorrisBoard::NodeModels NineMensMorrisBaseScene::setup_nodes() const {
    const auto mesh {ctx.get_mesh("node.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("node"_H, mesh)};

    const auto material {ctx.load_material(sm::MaterialType::Phong)};

    NineMensMorrisBoard::NodeModels models;

    for (int i {0}; i < NineMensMorrisBoard::NODES; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("node" + std::to_string(i)), material)};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(0.065f));  // FIXME depends on the environment lighting
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.05f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);

        const auto model {std::make_shared<sm::ModelNode>(mesh, vertex_array, material_instance)};
        model->cast_shadow = sm::NodeFlag::Disabled;

        models.push_back(model);
    }

    return models;
}

NineMensMorrisBoard::PieceModels NineMensMorrisBaseScene::setup_white_pieces() const {
    const auto mesh {ctx.get_mesh("piece.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("piece"_H, mesh)};

    const auto diffuse {load_piece_white_diffuse_texture()};
    const auto normal {load_piece_normal_texture()};

    const auto material {ctx.load_material(
        "piece"_H,
        ctx.path_engine_assets("shaders/phong_diffuse_normal_shadow.vert"),
        ctx.path_assets("shaders/nine_mens_morris/piece/phong_diffuse_normal_shadow.frag"),
        sm::MaterialType::PhongDiffuseNormalShadow
    )};

    material->add_uniform(sm::Material::Uniform::Vec3, "u_highlight_color"_H);

    NineMensMorrisBoard::PieceModels models;

    for (int i {0}; i < pieces_count() / 2; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("piece_white" + std::to_string(i)), material)};
        material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.05f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);
        material_instance->set_texture("u_material.normal"_H, normal, 1);

        models.push_back(std::make_shared<sm::ModelNode>(mesh, vertex_array, material_instance));
    }

    return models;
}

NineMensMorrisBoard::PieceModels NineMensMorrisBaseScene::setup_black_pieces() const {
    const auto mesh {ctx.get_mesh("piece.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("piece"_H, mesh)};

    const auto diffuse {load_piece_black_diffuse_texture()};
    const auto normal {load_piece_normal_texture()};

    const auto material {ctx.load_material(
        "piece"_H,
        ctx.path_engine_assets("shaders/phong_diffuse_normal_shadow.vert"),
        ctx.path_assets("shaders/nine_mens_morris/piece/phong_diffuse_normal_shadow.frag"),
        sm::MaterialType::PhongDiffuseNormalShadow
    )};

    material->add_uniform(sm::Material::Uniform::Vec3, "u_highlight_color"_H);

    NineMensMorrisBoard::PieceModels models;

    for (int i {0}; i < pieces_count() / 2; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("piece_black" + std::to_string(i)), material)};
        material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.05f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);
        material_instance->set_texture("u_material.normal"_H, normal, 1);

        models.push_back(std::make_shared<sm::ModelNode>(mesh, vertex_array, material_instance));
    }

    return models;
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

NineMensMorrisBoard NineMensMorrisBaseScene::initialize_board() {
    return NineMensMorrisBoard(
        setup_board(),
        setup_paint(),
        setup_nodes(),
        setup_white_pieces(),
        setup_black_pieces(),
        ctx.load_sound_data(ctx.path_assets("sounds/nine_mens_morris/piece_place1.ogg")),
        ctx.load_sound_data(ctx.path_assets("sounds/nine_mens_morris/piece_place2.ogg")),
        ctx.load_sound_data(ctx.path_assets("sounds/nine_mens_morris/piece_place3.ogg")),
        ctx.load_sound_data(ctx.path_assets("sounds/nine_mens_morris/piece_move1.ogg")),
        ctx.load_sound_data(ctx.path_assets("sounds/nine_mens_morris/piece_move2.ogg")),
        ctx.load_sound_data(ctx.path_assets("sounds/nine_mens_morris/piece_move3.ogg")),
        ctx.load_sound_data(ctx.path_assets("sounds/nine_mens_morris/piece_capture1.ogg")),
        ctx.load_sound_data(ctx.path_assets("sounds/nine_mens_morris/piece_capture1.ogg")),
        [this](const NineMensMorrisBoard::Move& move) {
            m_moves_list.push(NineMensMorrisBoard::move_to_string(move));  // This last move will be read soon
            m_game_state = GameState::FinishTurn;
        }
    );
}

int NineMensMorrisBaseScene::pieces_count() const {
    return twelve_mens_morris() ? NineMensMorrisBoard::TWELVE : NineMensMorrisBoard::NINE;
}
