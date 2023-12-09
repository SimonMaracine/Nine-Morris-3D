#include <memory>

#include "engine/application_base/events.hpp"
#include "engine/application_base/window.hpp"
#include "engine/application_base/input.hpp"
#include "engine/application_base/tasks.hpp"
#include "engine/application_base/application_properties.hpp"
#include "engine/application_base/context.hpp"
#include "engine/application_base/application.hpp"
#include "engine/audio/context.hpp"
#include "engine/graphics/renderer.hpp"
#include "engine/graphics/identifier.hpp"
#include "engine/other/resource_manager.hpp"
#include "engine/other/random_gen.hpp"
#include "engine/other/encrypt.hpp"
#include "engine/other/assert.hpp"
#include "engine/scene/scene.hpp"


namespace sm {
    void Ctx::change_scene(SceneId id) {
        SM_ASSERT(application->next_scene == nullptr, "Must be null");

        for (std::unique_ptr<Scene>& scene : application->scenes) {
            if (scene->id == id) {
                application->next_scene = scene.get();
                return;
            }
        }

        SM_ASSERT(false, "Scene not found");
    }

    const Scene* Ctx::get_current_scene() const {
        return application->current_scene;
    }
}
