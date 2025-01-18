#include "scenes/game_scene.hpp"

#include <nine_morris_3d_engine/external/resmanager.h++>

#include "global.hpp"
#include "muhle_engine.hpp"

void GameScene::on_start() {
    ctx.connect_event<sm::WindowResizedEvent, &GameScene::on_window_resized>(this);
    connect_events();

    ctx.set_renderer_clear_color(glm::vec3(0.1f, 0.1f, 0.1f));

    setup_camera();
    setup_skybox();
    setup_lights();

    m_ui.initialize(ctx);

    scene_setup();
}

void GameScene::on_stop() {
    m_cam_controller.disconnect_events(ctx);
    ctx.disconnect_events(this);
}

void GameScene::on_update() {
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

    m_clock.update();

    if (m_game_state != GameState::Ready && m_game_state != GameState::Over) {
        if (m_clock.get_white_time() == 0) {
            timeout(PlayerColor::White);
        }

        if (m_clock.get_black_time() == 0) {
            timeout(PlayerColor::Black);
        }
    }

    scene_update();
    update_game_state();

    // This needs to be called after all scene is updated
    ctx.shadow(m_shadow_box);
}

void GameScene::on_fixed_update() {
    m_cam_controller.update_friction();

    scene_fixed_update();
}

void GameScene::on_imgui_update() {
    m_ui.update(ctx, *this);

    scene_imgui_update();
}

void GameScene::load_and_set_skybox() {
    ctx.add_task_async([this](sm::AsyncTask& task, void*) {
        try {
            load_skybox_texture_data();
        } catch (const sm::RuntimeError&) {
            task.set_done(std::current_exception());
            return;
        }

        ctx.add_task([this](const sm::Task&, void*) {
            setup_skybox();
            setup_lights();
            m_ui.set_loading_skybox_done();

            return sm::Task::Result::Done;
        });

        task.set_done();
    });
}

void GameScene::load_and_set_textures() {
    ctx.add_task_async([this](sm::AsyncTask& task, void*) {
        try {
            load_all_texture_data();
        } catch (const sm::RuntimeError&) {
            task.set_done(std::current_exception());
            return;
        }

        ctx.add_task([this](const sm::Task&, void*) {
            setup_skybox();
            set_scene_textures();

            return sm::Task::Result::Done;
        });

        task.set_done();
    });
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
    switch (m_game_state) {
        case GameState::Ready:
            break;
        case GameState::Start:
            m_clock.start();
            m_game_state = GameState::NextTurn;

            break;
        case GameState::NextTurn: {
            switch (get_player_type()) {
                case GamePlayer::Human:
                    m_game_state = GameState::HumanThinking;
                    break;
                case GamePlayer::Computer:
                    m_game_state = GameState::ComputerStartThinking;
                    break;
            }

            break;
        }
        case GameState::HumanThinking:
            break;
        case GameState::ComputerStartThinking:
            // try {
            //     m_engine->start_thinking(
            //         board::position_to_string(m_board.get_setup_position()),
            //         m_moves,
            //         m_clock.get_white_time(),
            //         m_clock.get_black_time(),
            //         std::nullopt,
            //         std::nullopt
            //     );
            // } catch (const engine::EngineError& e) {
            //     std::cerr << "Engine error: " << e.what() << '\n';
            //     m_game_state = GameState::Stop;
            //     break;
            // }

            m_game_state = GameState::ComputerThinking;

            break;
        case GameState::ComputerThinking: {
            // std::optional<std::string> best_move;

            // try {
            //     best_move = m_engine->done_thinking();
            // } catch (const engine::EngineError& e) {
            //     std::cerr << "Engine error: " << e.what() << '\n';
            //     m_game_state = GameState::Stop;
            //     break;
            // }

            // if (best_move) {
            //     if (*best_move == "none") {
            //         if (m_board.get_game_over() == board::GameOver::None) {
            //             throw std::runtime_error("The engine calls game over, but the GUI doesn't agree");
            //         }

            //         break;
            //     }

            //     std::cout << "Playing move on the board " << *best_move << '\n';

            //     m_board.play_move(board::move_from_string(*best_move));
            // }

            break;
        }
        case GameState::Stop:
            m_clock.stop();
            m_game_state = GameState::Over;

            break;
        case GameState::Over:
            break;
    }
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
