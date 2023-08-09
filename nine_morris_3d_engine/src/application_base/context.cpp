#include <memory>
#include <cstddef>

#include <resmanager/resmanager.hpp>

#include "engine/application_base/context.hpp"
#include "engine/application_base/application.hpp"
#include "engine/application_base/events.hpp"
#include "engine/application_base/window.hpp"
#include "engine/audio/context.hpp"
#include "engine/graphics/opengl/framebuffer.hpp"
#include "engine/graphics/renderer/renderer.hpp"
#include "engine/graphics/renderer/gui_renderer.hpp"
#include "engine/other/resource_manager.hpp"
#include "engine/other/assert.hpp"
#include "engine/scene/scene.hpp"

namespace sm {
    struct ApplicationProperties;

    void Ctx::change_scene(SceneId id) {
        for (std::unique_ptr<Scene>& scene : application->scenes) {
            if (scene->id == id) {
                application->to_scene = scene.get();
                application->changed_scene = true;
                return;
            }
        }

        SM_ASSERT(false, "Scene not found");
    }

    const Scene* Ctx::get_current_scene() {
        return application->current_scene;
    }

    void Ctx::add_framebuffer(std::shared_ptr<GlFramebuffer> framebuffer) {
        application->framebuffers.push_back(framebuffer);
    }

    void Ctx::purge_framebuffers() {
        std::vector<size_t> indices;

        for (size_t i = 0; i < application->framebuffers.size(); i++) {
            if (application->framebuffers[i].expired()) {
                indices.push_back(i);
            }
        }

        for (size_t i = application->framebuffers.size(); i > 0; i--) {
            const size_t I = i - 1;

            for (size_t index : indices) {
                if (index == I) {
                    application->framebuffers.erase(std::next(application->framebuffers.begin(), index));
                    break;
                }
            }
        }
    }
}
