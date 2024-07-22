#pragma once

#include <any>
#include <string>
#include <utility>
#include <initializer_list>

#include <resmanager/resmanager.hpp>

#include "nine_morris_3d_engine/application/internal/file_system.hpp"
#include "nine_morris_3d_engine/application/internal/logging_base.hpp"
#include "nine_morris_3d_engine/application/internal/event_dispatcher.hpp"
#include "nine_morris_3d_engine/application/internal/window.hpp"
#include "nine_morris_3d_engine/application/internal/task_manager.hpp"
#include "nine_morris_3d_engine/application/internal/input.hpp"
#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/properties.hpp"
#include "nine_morris_3d_engine/application/id.hpp"
#include "nine_morris_3d_engine/audio/internal/context.hpp"
#include "nine_morris_3d_engine/audio/internal/music_player.hpp"
#include "nine_morris_3d_engine/graphics/internal/shader_library.hpp"
#include "nine_morris_3d_engine/graphics/internal/renderer.hpp"
#include "nine_morris_3d_engine/graphics/internal/scene.hpp"
#include "nine_morris_3d_engine/graphics/internal/debug_ui.hpp"
#include "nine_morris_3d_engine/graphics/opengl/vertex_array.hpp"
#include "nine_morris_3d_engine/graphics/opengl/texture.hpp"
#include "nine_morris_3d_engine/graphics/opengl/shader.hpp"
#include "nine_morris_3d_engine/graphics/opengl/framebuffer.hpp"
#include "nine_morris_3d_engine/graphics/mesh.hpp"
#include "nine_morris_3d_engine/graphics/material.hpp"
#include "nine_morris_3d_engine/graphics/font.hpp"
#include "nine_morris_3d_engine/graphics/texture_data.hpp"
#include "nine_morris_3d_engine/other/internal/resources_cache.hpp"

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
        void connect_event(T&&... value_or_instance) { evt.connect<E, F>(value_or_instance...); }

        template<typename E, auto F, typename... T>
        void disconnect_event(T&&... value_or_instance) { evt.disconnect<E, F>(value_or_instance...); }

        template<typename T>
        void disconnect_event(T& value_or_instance) { evt.disconnect(value_or_instance); }

        template<typename T>
        void disconnect_event(T* value_or_instance) { evt.disconnect(value_or_instance); }

        template<typename E, typename... Args>
        void enqueue_event(Args&&... args) { evt.enqueue<E>(std::forward<Args>(args)...); }

        template<typename E>
        void clear_events() { evt.clear<E>(); }

        // Window
        int get_window_width() const;
        int get_window_height() const;
        void show_window() const;
        void set_window_vsync(int interval) const;
        void add_window_cursor(Id id, std::unique_ptr<TextureData>&& cursor, int x_hotspot, int y_hotspot);
        void set_window_cursor(Id id) const;
        void set_window_icons(std::initializer_list<std::unique_ptr<TextureData>> icons) const;
        const Monitors& get_monitors();
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
        std::pair<float, float> get_mouse_position() const;

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
        void debug_add_lines(const std::vector<glm::vec3>& positions, glm::vec3 color);
        void debug_add_lines(std::initializer_list<glm::vec3> positions, glm::vec3 color);
        void debug_add_point(glm::vec3 position, glm::vec3 color);
        void debug_add_lamp(glm::vec3 position, glm::vec3 color);

        // Context
        void change_scene(Id id);
        void show_info_text();
        std::string get_information() const;

        Model load_model(Id id, const std::string& file_path, const std::string& mesh_name, Mesh::Type type);
        Model load_model(const std::string& file_path, const std::string& mesh_name, Mesh::Type type);

        std::shared_ptr<TextureData> load_texture_data(const std::string& file_path, const TexturePostProcessing& post_processing);

        std::shared_ptr<GlTexture> load_texture(Id id, std::shared_ptr<TextureData> texture_data, const TextureSpecification& specification);
        std::shared_ptr<GlTexture> reload_texture(Id id, std::shared_ptr<TextureData> texture_data, const TextureSpecification& specification);

        std::shared_ptr<GlTextureCubemap> load_texture_cubemap(Id id, std::initializer_list<std::shared_ptr<TextureData>> texture_data, TextureFormat format);
        std::shared_ptr<GlTextureCubemap> reload_texture_cubemap(Id id, std::initializer_list<std::shared_ptr<TextureData>> texture_data, TextureFormat format);

        std::shared_ptr<Material> load_material(MaterialType type, unsigned int flags = 0);
        std::shared_ptr<Material> load_material(Id id, const std::string& vertex_file_path, const std::string& fragment_file_path, MaterialType type, unsigned int flags = 0);

        std::shared_ptr<MaterialInstance> load_material_instance(Id id, std::shared_ptr<Material> material);

        std::shared_ptr<GlShader> load_shader(Id id, const std::string& source_vertex, const std::string& source_fragment, bool include_processing = true);

        std::shared_ptr<GlFramebuffer> load_framebuffer(Id id, const FramebufferSpecification& specification);

        std::shared_ptr<Font> load_font(Id id, const std::string& file_path, const FontSpecification& specification);

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
        internal::EventDispatcher evt;
        internal::Window win;
        internal::Renderer rnd;
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
