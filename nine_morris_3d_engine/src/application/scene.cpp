#include "nine_morris_3d_engine/application/scene.hpp"

namespace sm {
    void ApplicationScene::pre_update() {
        auto camera_controller {ctx.render_3d()->camera_controller};

        if (camera_controller == nullptr) {
            camera_controller = ctx.m_default_camera_controller;
        }

        camera_controller->update_controls(ctx.get_delta(), ctx);
        camera_controller->update_camera(ctx.get_delta());

        ctx.render_3d()->update_camera();
    }

    void ApplicationScene::post_update() {
        ctx.render_3d()->update_shadow_box();
    }
}
