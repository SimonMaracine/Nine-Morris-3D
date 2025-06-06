#include "nine_morris_3d_engine/application/context.hpp"

#include <resmanager/resmanager.hpp>

#include "nine_morris_3d_engine/application/internal/input.hpp"
#include "nine_morris_3d_engine/application/application.hpp"
#include "nine_morris_3d_engine/application/scene.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/graphics/internal/imgui_context.hpp"
#include "nine_morris_3d_engine/graphics/opengl/debug.hpp"
#include "nine_morris_3d_engine/other/dependencies.hpp"
#include "nine_morris_3d_engine/other/utilities.hpp"

namespace sm {
    Ctx::Ctx(const ApplicationProperties& properties)
        : m_build_date(properties.build_date),
        m_build_time(properties.build_time),
        m_fs(properties.path_logs, properties.path_saved_data, properties.path_assets, properties.assets_directory),
        m_log(properties.log_file, m_fs),
        m_shd({m_fs.path_engine_assets().string(), m_fs.path_assets().string()}),
        m_win(properties, m_evt),
        m_rnd(properties.width, properties.height, m_fs, m_shd) {
        if (properties.default_renderer_parameters) {
            m_rnd.initialize(properties.width, properties.height, m_fs);
        }

        if (!m_fs.get_error_string().empty()) {
            LOG_DIST_ERROR("{}", m_fs.get_error_string());
        }

        m_scn.root_node_3d = std::make_shared<RootNode3D>();
        m_scn.root_node_2d = std::make_shared<RootNode2D>();

        m_default_camera_controller = std::make_shared<internal::DefaultCameraController>();

        m_information_text = std::make_shared<TextNode>(m_rnd.get_default_font());
        m_information_text->color = glm::vec3(0.9f);
    }

    bool Ctx::file_exists(const std::filesystem::path& path) {
        return internal::FileSystem::file_exists(path);
    }

    bool Ctx::is_directory(const std::filesystem::path& path) {
        return internal::FileSystem::is_directory(path);
    }

    bool Ctx::create_directory(const std::filesystem::path& path) {
        return internal::FileSystem::create_directory(path);
    }

    bool Ctx::delete_file(const std::filesystem::path& path) {
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

    std::filesystem::path Ctx::path_logs(const std::filesystem::path& path) const {
        return m_fs.path_logs(path);
    }

    std::filesystem::path Ctx::path_saved_data(const std::filesystem::path& path) const {
        return m_fs.path_saved_data(path);
    }

    std::filesystem::path Ctx::path_assets(const std::filesystem::path& path) const {
        return m_fs.path_assets(path);
    }

    std::filesystem::path Ctx::path_engine_assets(const std::filesystem::path& path) const {
        return m_fs.path_engine_assets(path);
    }

    int Ctx::get_window_width() const {
        return m_win.get_width();
    }

    int Ctx::get_window_height() const {
        return m_win.get_height();
    }

    void Ctx::show_window() const {
        m_win.show();
    }

    void Ctx::set_window_vsync(bool enable) const {
        m_win.set_vsync(enable);
    }

    void Ctx::set_window_icons(std::initializer_list<std::unique_ptr<TextureData>> icons) {
        m_win.set_icons(icons);
    }

    void Ctx::set_window_size(int width, int height) {
        m_win.set_size(width, height);
    }

    double Ctx::get_time() {
        return internal::Window::get_time();
    }

    std::shared_ptr<Font> Ctx::get_renderer_default_font() const {
        return m_rnd.get_default_font();
    }

    void Ctx::set_renderer_color_correction(bool enable) {
        m_rnd.set_color_correction(enable);
    }

    bool Ctx::get_renderer_color_correction() const {
        return m_rnd.get_color_correction();
    }

    void Ctx::set_renderer_clear_color(glm::vec3 color) {
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

    void Ctx::play_audio_sound(std::shared_ptr<SoundData> sound_data) {
        internal::audio::play_sound(sound_data);
    }

    void Ctx::set_audio_volume(float volume) {
        internal::audio::set_volume(volume);
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

    bool Ctx::is_key_pressed(Key key) {
        return internal::is_key_pressed(key);
    }

    bool Ctx::is_button_pressed(Button button) {
        return internal::is_button_pressed(button);
    }

    std::pair<float, float> Ctx::get_mouse_position() {
        return internal::get_mouse_position();
    }

    void Ctx::invalidate_dear_imgui_texture() {
        internal::imgui_context::invalidate_font_texture();
    }

    void Ctx::change_scene(Id id, bool clear_resources) {
        m_application->change_scene(id, clear_resources);
    }

    void Ctx::show_information_text() {
        std::string information_text;
        information_text += std::string(opengl_debug::get_opengl_version()) + '\n';
        information_text += std::string(opengl_debug::get_glsl_version()) + '\n';
        information_text += std::string(opengl_debug::get_renderer()) + '\n';
        information_text += std::string(opengl_debug::get_vendor()) + '\n';
        information_text += std::to_string(static_cast<int>(m_fps)) + " FPS ";
        information_text += std::to_string(static_cast<int>(m_delta * 1000.0f)) + " ms\n";
        information_text += std::string("build ") + m_build_date + " " + m_build_time;
#ifndef SM_BUILD_DISTRIBUTION
        information_text += " dev";
#endif

        m_information_text->text = std::move(information_text);

        // The font could be empty, because renderer initializaton might have been deferred
        if (m_information_text->get_font() == nullptr) {
            m_information_text->set_font(m_rnd.get_default_font());
        }

        m_scn.root_node_2d->add_node(m_information_text);
    }

    float Ctx::get_delta() const {
        return m_delta;
    }

    float Ctx::get_fps() const {
        return m_fps;
    }

    std::string Ctx::get_information() const {
#ifndef SM_BUILD_DISTRIBUTION
        const char* SUFFIX {" dev"};
#else
        const char* SUFFIX {""};
#endif

        std::string result;
        result += "*** Build ***\n";
        result += std::string(m_build_date) + " " + m_build_time + SUFFIX + '\n';
        result += "*** Compiler ***\n";
#if defined(SM_PLATFORM_LINUX)
        result += "GCC: " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + '\n';
#elif defined(SM_PLATFORM_WINDOWS)
        result += "MSVC: " + std::to_string(_MSC_VER) + '\n';
#endif
        result += "*** Environment ***\n";
#if defined(SM_PLATFORM_LINUX)
        result += "HOSTNAME=" + utils::get_environment_variable("HOSTNAME") + '\n';
        result += "XDG_CURRENT_DESKTOP=" + utils::get_environment_variable("XDG_CURRENT_DESKTOP") + '\n';
        result += "XDG_SESSION_DESKTOP=" + utils::get_environment_variable("XDG_SESSION_DESKTOP") + '\n';
        result += "XDG_SESSION_TYPE=" + utils::get_environment_variable("XDG_SESSION_TYPE") + '\n';
#elif defined(SM_PLATFORM_WINDOWS)
        result += "OS=" + utils::get_environment_variable("OS") + '\n';
        result += "PROCESSOR_IDENTIFIER=" + utils::get_environment_variable("PROCESSOR_IDENTIFIER") + '\n';
        result += "NUMBER_OF_PROCESSORS=" + utils::get_environment_variable("NUMBER_OF_PROCESSORS") + '\n';
#endif
        result += opengl_debug::get_information();
        result += dependencies::get_information();

        return result;
    }

    std::shared_ptr<Mesh> Ctx::get_mesh(Id id) const {
        return m_res.mesh->get(id);
    }

    std::shared_ptr<Mesh> Ctx::load_mesh(Id id, const std::filesystem::path& file_path, const MeshSpecification& specification) {
        if (m_res.mesh->contains(id)) {
            return m_res.mesh->get(id);
        }

        return m_res.mesh->force_load(id, utils::read_file(file_path), specification);
    }

    std::shared_ptr<Mesh> Ctx::load_mesh(const std::filesystem::path& file_path, const MeshSpecification& specification) {
        const auto id {Id(utils::file_name(file_path))};

        if (m_res.mesh->contains(id)) {
            return m_res.mesh->get(id);
        }

        return m_res.mesh->force_load(id, utils::read_file(file_path), specification);
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
                case MeshType::P:
                    layout.add(0, VertexBufferLayout::Float, 3);
                    break;
                case MeshType::PN:
                    layout.add(0, VertexBufferLayout::Float, 3);
                    layout.add(1, VertexBufferLayout::Float, 3);
                    break;
                case MeshType::PNT:
                    layout.add(0, VertexBufferLayout::Float, 3);
                    layout.add(1, VertexBufferLayout::Float, 3);
                    layout.add(2, VertexBufferLayout::Float, 2);
                    break;
                case MeshType::PNTT:
                    layout.add(0, VertexBufferLayout::Float, 3);
                    layout.add(1, VertexBufferLayout::Float, 3);
                    layout.add(2, VertexBufferLayout::Float, 2);
                    layout.add(3, VertexBufferLayout::Float, 3);
                    break;
            }

            va->add_vertex_buffer(vertex_buffer, layout);
            va->add_index_buffer(index_buffer);
            va->bind_index_buffer(0);
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

    std::shared_ptr<Material> Ctx::load_material(MaterialType type) {
        using namespace resmanager::literals;

        switch (type) {
            case MaterialType::Flat: {
                const auto id {"flat"_H};

                const auto shader {load_shader(
                    id,
                    m_fs.path_engine_assets("shaders/flat.vert"),
                    m_fs.path_engine_assets("shaders/flat.frag")
                )};

                const auto [material, present] {m_res.material->load_check(id, shader)};

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

                const auto [material, present] {m_res.material->load_check(id, shader)};

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

                const auto [material, present] {m_res.material->load_check(id, shader)};

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

                const auto [material, present] {m_res.material->load_check(id, shader)};

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

                const auto [material, present] {m_res.material->load_check(id, shader)};

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

                const auto [material, present] {m_res.material->load_check(id, shader)};

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

    std::shared_ptr<Material> Ctx::load_material(Id id, const std::filesystem::path& vertex_file_path, const std::filesystem::path& fragment_file_path, MaterialType type) {
        using namespace resmanager::literals;

        const auto shader {load_shader(id, vertex_file_path, fragment_file_path)};

        const auto [material, present] {m_res.material->load_check(id, shader)};

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

        return m_res.sound_data->load(id, file_path);
    }
}
