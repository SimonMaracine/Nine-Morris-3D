#include "nine_morris_3d_engine/application/context.hpp"

#include <memory>
#include <cassert>

#include "nine_morris_3d_engine/application/application.hpp"
#include "nine_morris_3d_engine/application/scene.hpp"

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
