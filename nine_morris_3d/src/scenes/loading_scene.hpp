#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

class LoadingScene : public sm::ApplicationScene {
public:
    explicit LoadingScene(sm::Ctx& ctx)
        : sm::ApplicationScene(ctx) {}

    SM_SCENE_NAME("loading")

    void on_start() override;
    void on_stop() override;
    void on_update() override;
private:
    void on_window_resized(const sm::WindowResizedEvent& event);

    void update_splash_screen();
    void load_splash_screen();
    void load_assets(sm::AsyncTask& task);

    bool m_done {false};
    sm::Camera2D m_camera_2d;
    std::shared_ptr<sm::GlTexture> m_splash_screen;
};
