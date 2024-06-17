#include "engine/application_base/context.hpp"

#include <cassert>

#include "engine/application_base/application.hpp"

namespace sm {
    void Ctx::change_scene(SceneId id) {
        assert(application->next_scene == nullptr);

        for (std::unique_ptr<Scene>& scene : application->scenes) {
            if (scene->id == id) {
                application->next_scene = scene.get();
                return;
            }
        }

        assert(false);
    }

    const Scene* Ctx::get_current_scene() const {
        return application->current_scene;
    }
}
