#include "engine/application_base/context.hpp"

#include <cassert>

#include "engine/application_base/application.hpp"
#include "engine/scene/scene.hpp"

namespace sm {
    void Ctx::change_scene(Id id) {
        assert(application->next_scene == nullptr);

        for (const std::unique_ptr<Scene>& scene : application->scenes) {
            if (scene->id == id) {
                application->next_scene = scene.get();
                return;
            }
        }

        assert(false);
    }
}
