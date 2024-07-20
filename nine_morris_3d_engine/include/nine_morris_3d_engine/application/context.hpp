#pragma once

#include <any>
#include <string>
#include <utility>
#include <initializer_list>

#include <resmanager/resmanager.hpp>

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
#include "nine_morris_3d_engine/graphics/opengl/shader.hpp"
#include "nine_morris_3d_engine/graphics/opengl/framebuffer.hpp"
#include "nine_morris_3d_engine/graphics/renderer.hpp"
#include "nine_morris_3d_engine/graphics/shader_library.hpp"
#include "nine_morris_3d_engine/graphics/scene.hpp"
#include "nine_morris_3d_engine/graphics/debug_ui.hpp"
#include "nine_morris_3d_engine/graphics/mesh.hpp"
#include "nine_morris_3d_engine/graphics/material.hpp"
#include "nine_morris_3d_engine/graphics/font.hpp"
#include "nine_morris_3d_engine/graphics/texture_data.hpp"
#include "nine_morris_3d_engine/other/resources_cache.hpp"

namespace sm {
    class Application;

    using Model = std::pair<std::shared_ptr<Mesh>, std::shared_ptr<GlVertexArray>>;

    enum class MaterialType {
        Flat,
        Phong,
        PhongShadow,
        PhongDiffuse,
        PhongDiffuseShadow,
        PhongDiffuseNormalShadow
    };

    // Wrapper around functionality exposed to the user
    class Ctx {
    private:
        explicit Ctx(const ApplicationProperties& properties);
    public:
        ~Ctx() = default;

        Ctx(const Ctx&) = delete;
        Ctx& operator=(const Ctx&) = delete;
        Ctx(Ctx&&) = delete;
        Ctx& operator=(Ctx&&) = delete;

        // File system
        static bool directory_exists(const std::string& path);
        static bool create_directory(const std::string& path);
        static bool delete_file(const std::string& path);
        static std::string current_working_directory();
        std::string path_logs() const;
        std::string path_saved_data() const;
        std::string path_assets() const;
        std::string path_engine_assets() const;
        std::string path_logs(const std::string& path) const;
        std::string path_saved_data(const std::string& path) const;
        std::string path_assets(const std::string& path) const;
        std::string path_engine_assets(const std::string& path) const;

        // Shader library
        std::string load_shader(const std::string& source) const;

        // Events
        template<typename E, auto F, typename... T>
        void connect(T&&... value_or_instance) { evt.connect<E, F>(value_or_instance...); }

        template<typename E, auto F, typename... T>
        void disconnect(T&&... value_or_instance) { evt.disconnect<E, F>(value_or_instance...); }

        template<typename T>
        void disconnect(T& value_or_instance) { evt.disconnect(value_or_instance); }

        template<typename T>
        void disconnect(T* value_or_instance) { evt.disconnect(value_or_instance); }

        template<typename E, typename... Args>
        void enqueue(Args&&... args) { evt.enqueue<E>(std::forward<Args>(args)...); }

        template<typename E>
        void clear() { evt.clear<E>(); }

        // Window
        const Monitors& get_monitors();
        int get_width() const;
        int get_height() const;
        void show() const;
        void set_vsync(int interval) const;
        void add_cursor(Id id, std::unique_ptr<TextureData>&& cursor, int x_hotspot, int y_hotspot);
        void set_cursor(Id id) const;
        void set_icons(std::initializer_list<std::unique_ptr<TextureData>> icons) const;
        static double get_time();

        // Renderer
        std::shared_ptr<Font> get_default_font() const;
        void set_color_correction(bool enable);
        void set_clear_color(glm::vec3 color);

        // OpenAL
        AlListener& get_listener();

        // Music player
        void play_music_track(std::shared_ptr<MusicTrack> music_track);
        void stop_music_track();
        void pause_music_track();
        void continue_music_track();
        void set_music_gain(float gain);

        // Task manager
        void add_task(Id id, const Task::TaskFunction& function);
        void remove_task(Id id);

        // Input
        bool is_key_pressed(Key key) const;
        bool is_mouse_button_pressed(MouseButton button) const;
        float get_mouse_x() const;
        float get_mouse_y() const;
        std::pair<float, float> get_mouse() const;

        // Scene
        void capture(const Camera& camera, glm::vec3 position);
        void capture(const Camera2D& camera_2d);
        void skybox(std::shared_ptr<GlTextureCubemap> texture);
        void shadow(ShadowBox& box);
        void add_post_processing(std::shared_ptr<PostProcessingStep> step);
        void add_renderable(Renderable& renderable);
        void add_light(DirectionalLight& light);
        void add_light(PointLight& light);
        void add_text(Text& text);
        void add_quad(Quad& quad);
        void debug_add_line(glm::vec3 position1, glm::vec3 position2, glm::vec3 color);
        void debug_add_lines(const std::vector<glm::vec3>& points, glm::vec3 color);
        void debug_add_lines(std::initializer_list<glm::vec3> points, glm::vec3 color);
        void debug_add_point(glm::vec3 position, glm::vec3 color);
        void debug_add_lamp(glm::vec3 position, glm::vec3 color);

        void change_scene(Id id);
        void show_info_text();
        std::string get_information() const;

        Model load_model(
            resmanager::HashedStr64 identifier,
            const std::string& file_path,
            const std::string& mesh_name,
            Mesh::Type type
        );

        Model load_model(
            const std::string& file_path,
            const std::string& mesh_name,
            Mesh::Type type
        );

        std::shared_ptr<GlTexture> load_texture(
            const std::string& file_path,
            const TexturePostProcessing& post_processing,
            const TextureSpecification& specification
        );

        std::shared_ptr<GlTextureCubemap> load_texture_cubemap(
            const char* identifier,
            std::initializer_list<std::string> file_paths,
            const TexturePostProcessing& post_processing,
            TextureFormat format
        );

        std::shared_ptr<Material> load_material(
            MaterialType type,
            unsigned int flags = 0
        );

        std::shared_ptr<Material> load_material(
            resmanager::HashedStr64 identifier,
            const std::string& vertex_file_path,
            const std::string& fragment_file_path,
            MaterialType type,
            unsigned int flags = 0
        );

        std::shared_ptr<MaterialInstance> load_material_instance(
            resmanager::HashedStr64 identifier,
            std::shared_ptr<Material> material
        );

        std::shared_ptr<GlShader> load_shader(
            resmanager::HashedStr64 identifier,
            const std::string& source_vertex,
            const std::string& source_fragment,
            bool include_processing = true
        );

        std::shared_ptr<GlFramebuffer> load_framebuffer(
            resmanager::HashedStr64 identifier,
            const FramebufferSpecification& specification
        );

        std::shared_ptr<Font> load_font(
            resmanager::HashedStr64 identifier,
            const std::string& file_path,
            const FontSpecification& specification
        );

        template<typename T>
        T& global() {
            return std::any_cast<T&>(global_data);
        }

        template<typename T>
        T& user() {
            return *static_cast<T*>(user_data);
        }

        float get_delta() const { return delta; }
        float get_fps() const { return fps; }

        bool running {true};
        int exit_code {};
    private:
        // The order of these members matters

        internal::FileSystem fs;
        internal::Logging log;
        internal::ShaderLibrary shd;
        internal::EventDispatcher evt;  // Application events
        internal::Window win;  // One of the last objects destroyed in an application instance
        internal::Renderer rnd;  // Renderer for 3D, 2D and debug
        internal::OpenAlContext snd;
        internal::MusicPlayer mus;
        internal::TaskManager tsk;
        internal::Input inp;
        internal::Scene scn;
        internal::ResourcesCache res;  // Global cache of resources
#ifndef SM_BUILD_DISTRIBUTION
        internal::DebugUi dbg;
#endif

        float delta {};
        float fps {};

        std::any global_data;
        void* user_data {nullptr};  // Arbitrary data defined by the user
        Application* application {nullptr};

        friend class Application;
        friend class internal::DebugUi;
    };
}
