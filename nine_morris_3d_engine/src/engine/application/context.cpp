#include <resmanager/resmanager.h>

#include "engine/application/context.h"
#include "engine/application/application.h"
#include "engine/application/events.h"
#include "engine/application/window.h"
#include "engine/audio/context.h"
#include "engine/graphics/opengl/framebuffer.h"
#include "engine/graphics/renderer/renderer.h"
#include "engine/graphics/renderer/gui_renderer.h"
#include "engine/other/resource_manager.h"
#include "engine/other/assert.h"
#include "engine/scene/scene.h"

struct ApplicationData;

void Ctx::change_scene(SceneId id) {
    for (std::unique_ptr<Scene>& scene : _application->scenes) {
        if (scene->id == id) {
            _application->to_scene = scene.get();
            _application->changed_scene = true;
            return;
        }
    }

    ASSERT(false, "Scene not found");
}

const Scene* Ctx::get_current_scene() {
    return _application->current_scene;
}

void Ctx::add_framebuffer(std::shared_ptr<gl::Framebuffer> framebuffer) {
    _application->framebuffers.push_back(framebuffer);
}

void Ctx::purge_framebuffers() {
    std::vector<size_t> indices;

    for (size_t i = 0; i < _application->framebuffers.size(); i++) {
        if (_application->framebuffers[i].expired()) {
            indices.push_back(i);
        }
    }

    for (size_t i = _application->framebuffers.size(); i > 0; i--) {
        const size_t I = i - 1;

        for (size_t index : indices) {
            if (index == I) {
                _application->framebuffers.erase(std::next(_application->framebuffers.begin(), index));
                break;
            }
        }
    }
}

const ApplicationData& Ctx::data() {
    return _application->app_data;
}

void Ctx::destroy_user_data() {
    _application->_user_data->reset();
}
