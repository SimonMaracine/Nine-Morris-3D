#include "nine_morris_3d_engine/application/application.h"
#include "nine_morris_3d_engine/ecs/internal_systems.h"
#include "nine_morris_3d_engine/ecs/internal_components.h"

static void model_render_system_construct(entt::registry& registry, entt::entity entity) {
    auto [render_c, model_c] = registry.get<RenderComponent, ModelComponent>(entity);
    render_c.renderer->add_model(model_c.model, render_c.options);
};

static void model_render_system_destroy(entt::registry& registry, entt::entity entity) {
    auto [render_c, model_c] = registry.get<RenderComponent, ModelComponent>(entity);
    render_c.renderer->remove_model(model_c.model.handle);
};

static void quad_render_system_construct(entt::registry& registry, entt::entity entity) {
    auto [render_c, quad_c] = registry.get<RenderComponent, QuadComponent>(entity);
    render_c.renderer->add_quad(quad_c.quad);
};

static void quad_render_system_destroy(entt::registry& registry, entt::entity entity) {
    auto [render_c, quad_c] = registry.get<RenderComponent, QuadComponent>(entity);
    render_c.renderer->remove_quad(quad_c.quad.handle);
};

static void gui_image_system_construct(entt::registry& registry, entt::entity entity) {
    auto [gui_render_c, gui_image_c] = registry.get<GuiRenderComponent, GuiImageComponent>(entity);

    const auto* gui_offset_c = registry.try_get<GuiOffsetComponent>(entity);
    if (gui_offset_c != nullptr) {
        for (const auto [offset, relative] : gui_offset_c->offsets) {
            gui_image_c.image->offset(offset, relative);
        }   
    }

    const auto* gui_stick_c = registry.try_get<GuiStickComponent>(entity);
    if (gui_stick_c != nullptr) {
        gui_image_c.image->stick(gui_stick_c->sticky);
    }

    const auto* gui_scale_c = registry.try_get<GuiScaleComponent>(entity);
    if (gui_scale_c != nullptr) {
        gui_image_c.image->scale(
            gui_scale_c->min_scale,
            gui_scale_c->max_scale,
            gui_scale_c->min_bound,
            gui_scale_c->max_bound
        );
    }

    gui_render_c.gui_renderer->add_widget(gui_image_c.image);
};

static void gui_image_system_destroy(entt::registry& registry, entt::entity entity) {
    auto [gui_render_c, gui_image_c] = registry.get<GuiRenderComponent, GuiImageComponent>(entity);
    gui_render_c.gui_renderer->remove_widget(gui_image_c.image);
};

static void gui_text_system_construct(entt::registry& registry, entt::entity entity) {
    auto [gui_render_c, gui_text_c] = registry.get<GuiRenderComponent, GuiTextComponent>(entity);

    const auto* gui_offset_c = registry.try_get<GuiOffsetComponent>(entity);
    if (gui_offset_c != nullptr) {
        for (const auto [offset, relative] : gui_offset_c->offsets) {
            gui_text_c.text->offset(offset, relative);
        }   
    }

    const auto* gui_stick_c = registry.try_get<GuiStickComponent>(entity);
    if (gui_stick_c != nullptr) {
        gui_text_c.text->stick(gui_stick_c->sticky);
    }

    const auto* gui_scale_c = registry.try_get<GuiScaleComponent>(entity);
    if (gui_scale_c != nullptr) {
        gui_text_c.text->scale(
            gui_scale_c->min_scale,
            gui_scale_c->max_scale,
            gui_scale_c->min_bound,
            gui_scale_c->max_bound
        );
    }

    gui_render_c.gui_renderer->add_widget(gui_text_c.text);
};

static void gui_text_system_destroy(entt::registry& registry, entt::entity entity) {
    auto [gui_render_c, gui_text_c] = registry.get<GuiRenderComponent, GuiTextComponent>(entity);
    gui_render_c.gui_renderer->remove_widget(gui_text_c.text);
};

void CameraSystem::run() {
    auto view = registry.view<CameraComponent>();

    for (auto entity : view) {
        auto& camera_c = view.get<CameraComponent>(entity);
        camera_c.camera.update(
            app->get_mouse_wheel(),
            app->get_dx(),
            app->get_dy(),
            app->get_delta()
        );
    }
}

void CameraProjectionSystem::run() {
    auto view = registry.view<CameraComponent>();

    for (auto entity : view) {
        auto& camera_c = view.get<CameraComponent>(entity);
        camera_c.camera.update_projection(
            static_cast<float>(app->app_data.width), static_cast<float>(app->app_data.height)
        );
    }
}

void ModelRenderSystem::signal() {
    registry.on_construct<ModelComponent>().connect<&model_render_system_construct>();
    registry.on_destroy<ModelComponent>().connect<&model_render_system_destroy>();
}


void QuadRenderSystem::signal() {
    registry.on_construct<QuadComponent>().connect<&quad_render_system_construct>();
    registry.on_destroy<QuadComponent>().connect<&quad_render_system_destroy>();
}

void GuiImageSystem::signal() {
    registry.on_construct<GuiImageComponent>().connect<&gui_image_system_construct>();
    registry.on_destroy<GuiImageComponent>().connect<&gui_image_system_destroy>();
}

void GuiTextSystem::signal() {
    registry.on_construct<GuiTextComponent>().connect<&gui_text_system_construct>();
    registry.on_destroy<GuiTextComponent>().connect<&gui_text_system_destroy>();
}
