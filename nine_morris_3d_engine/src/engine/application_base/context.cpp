#include <resmanager/resmanager.h>

#include "engine/application_base/context.h"
#include "engine/application_base/application.h"
#include "engine/application_base/events.h"
#include "engine/application_base/window.h"
#include "engine/audio/context.h"
#include "engine/graphics/opengl/framebuffer.h"
#include "engine/graphics/renderer/renderer.h"
#include "engine/graphics/renderer/gui_renderer.h"
#include "engine/other/resource_manager.h"
#include "engine/other/assert.h"
#include "engine/scene/scene.h"

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

        ASSERT(false, "Scene not found");
    }

    const Scene* Ctx::get_current_scene() {
        return application->current_scene;
    }

    void Ctx::add_framebuffer(std::shared_ptr<gl::Framebuffer> framebuffer) {
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
