#include "nine_morris_3d_engine/application/context.hpp"

#include <utility>
#include <cassert>

#include "nine_morris_3d_engine/application/application.hpp"
#include "nine_morris_3d_engine/application/scene.hpp"
#include "nine_morris_3d_engine/audio/openal/debug.hpp"
#include "nine_morris_3d_engine/graphics/opengl/debug.hpp"
#include "nine_morris_3d_engine/other/dependencies.hpp"

namespace sm {
    void Ctx::change_scene(Id id) {
        assert(application->next_scene == nullptr);

        for (const std::unique_ptr<ApplicationScene>& scene : application->scenes) {
            if (scene->id == id) {
                application->next_scene = scene.get();
                return;
            }
        }

        assert(false);
    }

    void Ctx::show_info_text() {
        std::string info_text;
        info_text += std::string(reinterpret_cast<const char*>(opengl_debug::get_opengl_version())) + '\n';
        info_text += std::string(reinterpret_cast<const char*>(opengl_debug::get_glsl_version())) + '\n';
        info_text += std::string(reinterpret_cast<const char*>(opengl_debug::get_renderer())) + '\n';
        info_text += std::string(reinterpret_cast<const char*>(opengl_debug::get_vendor())) + '\n';
        info_text += std::to_string(static_cast<int>(fps)) + " FPS ";
        info_text += std::to_string(static_cast<int>(delta * 1000.0f)) + " ms";

        Text text;
        text.font = rnd.storage.default_font;
        text.text = std::move(info_text);
        text.color = glm::vec3(1.0f);

        // Don't add it to the debug lists
        scn.add_text(const_cast<const Text&>(text));
    }

    std::string Ctx::get_information() {
        std::string result;

        result += '\n';
        result += openal_debug::get_information();
        result += opengl_debug::get_information();
        result += dependencies::get_information();

        return result;
    }
}
