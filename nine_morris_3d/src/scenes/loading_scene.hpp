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
    void update_loading_image();
    std::shared_ptr<sm::GlTexture> load_splash_screen();
    void load_assets(sm::AsyncTask& task);

    bool m_done {false};
    std::shared_ptr<sm::ImageNode> m_loading_image;
};
