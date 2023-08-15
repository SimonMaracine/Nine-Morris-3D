#include <memory>

#include "engine/application_base/context.hpp"
#include "engine/application_base/application.hpp"
#include "engine/other/assert.hpp"
#include "engine/scene/scene.hpp"

namespace sm {
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
}
