#include "scenes/game_scene.hpp"

#include <nine_morris_3d_engine/external/resmanager.h++>

#include "global.hpp"
#include "muhle_engine.hpp"

void GameScene::on_start() {
    ctx.connect_event<sm::WindowResizedEvent, &GameScene::on_window_resized>(this);

    ctx.set_renderer_clear_color(glm::vec3(0.1f, 0.1f, 0.1f));

    setup_camera();
    setup_skybox();
    setup_lights();

    m_ui.initialize(ctx);

    const auto& g {ctx.global<Global>()};

    // m_player_white = static_cast<GamePlayer>(g.options.white_player);
    // m_player_black = static_cast<GamePlayer>(g.options.black_player);

    // muhle_engine::initialize();  // TODO don't create and destroy engine every time
}

void GameScene::on_stop() {
    // if (m_engine_started) {
    //     muhle_engine::send_message("quit\n");
    // }

    // muhle_engine::uninitialize();

    m_cam_controller.disconnect_events(ctx);
    ctx.disconnect_events(this);
}

void GameScene::on_update() {
    // if (!m_engine_started) {
    //     if (muhle_engine::is_ready()) {
    //         muhle_engine::send_message("init\n");
    //         m_engine_started = true;
    //     }
    // }

    m_cam_controller.update_controls(ctx.get_delta(), ctx);
    m_cam_controller.update_camera(ctx.get_delta());

    ctx.capture(m_cam, m_cam_controller.get_position());
    ctx.capture(m_cam_2d);

    ctx.add_light(m_directional_light);

    if (m_skybox) {
        ctx.skybox(m_skybox);
    }

    if (m_ui.get_show_information()) {
        ctx.show_information_text();
    }

    // Origin
    ctx.debug_add_line(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ctx.debug_add_line(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ctx.debug_add_line(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Light
    ctx.debug_add_lamp(glm::normalize(-m_directional_light.direction) * 15.0f, glm::vec3(0.6f));
}

void GameScene::on_fixed_update() {
    m_cam_controller.update_friction();
}

void GameScene::on_imgui_update() {
    m_ui.update(ctx, *this);
}

void GameScene::load_and_set_skybox() {
    ctx.add_task_async([this](sm::AsyncTask& task, void*) {
        load_skybox_texture_data();

        ctx.add_task([this](const sm::Task&, void*) {
            setup_skybox();
            setup_lights();
            m_ui.set_loading_skybox_done();

            return sm::Task::Result::Done;
        });

        task.set_done();
    });
}

void GameScene::load_and_set_board_paint_texture() {
    ctx.add_task_async([this](sm::AsyncTask& task, void*) {
        load_board_paint_texture_data();

        ctx.add_task([this](const sm::Task&, void*) {
            get_board().get_paint_renderable().get_material()->set_texture(
                "u_material.ambient_diffuse"_H,
                load_board_paint_diffuse_texture(true),
                0
            );

            return sm::Task::Result::Done;
        });

        task.set_done();
    });
}

void GameScene::load_and_set_textures() {
    ctx.add_task_async([this](sm::AsyncTask& task, void*) {
        load_all_texture_data();

        ctx.add_task([this](const sm::Task&, void*) {
            setup_skybox();
            set_renderable_textures();

            return sm::Task::Result::Done;
        });

        task.set_done();
    });
}

void GameScene::set_renderable_textures() {
    const auto board_normal {load_board_normal_texture(true)};

    get_board().get_renderable().get_material()->set_texture(
        "u_material.ambient_diffuse"_H,
        load_board_diffuse_texture(true),
        0
    );

    get_board().get_renderable().get_material()->set_texture(
        "u_material.normal"_H,
        board_normal,
        1
    );

    get_board().get_paint_renderable().get_material()->set_texture(
        "u_material.ambient_diffuse"_H,
        load_board_paint_diffuse_texture(true),
        0
    );

    get_board().get_paint_renderable().get_material()->set_texture(
        "u_material.normal"_H,
        board_normal,
        1
    );

    const auto white_piece_diffuse {load_white_piece_diffuse_texture(true)};
    const auto black_piece_diffuse {load_black_piece_diffuse_texture(true)};
    const auto piece_normal {load_piece_normal_texture(true)};

    for (PieceObj& piece : get_board().get_pieces()) {
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

void GameScene::on_window_resized(const sm::WindowResizedEvent& event) {
    m_cam.set_projection(event.width, event.height, LENS_FOV, LENS_NEAR, LENS_FAR);
    m_cam_2d.set_projection(0, event.width, 0, event.height);
}

/* FIXME
    don't allow the computer to automatically start the game
    make button for the computer to start the game
    make button to stop the computer from thinking
    board's nodes and pieces should update only when it's human's turn
    when computer plays with itself, there is no pause and pieces end up in invalid places (probably a bug)
    right now user can only change the player types when it's their turn, this is not good
    the engine and protocol are not yet done and they have bugs (+ support the other game modes)
*/

/* TODO
    saving and loading games
    undo and redo moves
    piece highlights (+ user feedback)
    create the engine once at the beginning
    loading screen

    implement all other game modes + finish engine
*/
void GameScene::update_game_state() {
    // switch (m_game_state) {
    //     case GameState::NextPlayer: {
    //         const auto switch_player {
    //             [this](GamePlayer game_player) {
    //                 switch (game_player) {
    //                     case GamePlayer::Human:
    //                         m_game_state = GameState::HumanMakeMove;
    //                         break;
    //                     case GamePlayer::Computer:
    //                         m_game_state = GameState::ComputerThink;
    //                         break;
    //                 }
    //             }
    //         };

    //         switch (get_board().get_turn()) {
    //             case Player::White:
    //                 switch_player(m_player_white);
    //                 break;
    //             case Player::Black:
    //                 switch_player(m_player_black);
    //                 break;
    //         }

    //         break;
    //     }
    //     case GameState::HumanMakeMove:  // FIXME update board only in this state
    //         break;
    //     case GameState::ComputerThink:
    //         if (!m_engine_started) {
    //             break;
    //         }

    //         muhle_engine::send_message("go\n");
    //         m_game_state = GameState::ComputerMakeMove;

    //         break;
    //     case GameState::ComputerMakeMove: {
    //         const auto message {muhle_engine::receive_message()};

    //         if (!message) {
    //             break;
    //         }

    //         const auto tokens {muhle_engine::tokenize_message(*message)};

    //         if (tokens.at(0) == "bestmove") {
    //             if (tokens.at(1) == "none") {
    //                 break;
    //             }

    //             play_move_on_board(tokens.at(1));
    //         } else if (tokens.at(0) == "info") {
    //             LOG_INFO("{}", message->substr(0, message->size() - 1));
    //         }

    //         break;
    //     }
    //     case GameState::Over:
    //         break;
    // }
}

void GameScene::setup_camera() {
    const auto& g {ctx.global<Global>()};

    m_cam_controller = PointCameraController(
        m_cam,
        ctx.get_window_width(),
        ctx.get_window_height(),
        glm::vec3(0.0f),
        8.0f,
        47.0f,
        g.options.camera_sensitivity
    );

    m_cam_controller.connect_events(ctx);

    m_default_camera_position = m_cam_controller.get_position();
    m_cam_controller.set_distance_to_point(m_cam_controller.get_distance_to_point() + 1.0f);
    m_cam_controller.go_towards_position(m_default_camera_position);

    m_cam_2d.set_projection(0, ctx.get_window_width(), 0, ctx.get_window_height());
}

void GameScene::setup_skybox() {
    m_skybox = load_skybox_texture_cubemap();
}

void GameScene::setup_lights() {
    const auto& g {ctx.global<Global>()};

    switch (g.options.skybox) {
        case static_cast<int>(Skybox::None):
            m_directional_light.direction = glm::normalize(glm::vec3(0.1f, -0.8f, 0.1f));
            m_directional_light.ambient_color = glm::vec3(0.07f);
            m_directional_light.diffuse_color = glm::vec3(0.6f);
            m_directional_light.specular_color = glm::vec3(0.75f);
            break;
        case static_cast<int>(Skybox::Field):
            m_directional_light.direction = glm::normalize(glm::vec3(-0.198f, -0.192f, -0.282f));
            m_directional_light.ambient_color = glm::vec3(0.08f);
            m_directional_light.diffuse_color = glm::vec3(0.95f);
            m_directional_light.specular_color = glm::vec3(1.0f);
            break;
        case static_cast<int>(Skybox::Autumn):
            m_directional_light.direction = glm::normalize(glm::vec3(0.4f, -1.0f, -0.1f));
            m_directional_light.ambient_color = glm::vec3(0.15f);
            m_directional_light.diffuse_color = glm::vec3(0.75f);
            m_directional_light.specular_color = glm::vec3(0.65f);
            break;
    }
}

sm::Renderable GameScene::setup_board() const {
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

sm::Renderable GameScene::setup_board_paint() const {
    const auto mesh {ctx.get_mesh("board_paint.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("board_paint"_H, mesh)};

    const auto diffuse {load_board_paint_diffuse_texture()};
    const auto normal {load_board_normal_texture()};

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

std::vector<sm::Renderable> GameScene::setup_nodes(unsigned int count) const {
    const auto mesh {ctx.get_mesh("node.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("node"_H, mesh)};

    const auto material {ctx.load_material(sm::MaterialType::Phong)};

    std::vector<sm::Renderable> renderables;

    for (unsigned int i {0}; i < count; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("node" + std::to_string(i)), material)};
        material_instance->set_vec3("u_material.ambient_diffuse"_H, glm::vec3(0.075f));
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.3f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}

std::vector<sm::Renderable> GameScene::setup_white_pieces(unsigned int count) const {
    const auto mesh {ctx.get_mesh("piece_white.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("piece_white"_H, mesh)};

    const auto diffuse {load_white_piece_diffuse_texture()};
    const auto normal {load_piece_normal_texture()};

    const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseNormalShadow, sm::Material::CastShadow)};

    std::vector<sm::Renderable> renderables;

    for (unsigned int i {0}; i < count; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("piece_white" + std::to_string(i)), material)};
        material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);
        material_instance->set_texture("u_material.normal"_H, normal, 1);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}

std::vector<sm::Renderable> GameScene::setup_black_pieces(unsigned int count) const {
    const auto mesh {ctx.get_mesh("piece_black.obj"_H)};

    const auto vertex_array {ctx.load_vertex_array("piece_black"_H, mesh)};

    const auto diffuse {load_black_piece_diffuse_texture()};
    const auto normal {load_piece_normal_texture()};

    const auto material {ctx.load_material(sm::MaterialType::PhongDiffuseNormalShadow, sm::Material::CastShadow)};

    std::vector<sm::Renderable> renderables;

    for (unsigned int i {0}; i < count; i++) {
        const auto material_instance {ctx.load_material_instance(sm::Id("piece_black" + std::to_string(i)), material)};
        material_instance->set_texture("u_material.ambient_diffuse"_H, diffuse, 0);
        material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
        material_instance->set_float("u_material.shininess"_H, 8.0f);
        material_instance->set_texture("u_material.normal"_H, normal, 1);

        renderables.emplace_back(mesh, vertex_array, material_instance);
    }

    return renderables;
}

// TurnIndicator GameScene::setup_turn_indicator() const {
//     sm::TextureSpecification specification;
//     specification.format = sm::TextureFormat::Rgba8;

//     const auto white_texture {ctx.load_texture("white_indicator"_H, ctx.get_texture_data("white_indicator.png"_H), specification)};
//     const auto black_texture {ctx.load_texture("black_indicator"_H, ctx.get_texture_data("black_indicator.png"_H), specification)};

//     return TurnIndicator(white_texture, black_texture);
// }

// Timer GameScene::setup_timer() const {
//     sm::FontSpecification specification;
//     specification.size_height = 90.0f;
//     specification.bitmap_size = 512;

//     const auto font {ctx.load_font(
//         "open_sans"_H,
//         ctx.path_assets("fonts/OpenSans/OpenSans-Regular.ttf"),
//         specification,
//         [](sm::Font* font) {
//             font->begin_baking();
//             font->bake_ascii();
//             font->end_baking();
//         }
//     )};

//     return Timer(font);
// }

void GameScene::load_skybox_texture_data() const {
    // Global options must have been set to the desired skybox

    const auto& g {ctx.global<Global>()};

    sm::TexturePostProcessing post_processing;
    post_processing.flip = false;

    switch (g.options.skybox) {
        case static_cast<int>(Skybox::None):
            break;
        case static_cast<int>(Skybox::Field):
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/px.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/nx.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/py.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/ny.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/pz.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/nz.png"), post_processing);
            break;
        case static_cast<int>(Skybox::Autumn):
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/px.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/nx.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/py.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/ny.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/pz.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/nz.png"), post_processing);
            break;
    }
}

void GameScene::load_board_paint_texture_data() const {
    // Global options must have been set to the desired texture

    const auto& g {ctx.global<Global>()};

    // if (g.options.labeled_board) {
        ctx.load_texture_data(ctx.path_assets("textures/board/board_paint_labeled_diffuse.png"), sm::TexturePostProcessing());
    // } else {
    //     ctx.load_texture_data(ctx.path_assets("textures/board/board_paint_diffuse.png"), sm::TexturePostProcessing());
    // }
}

void GameScene::load_all_texture_data() const {
    // Global options must have been set to the desired textures

    const auto& g {ctx.global<Global>()};

    {
        sm::TexturePostProcessing post_processing;
        post_processing.flip = false;

        if (g.options.texture_quality == static_cast<int>(TextureQuality::Half)) {
            post_processing.size = sm::TextureSize::Half;
        }

        switch (g.options.skybox) {
            case static_cast<int>(Skybox::None):
                break;
            case static_cast<int>(Skybox::Field):
                ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/px.png"), post_processing);
                ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/nx.png"), post_processing);
                ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/py.png"), post_processing);
                ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/ny.png"), post_processing);
                ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/pz.png"), post_processing);
                ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/nz.png"), post_processing);
                break;
            case static_cast<int>(Skybox::Autumn):
                ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/px.png"), post_processing);
                ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/nx.png"), post_processing);
                ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/py.png"), post_processing);
                ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/ny.png"), post_processing);
                ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/pz.png"), post_processing);
                ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/nz.png"), post_processing);
                break;
        }
    }

    {
        sm::TexturePostProcessing post_processing;

        if (g.options.texture_quality == static_cast<int>(TextureQuality::Half)) {
            post_processing.size = sm::TextureSize::Half;
        }

        ctx.reload_texture_data(ctx.path_assets("textures/board/board_diffuse.png"), post_processing);
        ctx.reload_texture_data(ctx.path_assets("textures/board/board_normal.png"), post_processing);
    }

    {
        sm::TexturePostProcessing post_processing;

        if (g.options.texture_quality == static_cast<int>(TextureQuality::Half)) {
            post_processing.size = sm::TextureSize::Half;
        }

        // if (g.options.labeled_board) {
            ctx.reload_texture_data(ctx.path_assets("textures/board/board_paint_labeled_diffuse.png"), post_processing);
        // } else {
        //     ctx.reload_texture_data(ctx.path_assets("textures/board/board_paint_diffuse.png"), post_processing);
        // }
    }

    {
        sm::TexturePostProcessing post_processing;

        if (g.options.texture_quality == static_cast<int>(TextureQuality::Half)) {
            post_processing.size = sm::TextureSize::Half;
        }

        ctx.reload_texture_data(ctx.path_assets("textures/piece/piece_white_diffuse.png"), post_processing);
        ctx.reload_texture_data(ctx.path_assets("textures/piece/piece_black_diffuse.png"), post_processing);
        ctx.reload_texture_data(ctx.path_assets("textures/piece/piece_normal.png"), post_processing);
    }
}

std::shared_ptr<sm::GlTexture> GameScene::load_board_diffuse_texture(bool reload) const {
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

std::shared_ptr<sm::GlTexture> GameScene::load_board_paint_diffuse_texture(bool reload) const {
    const auto& g {ctx.global<Global>()};

    sm::TextureSpecification specification;
    specification.mipmapping.emplace();
    specification.mipmapping->bias = -0.8f;
    specification.mipmapping->anisotropic_filtering = g.options.anisotropic_filtering;

    if (reload) {
        return ctx.reload_texture(
            /*g.options.labeled_board ?*/ "board_paint_labeled_diffuse"_H /*: "board_paint_diffuse"_H*/,
            ctx.get_texture_data(/*g.options.labeled_board ?*/ "board_paint_labeled_diffuse.png"_H /*: "board_paint_diffuse.png"_H*/),
            specification
        );
    } else {
        return ctx.load_texture(
            /*g.options.labeled_board ?*/ "board_paint_labeled_diffuse"_H /*: "board_paint_diffuse"_H*/,
            ctx.get_texture_data(/*g.options.labeled_board ?*/ "board_paint_labeled_diffuse.png"_H /*: "board_paint_diffuse.png"_H*/),
            specification
        );
    }
}

std::shared_ptr<sm::GlTexture> GameScene::load_board_normal_texture(bool reload) const {
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

std::shared_ptr<sm::GlTexture> GameScene::load_white_piece_diffuse_texture(bool reload) const {
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

std::shared_ptr<sm::GlTexture> GameScene::load_black_piece_diffuse_texture(bool reload) const {
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

std::shared_ptr<sm::GlTexture> GameScene::load_piece_normal_texture(bool reload) const {
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

std::shared_ptr<sm::GlTextureCubemap> GameScene::load_skybox_texture_cubemap(bool reload) const {
    const auto& g {ctx.global<Global>()};

    const auto load_or_reload {
        [this, reload](sm::Id id) {
            if (reload) {
                return ctx.reload_texture_cubemap(
                    id,
                    {
                        ctx.get_texture_data("px.png"_H),
                        ctx.get_texture_data("nx.png"_H),
                        ctx.get_texture_data("py.png"_H),
                        ctx.get_texture_data("ny.png"_H),
                        ctx.get_texture_data("pz.png"_H),
                        ctx.get_texture_data("nz.png"_H)
                    },
                    sm::TextureFormat::Srgb8Alpha8
                );
            } else {
                return ctx.load_texture_cubemap(
                    id,
                    {
                        ctx.get_texture_data("px.png"_H),
                        ctx.get_texture_data("nx.png"_H),
                        ctx.get_texture_data("py.png"_H),
                        ctx.get_texture_data("ny.png"_H),
                        ctx.get_texture_data("pz.png"_H),
                        ctx.get_texture_data("nz.png"_H)
                    },
                    sm::TextureFormat::Srgb8Alpha8
                );
            }
        }
    };

    switch (g.options.skybox) {
        case static_cast<int>(Skybox::None):
            return nullptr;
        case static_cast<int>(Skybox::Field):
            return load_or_reload("field"_H);
        case static_cast<int>(Skybox::Autumn):
            return load_or_reload("autumn"_H);
    }

    return {};
}
