#include "nine_morris_3d_engine/application/context.hpp"

#include <resmanager/resmanager.hpp>

#include "nine_morris_3d_engine/application/application.hpp"
#include "nine_morris_3d_engine/application/scene.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/audio/openal/debug.hpp"
#include "nine_morris_3d_engine/graphics/internal/imgui_context.hpp"
#include "nine_morris_3d_engine/graphics/opengl/debug.hpp"
#include "nine_morris_3d_engine/other/dependencies.hpp"
#include "nine_morris_3d_engine/other/utilities.hpp"

namespace sm {
    Ctx::Ctx(const ApplicationProperties& properties)
        : m_fs(properties.path_logs, properties.path_saved_data, properties.path_assets, properties.assets_directory),
        m_log(properties.log_file, m_fs),
        m_shd({m_fs.path_engine_assets().string(), m_fs.path_assets().string()}),
        m_win(properties, m_evt),
        m_rnd(properties.width, properties.height, m_fs, m_shd),
        m_snd(properties.audio),
        m_mus(properties.audio),
        m_inp(m_win.get_handle()) {
        if (properties.default_renderer_parameters) {
            m_rnd.initialize(properties.width, properties.height, m_fs);
        }

        if (!m_fs.get_error_string().empty()) {
            LOG_DIST_ERROR("{}", m_fs.get_error_string());
        }
    }

    bool Ctx::file_exists(const std::string& path) {
        return internal::FileSystem::file_exists(path);
    }

    bool Ctx::is_directory(const std::string& path) {
        return internal::FileSystem::is_directory(path);
    }

    bool Ctx::create_directory(const std::string& path) {
        return internal::FileSystem::create_directory(path);
    }

    bool Ctx::delete_file(const std::string& path) {
        return internal::FileSystem::delete_file(path);
    }

    std::filesystem::path Ctx::current_working_directory() {
        return internal::FileSystem::current_working_directory();
    }

    std::filesystem::path Ctx::path_logs() const {
        return m_fs.path_logs();
    }

    std::filesystem::path Ctx::path_saved_data() const {
        return m_fs.path_saved_data();
    }

    std::filesystem::path Ctx::path_assets() const {
        return m_fs.path_assets();
    }

    std::filesystem::path Ctx::path_engine_assets() const {
        return m_fs.path_engine_assets();
    }

    std::filesystem::path Ctx::path_logs(const std::string& path) const {
        return m_fs.path_logs(path);
    }

    std::filesystem::path Ctx::path_saved_data(const std::string& path) const {
        return m_fs.path_saved_data(path);
    }

    std::filesystem::path Ctx::path_assets(const std::string& path) const {
        return m_fs.path_assets(path);
    }

    std::filesystem::path Ctx::path_engine_assets(const std::string& path) const {
        return m_fs.path_engine_assets(path);
    }

    int Ctx::get_window_width() const noexcept {
        return m_win.get_width();
    }

    int Ctx::get_window_height() const noexcept {
        return m_win.get_height();
    }

    void Ctx::show_window() const noexcept {
        m_win.show();
    }

    void Ctx::set_window_vsync(bool enable) const noexcept {
        m_win.set_vsync(enable);
    }

    void Ctx::add_window_cursor(Id id, std::unique_ptr<TextureData>&& cursor, int x_hotspot, int y_hotspot) {
        m_win.add_cursor(id, std::move(cursor), x_hotspot, y_hotspot);
    }

    void Ctx::set_window_cursor(Id id) const {
        m_win.set_cursor(id);
    }

    void Ctx::set_window_icons(std::initializer_list<std::unique_ptr<TextureData>> icons) const {
        m_win.set_icons(icons);
    }

    void Ctx::set_window_dimensions(int width, int height) noexcept {
        m_win.set_dimensions(width, height);
    }

    Monitors Ctx::get_monitors() const {
        return m_win.get_monitors();
    }

    double Ctx::get_time() noexcept {
        return internal::Window::get_time();
    }

    std::shared_ptr<Font> Ctx::get_renderer_default_font() const noexcept {
        return m_rnd.get_default_font();
    }

    void Ctx::set_renderer_color_correction(bool enable) noexcept {
        m_rnd.set_color_correction(enable);
    }

    bool Ctx::get_renderer_color_correction() const noexcept {
        return m_rnd.get_color_correction();
    }

    void Ctx::set_renderer_clear_color(glm::vec3 color) noexcept {
        m_rnd.set_clear_color(color);
    }

    void Ctx::set_renderer_samples(int samples) {
        m_rnd.set_samples(m_win.get_width(), m_win.get_height(), samples);
    }

    void Ctx::set_renderer_scale(int scale) {
        m_rnd.set_scale(m_fs, scale);
    }

    void Ctx::set_renderer_shadow_map_size(int size) {
        m_rnd.set_shadow_map_size(size);
    }

    void Ctx::initialize_renderer(const RendererSpecification& specification) {
        m_rnd.initialize(m_win.get_width(), m_win.get_height(), m_fs, specification);
    }

    void Ctx::play_music_track(std::shared_ptr<MusicTrack> music_track) {
        m_mus.play(music_track);
    }

    void Ctx::stop_music_track() noexcept {
        m_mus.stop();
    }

    void Ctx::pause_music_track() noexcept {
        m_mus.pause();
    }

    void Ctx::resume_music_track() noexcept {
        m_mus.resume();
    }

    void Ctx::set_music_gain(float gain) noexcept {
        m_mus.set_gain(gain);
    }

    void Ctx::add_task_immediate(Task::Function&& function) {
        m_tsk.add_immediate(std::move(function));
    }

    void Ctx::add_task_delayed(Task::Function&& function, double delay) {
        m_tsk.add_delayed(std::move(function), delay);
    }

    void Ctx::add_task_deffered(Task::Function&& function) {
        m_tsk.add_deffered(std::move(function));
    }

    void Ctx::add_task_async(AsyncTask::Function&& function) {
        m_tsk.add_async(std::move(function));
    }

    bool Ctx::is_key_pressed(Key key) const noexcept {
        return m_inp.is_key_pressed(key);
    }

    bool Ctx::is_mouse_button_pressed(MouseButton button) const noexcept {
        return m_inp.is_mouse_button_pressed(button);
    }

    std::pair<float, float> Ctx::get_mouse_position() const noexcept {
        return m_inp.get_mouse_position();
    }

    void Ctx::capture(const Camera& camera, glm::vec3 position) noexcept {
        m_scn.capture(camera, position);
    }

    void Ctx::capture(const Camera2D& camera_2d) noexcept {
        m_scn.capture(camera_2d);
    }

    void Ctx::environment(const Skybox& skybox) noexcept {
        m_scn.environment(skybox);
    }

    void Ctx::shadow(ShadowBox& box) noexcept {
        m_scn.shadow(box);
    }

    void Ctx::add_post_processing(std::shared_ptr<PostProcessingStep> step) {
        m_scn.add_post_processing(step);
    }

    void Ctx::add_renderable(Renderable& renderable) {
        m_scn.add_renderable(renderable);
    }

    void Ctx::add_light(DirectionalLight& light) noexcept {
        m_scn.add_light(light);
    }

    void Ctx::add_light(PointLight& light) {
        m_scn.add_light(light);
    }

    void Ctx::add_text(Text& text) {
        m_scn.add_text(text);
    }

    void Ctx::add_quad(Quad& quad) {
        m_scn.add_quad(quad);
    }

    void Ctx::debug_add_line(glm::vec3 position1, glm::vec3 position2, glm::vec3 color) {
        m_scn.debug_add_line(position1, position2, color);
    }

    void Ctx::debug_add_lines(const std::vector<glm::vec3>& positions, glm::vec3 color) {
        m_scn.debug_add_lines(positions, color);
    }

    void Ctx::debug_add_lines(std::initializer_list<glm::vec3> positions, glm::vec3 color) {
        m_scn.debug_add_lines(positions, color);
    }

    void Ctx::debug_add_point(glm::vec3 position, glm::vec3 color) {
        m_scn.debug_add_point(position, color);
    }

    void Ctx::debug_add_lamp(glm::vec3 position, glm::vec3 color) {
        m_scn.debug_add_lamp(position, color);
    }

    void Ctx::invalidate_dear_imgui_texture() noexcept {
        internal::imgui_context::invalidate_texture();
    }

    void Ctx::change_scene(Id id, bool clear_resources) noexcept {
        m_application->change_scene(id, clear_resources);
    }

    void Ctx::show_information_text() {
        std::string information_text;
        information_text += std::string(reinterpret_cast<const char*>(opengl_debug::get_opengl_version())) + '\n';
        information_text += std::string(reinterpret_cast<const char*>(opengl_debug::get_glsl_version())) + '\n';
        information_text += std::string(reinterpret_cast<const char*>(opengl_debug::get_renderer())) + '\n';
        information_text += std::string(reinterpret_cast<const char*>(opengl_debug::get_vendor())) + '\n';
        information_text += std::to_string(static_cast<int>(m_fps)) + " FPS ";
        information_text += std::to_string(static_cast<int>(m_delta * 1000.0f)) + " ms\n";
        information_text += std::string("build ") + __DATE__ + " " + __TIME__;
#ifndef SM_BUILD_DISTRIBUTION
        information_text += " dev";
#endif

        Text text;
        text.font = m_rnd.get_default_font();
        text.text = std::move(information_text);
        text.color = glm::vec3(1.0f);

        // Don't add it to the debug lists
        m_scn.add_text(const_cast<const Text&>(text));
    }

    float Ctx::get_delta() const noexcept {
        return m_delta;
    }

    float Ctx::get_fps() const noexcept {
        return m_fps;
    }

    std::string Ctx::get_information() {
        std::string result;
        result += '\n';
        result += openal_debug::get_information();
        result += opengl_debug::get_information();
        result += dependencies::get_information();

        return result;
    }

    std::shared_ptr<Mesh> Ctx::get_mesh(Id id) const {
        return m_res.mesh->get(id);
    }

    std::shared_ptr<Mesh> Ctx::load_mesh(Id id, const std::filesystem::path& file_path, const std::string& mesh_name, Mesh::Type type, bool flip_winding) {
        if (m_res.mesh->contains(id)) {
            return m_res.mesh->get(id);
        }

        return m_res.mesh->force_load(id, utils::read_file(file_path), mesh_name, type, flip_winding);
    }

    std::shared_ptr<Mesh> Ctx::load_mesh(const std::filesystem::path& file_path, const std::string& mesh_name, Mesh::Type type, bool flip_winding) {
        const auto id {Id(utils::file_name(file_path))};

        if (m_res.mesh->contains(id)) {
            return m_res.mesh->get(id);
        }

        return m_res.mesh->force_load(id, utils::read_file(file_path), mesh_name, type, flip_winding);
    }

    std::shared_ptr<GlVertexArray> Ctx::load_vertex_array(Id id, std::shared_ptr<Mesh> mesh) {
        const auto vertex_buffer {m_res.vertex_buffer->load(id, mesh->get_vertices(), mesh->get_vertices_size())};
        const auto index_buffer {m_res.index_buffer->load(id, mesh->get_indices(), mesh->get_indices_size())};

        const auto [vertex_array, present] {m_res.vertex_array->load_check(id)};

        if (present) {
            return vertex_array;
        }

        vertex_array->configure([=](GlVertexArray* va) {
            VertexBufferLayout layout;

            switch (mesh->get_type()) {
                case Mesh::Type::P:
                    layout.add(0, VertexBufferLayout::Float, 3);
                    break;
                case Mesh::Type::PN:
                    layout.add(0, VertexBufferLayout::Float, 3);
                    layout.add(1, VertexBufferLayout::Float, 3);
                    break;
                case Mesh::Type::PNT:
                    layout.add(0, VertexBufferLayout::Float, 3);
                    layout.add(1, VertexBufferLayout::Float, 3);
                    layout.add(2, VertexBufferLayout::Float, 2);
                    break;
                case Mesh::Type::PNTT:
                    layout.add(0, VertexBufferLayout::Float, 3);
                    layout.add(1, VertexBufferLayout::Float, 3);
                    layout.add(2, VertexBufferLayout::Float, 2);
                    layout.add(3, VertexBufferLayout::Float, 3);
                    break;
            }

            va->add_vertex_buffer(vertex_buffer, layout);
            va->add_index_buffer(index_buffer);
        });

        return vertex_array;
    }

    std::shared_ptr<TextureData> Ctx::get_texture_data(Id id) const {
        return m_res.texture_data->get(id);
    }

    std::shared_ptr<TextureData> Ctx::load_texture_data(const std::filesystem::path& file_path, const TexturePostProcessing& post_processing) {
        const auto id {Id(utils::file_name(file_path))};

        if (m_res.texture_data->contains(id)) {
            return m_res.texture_data->get(id);
        }

        return m_res.texture_data->force_load(id, utils::read_file(file_path), post_processing);
    }

    std::shared_ptr<TextureData> Ctx::reload_texture_data(const std::filesystem::path& file_path, const TexturePostProcessing& post_processing) {
        const auto id {Id(utils::file_name(file_path))};

        return m_res.texture_data->force_load(id, utils::read_file(file_path), post_processing);
    }

    std::shared_ptr<GlTexture> Ctx::load_texture(Id id, std::shared_ptr<TextureData> texture_data, const TextureSpecification& specification) {
        return m_res.texture->load(id, texture_data, specification);
    }

    std::shared_ptr<GlTexture> Ctx::reload_texture(Id id, std::shared_ptr<TextureData> texture_data, const TextureSpecification& specification) {
        return m_res.texture->force_load(id, texture_data, specification);
    }

    std::shared_ptr<GlTextureCubemap> Ctx::load_texture_cubemap(Id id, std::initializer_list<std::shared_ptr<TextureData>> texture_data, TextureFormat format) {
        return m_res.texture_cubemap->load(id, texture_data, format);
    }

    std::shared_ptr<GlTextureCubemap> Ctx::reload_texture_cubemap(Id id, std::initializer_list<std::shared_ptr<TextureData>> texture_data, TextureFormat format) {
        return m_res.texture_cubemap->force_load(id, texture_data, format);
    }

    std::shared_ptr<Material> Ctx::load_material(MaterialType type, unsigned int flags) {
        using namespace resmanager::literals;

        switch (type) {
            case MaterialType::Flat: {
                const auto id {"flat"_H};

                const auto shader {load_shader(
                    id,
                    m_fs.path_engine_assets("shaders/flat.vert"),
                    m_fs.path_engine_assets("shaders/flat.frag")
                )};

                const auto [material, present] {m_res.material->load_check(id, shader, flags)};

                if (present) {
                    return material;
                }

                material->add_uniform(Material::Uniform::Vec3, "u_material.color"_H);

                return material;
            }
            case MaterialType::Phong: {
                const auto id {"phong"_H};

                const auto shader {load_shader(
                    id,
                    m_fs.path_engine_assets("shaders/phong.vert"),
                    m_fs.path_engine_assets("shaders/phong.frag")
                )};

                const auto [material, present] {m_res.material->load_check(id, shader, flags)};

                if (present) {
                    return material;
                }

                material->add_uniform(Material::Uniform::Vec3, "u_material.ambient_diffuse"_H);
                material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
                material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);

                return material;
            }
            case MaterialType::PhongShadow: {
                const auto id {"phong_shadow"_H};

                const auto shader {load_shader(
                    id,
                    m_fs.path_engine_assets("shaders/phong_shadow.vert"),
                    m_fs.path_engine_assets("shaders/phong_shadow.frag")
                )};

                const auto [material, present] {m_res.material->load_check(id, shader, flags)};

                if (present) {
                    return material;
                }

                material->add_uniform(Material::Uniform::Vec3, "u_material.ambient_diffuse"_H);
                material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
                material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);

                return material;
            }
            case MaterialType::PhongDiffuse: {
                const auto id {"phong_diffuse"_H};

                const auto shader {load_shader(
                    id,
                    m_fs.path_engine_assets("shaders/phong_diffuse.vert"),
                    m_fs.path_engine_assets("shaders/phong_diffuse.frag")
                )};

                const auto [material, present] {m_res.material->load_check(id, shader, flags)};

                if (present) {
                    return material;
                }

                material->add_uniform(Material::Uniform::Vec3, "u_material.ambient_diffuse"_H);
                material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
                material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);

                return material;
            }
            case MaterialType::PhongDiffuseShadow: {
                const auto id {"phong_diffuse_shadow"_H};

                const auto shader {load_shader(
                    id,
                    m_fs.path_engine_assets("shaders/phong_diffuse_shadow.vert"),
                    m_fs.path_engine_assets("shaders/phong_diffuse_shadow.frag")
                )};

                const auto [material, present] {m_res.material->load_check(id, shader, flags)};

                if (present) {
                    return material;
                }

                material->add_texture("u_material.ambient_diffuse"_H);
                material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
                material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);

                return material;
            }
            case MaterialType::PhongDiffuseNormalShadow: {
                const auto id {"phong_diffuse_normal_shadow"_H};

                const auto shader {load_shader(
                    id,
                    m_fs.path_engine_assets("shaders/phong_diffuse_normal_shadow.vert"),
                    m_fs.path_engine_assets("shaders/phong_diffuse_normal_shadow.frag")
                )};

                const auto [material, present] {m_res.material->load_check(id, shader, flags)};

                if (present) {
                    return material;
                }

                material->add_texture("u_material.ambient_diffuse"_H);
                material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
                material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);
                material->add_texture("u_material.normal"_H);

                return material;
            }
        }

        return {};
    }

    std::shared_ptr<Material> Ctx::load_material(Id id, const std::filesystem::path& vertex_file_path, const std::filesystem::path& fragment_file_path, MaterialType type, unsigned int flags) {
        using namespace resmanager::literals;

        const auto shader {load_shader(id, vertex_file_path, fragment_file_path)};

        const auto [material, present] {m_res.material->load_check(id, shader, flags)};

        if (present) {
            return material;
        }

        switch (type) {
            case MaterialType::Flat: {
                material->add_uniform(Material::Uniform::Vec3, "u_material.color"_H);
                break;
            }
            case MaterialType::Phong:
            case MaterialType::PhongShadow: {
                material->add_uniform(Material::Uniform::Vec3, "u_material.ambient_diffuse"_H);
                material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
                material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);
                break;
            }
            case MaterialType::PhongDiffuse:
            case MaterialType::PhongDiffuseShadow: {
                material->add_texture("u_material.ambient_diffuse"_H);
                material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
                material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);
                break;
            }
            case MaterialType::PhongDiffuseNormalShadow: {
                material->add_texture("u_material.ambient_diffuse"_H);
                material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
                material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);
                material->add_texture("u_material.normal"_H);
                break;
            }
        }

        return material;
    }

    std::shared_ptr<MaterialInstance> Ctx::load_material_instance(Id id, std::shared_ptr<Material> material) {
        return m_res.material_instance->load(id, material);
    }

    std::shared_ptr<GlShader> Ctx::load_shader(Id id, const std::filesystem::path& vertex_file_path, const std::filesystem::path& fragment_file_path) {
        if (m_res.shader->contains(id)) {
            return m_res.shader->get(id);
        }

        std::shared_ptr<GlShader> shader {
            m_res.shader->force_load(
                id,
                m_shd.load_shader(
                    m_shd.load_shader(utils::read_file(vertex_file_path)),
                    {{"D_POINT_LIGHTS", std::to_string(internal::Renderer::get_max_point_lights())}}
                ),
                m_shd.load_shader(
                    m_shd.load_shader(utils::read_file(fragment_file_path)),
                    {{"D_POINT_LIGHTS", std::to_string(internal::Renderer::get_max_point_lights())}}
                )
            )
        };

        m_rnd.register_shader(shader);

        return shader;
    }

    std::shared_ptr<GlFramebuffer> Ctx::load_framebuffer(Id id, const FramebufferSpecification& specification) {
        const auto [framebuffer, present] {m_res.framebuffer->load_check(id, specification)};

        if (present) {
            return framebuffer;
        }

        m_rnd.register_framebuffer(framebuffer);

        return framebuffer;
    }

    std::shared_ptr<Font> Ctx::load_font(Id id, const std::filesystem::path& file_path, const FontSpecification& specification, const std::function<void(Font*)>& bake) {
        if (m_res.font->contains(id)) {
            return m_res.font->get(id);
        }

        const auto font {m_res.font->force_load(id, utils::read_file(file_path), specification)};

        bake(font.get());

        return font;
    }

    std::shared_ptr<SoundData> Ctx::load_sound_data(const std::filesystem::path& file_path) {
        const auto id {Id(utils::file_name(file_path))};

        if (m_res.sound_data->contains(id)) {
            return m_res.sound_data->get(id);
        }

        return m_res.sound_data->force_load(id, utils::read_file(file_path));
    }

    std::shared_ptr<MusicTrack> Ctx::load_music_track(Id id, std::shared_ptr<SoundData> sound_data) {
        return m_res.music_track->load(id, sound_data);
    }

    std::shared_ptr<AlSource> Ctx::load_source(Id id) {
        return m_res.source->load(id);
    }

    std::shared_ptr<AlBuffer> Ctx::load_buffer(Id id, std::shared_ptr<SoundData> sound_data) {
        return m_res.buffer->load(id, sound_data);
    }
}
