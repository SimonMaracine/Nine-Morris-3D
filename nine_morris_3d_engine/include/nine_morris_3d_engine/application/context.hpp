#pragma once

#include <string>
#include <utility>
#include <initializer_list>
#include <functional>
#include <filesystem>

#include "nine_morris_3d_engine/application/internal/error.hpp"
#include "nine_morris_3d_engine/application/internal/file_system.hpp"
#include "nine_morris_3d_engine/application/internal/input_codes.hpp"
#include "nine_morris_3d_engine/application/internal/logging_base.hpp"
#include "nine_morris_3d_engine/application/internal/event_dispatcher.hpp"
#include "nine_morris_3d_engine/application/internal/window.hpp"
#include "nine_morris_3d_engine/application/internal/task_manager.hpp"
#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/properties.hpp"
#include "nine_morris_3d_engine/application/id.hpp"
#include "nine_morris_3d_engine/application/global_data.hpp"
#include "nine_morris_3d_engine/audio/internal/audio.hpp"
#include "nine_morris_3d_engine/audio/sound_data.hpp"
#include "nine_morris_3d_engine/graphics/internal/shader_library.hpp"
#include "nine_morris_3d_engine/graphics/internal/renderer.hpp"
#include "nine_morris_3d_engine/graphics/internal/debug_ui.hpp"
#include "nine_morris_3d_engine/graphics/opengl/vertex_array.hpp"
#include "nine_morris_3d_engine/graphics/opengl/texture.hpp"
#include "nine_morris_3d_engine/graphics/opengl/shader.hpp"
#include "nine_morris_3d_engine/graphics/opengl/framebuffer.hpp"
#include "nine_morris_3d_engine/graphics/scene.hpp"
#include "nine_morris_3d_engine/graphics/mesh.hpp"
#include "nine_morris_3d_engine/graphics/material.hpp"
#include "nine_morris_3d_engine/graphics/font.hpp"
#include "nine_morris_3d_engine/graphics/texture_data.hpp"
#include "nine_morris_3d_engine/other/internal/resources_cache.hpp"
#include "nine_morris_3d_engine/other/internal/default_camera_controller.hpp"

namespace sm {
    class Application;
    class ApplicationScene;

    enum class MaterialType {
        Flat,
        Phong,
        PhongShadow,
        PhongDiffuse,
        PhongDiffuseShadow,
        PhongDiffuseNormalShadow
    };

    using RuntimeError = internal::RuntimeError;
    using ApplicationError = internal::ApplicationError;
    using RendererSpecification = internal::RendererSpecification;
    using Key = internal::Key;
    using Button = internal::Button;

    // Wrapper around functionality exposed to the user, i.e. the API
    // Scenes contain references to the context instance
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
        static bool file_exists(const std::filesystem::path& path);
        static bool is_directory(const std::filesystem::path& path);
        static bool create_directory(const std::filesystem::path& path);
        static bool delete_file(const std::filesystem::path& path);
        static std::filesystem::path current_working_directory();
        std::filesystem::path path_logs() const;
        std::filesystem::path path_saved_data() const;
        std::filesystem::path path_assets() const;
        std::filesystem::path path_engine_assets() const;
        std::filesystem::path path_logs(const std::filesystem::path& path) const;
        std::filesystem::path path_saved_data(const std::filesystem::path& path) const;
        std::filesystem::path path_assets(const std::filesystem::path& path) const;
        std::filesystem::path path_engine_assets(const std::filesystem::path& path) const;

        // Events
        template<typename E, auto F, typename... T>
        void connect_event(T&&... value_or_instance) { m_evt.connect<E, F>(value_or_instance...); }

        template<typename E, auto F, typename... T>
        void disconnect_event(T&&... value_or_instance) { m_evt.disconnect<E, F>(value_or_instance...); }

        template<typename T>
        void disconnect_events(T& value_or_instance) { m_evt.disconnect(value_or_instance); }

        template<typename T>
        void disconnect_events(T* value_or_instance) { m_evt.disconnect(value_or_instance); }

        template<typename E, typename... Args>
        void enqueue_event(Args&&... args) { m_evt.enqueue<E>(std::forward<Args>(args)...); }

        template<typename E>
        void clear_events() { m_evt.clear<E>(); }

        // Window
        int get_window_width() const;
        int get_window_height() const;
        void show_window() const;
        void set_window_vsync(bool enable) const;
        void set_window_icons(std::initializer_list<std::unique_ptr<TextureData>> icons);
        void set_window_size(int width, int height);
        static double get_time();

        // Renderer
        std::shared_ptr<Font> get_renderer_default_font() const;
        void set_renderer_color_correction(bool enable);
        bool get_renderer_color_correction() const;
        void set_renderer_clear_color(glm::vec3 color);
        void set_renderer_samples(int samples);
        void set_renderer_scale(int scale);
        void set_renderer_shadow_map_size(int size);
        void initialize_renderer(const RendererSpecification& specification);

        // Audio
        static void play_audio_sound(std::shared_ptr<SoundData> sound_data);
        static void set_audio_volume(float volume);

        // Task manager
        void add_task_immediate(Task::Function&& function);
        void add_task_delayed(Task::Function&& function, double delay);
        void add_task_deffered(Task::Function&& function);
        void add_task_async(AsyncTask::Function&& function);

        // Input
        static bool is_key_pressed(Key key);
        static bool is_button_pressed(Button button);
        static std::pair<float, float> get_mouse_position();

        // Scene
        std::shared_ptr<RootNode3D> render_3d() const { return m_scn.root_node_3d; }
        std::shared_ptr<RootNode2D> render_2d() const { return m_scn.root_node_2d; }

        // Dear ImGui
        static void invalidate_dear_imgui_texture();

        // Context
        void change_scene(Id id, bool clear_resources = false);
        void show_information_text();
        float get_delta() const;
        float get_fps() const;
        std::string get_information() const;
        std::shared_ptr<Mesh> get_mesh(Id id) const;
        std::shared_ptr<Mesh> load_mesh(Id id, const std::filesystem::path& file_path, const std::string& mesh_name, Mesh::Type type, bool flip_winding = false);
        std::shared_ptr<Mesh> load_mesh(const std::filesystem::path& file_path, const std::string& mesh_name, Mesh::Type type, bool flip_winding = false);
        std::shared_ptr<GlVertexArray> load_vertex_array(Id id, std::shared_ptr<Mesh> mesh);
        std::shared_ptr<TextureData> get_texture_data(Id id) const;
        std::shared_ptr<TextureData> load_texture_data(const std::filesystem::path& file_path, const TexturePostProcessing& post_processing);
        std::shared_ptr<TextureData> reload_texture_data(const std::filesystem::path& file_path, const TexturePostProcessing& post_processing);
        std::shared_ptr<GlTexture> load_texture(Id id, std::shared_ptr<TextureData> texture_data, const TextureSpecification& specification);
        std::shared_ptr<GlTexture> reload_texture(Id id, std::shared_ptr<TextureData> texture_data, const TextureSpecification& specification);
        std::shared_ptr<GlTextureCubemap> load_texture_cubemap(Id id, std::initializer_list<std::shared_ptr<TextureData>> texture_data, TextureFormat format);
        std::shared_ptr<GlTextureCubemap> reload_texture_cubemap(Id id, std::initializer_list<std::shared_ptr<TextureData>> texture_data, TextureFormat format);
        std::shared_ptr<Material> load_material(MaterialType type);
        std::shared_ptr<Material> load_material(Id id, const std::filesystem::path& vertex_file_path, const std::filesystem::path& fragment_file_path, MaterialType type);
        std::shared_ptr<MaterialInstance> load_material_instance(Id id, std::shared_ptr<Material> material);
        std::shared_ptr<GlShader> load_shader(Id id, const std::filesystem::path& vertex_file_path, const std::filesystem::path& fragment_file_path);
        std::shared_ptr<GlFramebuffer> load_framebuffer(Id id, const FramebufferSpecification& specification);
        std::shared_ptr<Font> load_font(Id id, const std::filesystem::path& file_path, const FontSpecification& specification, const std::function<void(Font*)>& bake);
        std::shared_ptr<SoundData> load_sound_data(const std::filesystem::path& file_path);

        // Retrieve shared data
        template<typename T>
        T& global() {
            return *static_cast<T*>(m_global_data.get());
        }

        // Retrieve shared data
        template<typename T>
        const T& global() const {
            return *static_cast<T*>(m_global_data.get());
        }

        // Retrieve user data
        template<typename T>
        T& user() {
            return *static_cast<T*>(m_user_data);
        }

        // Retrieve user data
        template<typename T>
        const T& user() const {
            return *static_cast<T*>(m_user_data);
        }

        // Read/write flag
        // Set to false to close the application
        bool running {true};

        // Read/write code
        // Returned by application run()
        int exit_code {};
    private:
        const char* m_build_date {};
        const char* m_build_time {};

        // The order of these members matters

        internal::FileSystem m_fs;
        internal::Logging m_log;
        internal::ShaderLibrary m_shd;
        internal::EventDispatcher m_evt;
        internal::Window m_win;
        internal::Renderer m_rnd;
        internal::TaskManager m_tsk;
        internal::ResourcesCache m_res;
#ifndef SM_BUILD_DISTRIBUTION
        internal::DebugUi m_dbg;
#endif
        Scene m_scn;

        float m_delta {};
        float m_fps {};

        std::unique_ptr<GlobalData> m_global_data;  // Arbitrary data defined by the user
        void* m_user_data {};  // Arbitrary pointer to data defined by the user
        Application* m_application {};  // Context needs to know about the application

        std::shared_ptr<internal::DefaultCameraController> m_default_camera_controller;
        std::shared_ptr<TextNode> m_information_text;

        friend class Application;
        friend class ApplicationScene;
        friend class internal::DebugUi;
    };
}
