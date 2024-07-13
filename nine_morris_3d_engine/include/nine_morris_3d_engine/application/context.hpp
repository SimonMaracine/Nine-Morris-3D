#pragma once

#include <any>
#include <string>
#include <utility>
#include <initializer_list>

#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/events.hpp"
#include "nine_morris_3d_engine/application/window.hpp"
#include "nine_morris_3d_engine/application/tasks.hpp"
#include "nine_morris_3d_engine/application/properties.hpp"
#include "nine_morris_3d_engine/application/logging_base.hpp"
#include "nine_morris_3d_engine/application/file_system.hpp"
#include "nine_morris_3d_engine/application/input.hpp"
#include "nine_morris_3d_engine/application/id.hpp"
#include "nine_morris_3d_engine/audio/context.hpp"
#include "nine_morris_3d_engine/audio/music.hpp"
#include "nine_morris_3d_engine/graphics/opengl/vertex_array.hpp"
#include "nine_morris_3d_engine/graphics/opengl/texture.hpp"
#include "nine_morris_3d_engine/graphics/renderer.hpp"
#include "nine_morris_3d_engine/graphics/shader_library.hpp"
#include "nine_morris_3d_engine/graphics/scene.hpp"
#include "nine_morris_3d_engine/graphics/debug_ui.hpp"
#include "nine_morris_3d_engine/graphics/mesh.hpp"
#include "nine_morris_3d_engine/graphics/texture_data.hpp"
#include "nine_morris_3d_engine/other/resources_cache.hpp"
#include "nine_morris_3d_engine/other/random_gen.hpp"

namespace sm {
    class Application;

    using Model = std::pair<std::shared_ptr<Mesh>, std::shared_ptr<GlVertexArray>>;

    enum class MaterialType {
        Flat,
        Phong,
        PhongShadow,
        PhongDiffuse,
        PhongDiffuseShadow,
        PhongDiffuseNormalShadow,
    };

    // Wrapper around functionality exposed to the user
    class Ctx {
    public:
        explicit Ctx(const ApplicationProperties& properties)
            : fs(properties.application_name, properties.assets_directory), log(properties.log_file, fs),
            shd({"engine_assets", properties.assets_directory}), win(properties, &evt),
            rnd(properties.width, properties.height, properties.samples, fs, shd),
            snd(properties.audio), inp(win.get_handle()) {}

        ~Ctx() = default;

        Ctx(const Ctx&) = delete;
        Ctx& operator=(const Ctx&) = delete;
        Ctx(Ctx&&) = delete;
        Ctx& operator=(Ctx&&) = delete;

        void change_scene(Id id);
        void show_info_text();
        std::string get_information() const;
        Model load_model(
            const std::string& identifier,
            const std::string& file_path,
            const std::string& mesh_name,
            Mesh::Type type
        );
        Model load_model(const std::string& file_path, const std::string& mesh_name, Mesh::Type type);
        std::shared_ptr<GlTexture> load_texture(
            const std::string& file_path,
            const TexturePostProcessing& post_processing,
            const TextureSpecification& specification
        );
        std::shared_ptr<GlTextureCubemap> load_texture_cubemap(
            const std::string& identifier,
            std::initializer_list<std::string> file_paths,
            const TexturePostProcessing& post_processing
        );
        std::shared_ptr<Material> load_material(MaterialType type, unsigned int flags = 0);
        std::shared_ptr<Material> load_material(
            const std::string& identifier,
            const std::string& vertex_file_path,
            const std::string& fragment_file_path,
            MaterialType type,
            unsigned int flags = 0
        );

        template<typename T>
        T& global() {
            return std::any_cast<T&>(global_data);
        }

        template<typename T>
        T& user() {
            return *static_cast<T*>(user_data);
        }

        bool running {true};
        int exit_code {};
        float delta {};
        float fps {};

        // The order of these members matters

        FileSystem fs;
        Logging log;
        ShaderLibrary shd;
        EventDispatcher evt;  // Application events
        Window win;  // One of the last objects destroyed in an application instance
        Renderer rnd;  // Renderer for 3D, 2D and debug
        OpenAlContext snd;
        MusicPlayer mus;
        ResourcesCache res;  // Global cache of resources
        TaskManager tsk;
        RandomGenerator rng;
        Input inp;
        Scene scn;
    private:
#ifndef SM_BUILD_DISTRIBUTION
        DebugUi dbg;
#endif
        std::any global_data;
        void* user_data {nullptr};  // Arbitrary data defined by the user
        Application* application {nullptr};

        friend class Application;
    };
}
