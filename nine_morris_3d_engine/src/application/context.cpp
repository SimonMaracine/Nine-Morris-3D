#include "nine_morris_3d_engine/application/context.hpp"

#include <array>
#include <cassert>

#include <resmanager/resmanager.hpp>

#include "nine_morris_3d_engine/application/application.hpp"
#include "nine_morris_3d_engine/application/scene.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/audio/openal/debug.hpp"
#include "nine_morris_3d_engine/graphics/opengl/debug.hpp"
#include "nine_morris_3d_engine/other/dependencies.hpp"
#include "nine_morris_3d_engine/other/utilities.hpp"

namespace sm {
    Ctx::Ctx(const ApplicationProperties& properties)
        : fs(properties.application_name, properties.assets_directory),
        log(properties.log_file, fs),
        shd({"engine_assets", properties.assets_directory}),
        win(properties, &evt),
        rnd(properties.width, properties.height, properties.samples, fs, shd),
        snd(properties.audio), inp(win.get_handle()) {
        if (!fs.get_error_string().empty()) {
            LOG_DIST_ERROR("{}", fs.get_error_string());
        }
    }

    bool Ctx::directory_exists(const std::string& path) {
        return internal::FileSystem::directory_exists(path);
    }

    bool Ctx::create_directory(const std::string& path) {
        return internal::FileSystem::create_directory(path);
    }

    bool Ctx::delete_file(const std::string& path) {
        return internal::FileSystem::delete_file(path);
    }

    std::string Ctx::current_working_directory() {
        return internal::FileSystem::current_working_directory();
    }

    std::string Ctx::path_logs() const {
        return fs.path_logs();
    }

    std::string Ctx::path_saved_data() const {
        return fs.path_saved_data();
    }

    std::string Ctx::path_assets() const {
        return fs.path_assets();
    }

    std::string Ctx::path_engine_assets() const {
        return fs.path_engine_assets();
    }

    std::string Ctx::path_logs(const std::string& path) const {
        return fs.path_logs(path);
    }

    std::string Ctx::path_saved_data(const std::string& path) const {
        return fs.path_saved_data(path);
    }

    std::string Ctx::path_assets(const std::string& path) const {
        return fs.path_assets(path);
    }

    std::string Ctx::path_engine_assets(const std::string& path) const {
        return fs.path_engine_assets(path);
    }

    int Ctx::get_window_width() const noexcept {
        return win.get_width();
    }

    int Ctx::get_window_height() const noexcept {
        return win.get_height();
    }

    void Ctx::show_window() const noexcept {
        win.show();
    }

    void Ctx::set_window_vsync(int interval) const noexcept {
        win.set_vsync(interval);
    }

    void Ctx::add_window_cursor(Id id, std::unique_ptr<TextureData>&& cursor, int x_hotspot, int y_hotspot) {
        win.add_cursor(id, std::move(cursor), x_hotspot, y_hotspot);
    }

    void Ctx::set_window_cursor(Id id) const {
        win.set_cursor(id);
    }

    void Ctx::set_window_icons(std::initializer_list<std::unique_ptr<TextureData>> icons) const {
        win.set_icons(icons);
    }

    Monitors Ctx::get_monitors() const {
        return win.get_monitors();
    }

    double Ctx::get_time() noexcept {
        return internal::Window::get_time();
    }

    std::shared_ptr<Font> Ctx::get_default_font() const noexcept {
        return rnd.get_default_font();
    }

    void Ctx::set_color_correction(bool enable) noexcept {
        rnd.set_color_correction(enable);
    }

    bool Ctx::get_color_correction() const noexcept {
        return rnd.get_color_correction();
    }

    void Ctx::set_clear_color(glm::vec3 color) noexcept {
        rnd.set_clear_color(color);
    }

    void Ctx::play_music_track(std::shared_ptr<MusicTrack> music_track) {
        mus.play(music_track);
    }

    void Ctx::stop_music_track() {
        mus.stop();
    }

    void Ctx::pause_music_track() {
        mus.pause();
    }

    void Ctx::resume_music_track() {
        mus.resume();
    }

    void Ctx::set_music_gain(float gain) {
        mus.set_gain(gain);
    }

    void Ctx::add_task(const Task::TaskFunction& function, void* user_data) {
        tsk.add(function, user_data);
    }

    void Ctx::add_task_async(const AsyncTask::TaskFunction& function, void* user_data) {
        tsk.add_async(function, user_data);
    }

    bool Ctx::is_key_pressed(Key key) const noexcept {
        return inp.is_key_pressed(key);
    }

    bool Ctx::is_mouse_button_pressed(MouseButton button) const noexcept {
        return inp.is_mouse_button_pressed(button);
    }

    std::pair<float, float> Ctx::get_mouse_position() const noexcept {
        return inp.get_mouse_position();
    }

    void Ctx::capture(const Camera& camera, glm::vec3 position) noexcept {
        scn.capture(camera, position);
    }

    void Ctx::capture(const Camera2D& camera_2d) noexcept {
        scn.capture(camera_2d);
    }

    void Ctx::skybox(std::shared_ptr<GlTextureCubemap> texture) noexcept {
        scn.skybox(texture);
    }

    void Ctx::shadow(ShadowBox& box) noexcept {
        scn.shadow(box);
    }

    void Ctx::add_post_processing(std::shared_ptr<PostProcessingStep> step) {
        scn.add_post_processing(step);
    }

    void Ctx::add_renderable(Renderable& renderable) {
        scn.add_renderable(renderable);
    }

    void Ctx::add_light(DirectionalLight& light) {
        scn.add_light(light);
    }

    void Ctx::add_light(PointLight& light) {
        scn.add_light(light);
    }

    void Ctx::add_text(Text& text) {
        scn.add_text(text);
    }

    void Ctx::add_quad(Quad& quad) {
        scn.add_quad(quad);
    }

    void Ctx::debug_add_line(glm::vec3 position1, glm::vec3 position2, glm::vec3 color) {
        scn.debug_add_line(position1, position2, color);
    }

    void Ctx::debug_add_lines(const std::vector<glm::vec3>& positions, glm::vec3 color) {
        scn.debug_add_lines(positions, color);
    }

    void Ctx::debug_add_lines(std::initializer_list<glm::vec3> positions, glm::vec3 color) {
        scn.debug_add_lines(positions, color);
    }

    void Ctx::debug_add_point(glm::vec3 position, glm::vec3 color) {
        scn.debug_add_point(position, color);
    }

    void Ctx::debug_add_lamp(glm::vec3 position, glm::vec3 color) {
        scn.debug_add_lamp(position, color);
    }

    void Ctx::change_scene(Id id, bool clear_resources) noexcept {
        assert(application->scene_next == nullptr);

        for (auto& meta_scene : application->scene_meta_scenes) {
            if (meta_scene.id == id) {
                application->scene_next = &meta_scene;
                application->scene_clear_resources = clear_resources;
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
        text.font = rnd.get_default_font();
        text.text = std::move(info_text);
        text.color = glm::vec3(1.0f);

        // Don't add it to the debug lists
        scn.add_text(const_cast<const Text&>(text));
    }

    std::string Ctx::get_information() const {
        std::string result;

        result += '\n';
        result += openal_debug::get_information();
        result += opengl_debug::get_information();
        result += dependencies::get_information();

        return result;
    }

    std::shared_ptr<Mesh> Ctx::load_mesh(Id id, const std::string& file_path, const std::string& mesh_name, Mesh::Type type) {
        if (res.mesh->contains(id)) {
            return res.mesh->get(id);
        }

        return res.mesh->force_load(id, utils::read_file(file_path), mesh_name, type);
    }

    std::shared_ptr<Mesh> Ctx::load_mesh(const std::string& file_path, const std::string& mesh_name, Mesh::Type type) {
        const auto id {Id(utils::file_name(file_path))};

        if (res.mesh->contains(id)) {
            return res.mesh->get(id);
        }

        return res.mesh->force_load(id, utils::read_file(file_path), mesh_name, type);
    }

    std::shared_ptr<GlVertexArray> Ctx::load_vertex_array(Id id, std::shared_ptr<Mesh> mesh) {
        const auto vertex_buffer {res.vertex_buffer->load(id, mesh->get_vertices(), mesh->get_vertices_size())};
        const auto index_buffer {res.index_buffer->load(id, mesh->get_indices(), mesh->get_indices_size())};

        const auto [vertex_array, present] {res.vertex_array->load_check(id)};

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

    std::shared_ptr<TextureData> Ctx::load_texture_data(const std::string& file_path, const TexturePostProcessing& post_processing) {
        const auto id {Id(utils::file_name(file_path))};

        if (res.texture_data->contains(id)) {
            return res.texture_data->get(id);
        }

        return res.texture_data->force_load(id, utils::read_file(file_path), post_processing);
    }

    std::shared_ptr<GlTexture> Ctx::load_texture(Id id, std::shared_ptr<TextureData> texture_data, const TextureSpecification& specification) {
        return res.texture->load(id, texture_data, specification);
    }

    std::shared_ptr<GlTexture> Ctx::reload_texture(Id id, std::shared_ptr<TextureData> texture_data, const TextureSpecification& specification) {
        return res.texture->force_load(id, texture_data, specification);
    }

    std::shared_ptr<GlTextureCubemap> Ctx::load_texture_cubemap(Id id, std::initializer_list<std::shared_ptr<TextureData>> texture_data, TextureFormat format) {
        return res.texture_cubemap->load(id, texture_data, format);
    }

    std::shared_ptr<GlTextureCubemap> Ctx::reload_texture_cubemap(Id id, std::initializer_list<std::shared_ptr<TextureData>> texture_data, TextureFormat format) {
        return res.texture_cubemap->force_load(id, texture_data, format);
    }

    std::shared_ptr<Material> Ctx::load_material(MaterialType type, unsigned int flags) {
        using namespace resmanager::literals;

        switch (type) {
            case MaterialType::Flat: {
                const auto id {"flat"_H};

                const auto shader {load_shader(
                    id,
                    fs.path_engine_assets("shaders/flat.vert"),
                    fs.path_engine_assets("shaders/flat.frag")
                )};

                const auto [material, present] {res.material->load_check(id, shader, flags)};

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
                    fs.path_engine_assets("shaders/phong.vert"),
                    fs.path_engine_assets("shaders/phong.frag")
                )};

                const auto [material, present] {res.material->load_check(id, shader, flags)};

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
                    fs.path_engine_assets("shaders/phong_shadow.vert"),
                    fs.path_engine_assets("shaders/phong_shadow.frag")
                )};

                const auto [material, present] {res.material->load_check(id, shader, flags)};

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
                    fs.path_engine_assets("shaders/phong_diffuse.vert"),
                    fs.path_engine_assets("shaders/phong_diffuse.frag")
                )};

                const auto [material, present] {res.material->load_check(id, shader, flags)};

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
                    fs.path_engine_assets("shaders/phong_diffuse_shadow.vert"),
                    fs.path_engine_assets("shaders/phong_diffuse_shadow.frag")
                )};

                const auto [material, present] {res.material->load_check(id, shader, flags)};

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
                    fs.path_engine_assets("shaders/phong_diffuse_normal_shadow.vert"),
                    fs.path_engine_assets("shaders/phong_diffuse_normal_shadow.frag")
                )};

                const auto [material, present] {res.material->load_check(id, shader, flags)};

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

        assert(false);
        return {};
    }

    std::shared_ptr<Material> Ctx::load_material(Id id, const std::string& vertex_file_path, const std::string& fragment_file_path, MaterialType type, unsigned int flags) {
        using namespace resmanager::literals;

        const auto shader {load_shader(id, vertex_file_path, fragment_file_path)};

        const auto [material, present] {res.material->load_check(id, shader, flags)};

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
        return res.material_instance->load(id, material);
    }

    std::shared_ptr<GlShader> Ctx::load_shader(Id id, const std::string& vertex_file_path, const std::string& fragment_file_path) {
        if (res.shader->contains(id)) {
            return res.shader->get(id);
        }

        std::shared_ptr<GlShader> shader {
            res.shader->force_load(
                id,
                shd.load_shader(
                    shd.load_shader(utils::read_file(vertex_file_path)),
                    {{"D_POINT_LIGHTS", std::to_string(internal::Renderer::get_max_point_lights())}}
                ),
                shd.load_shader(
                    shd.load_shader(utils::read_file(fragment_file_path)),
                    {{"D_POINT_LIGHTS", std::to_string(internal::Renderer::get_max_point_lights())}}
                )
            )
        };

        rnd.register_shader(shader);

        return shader;
    }

    std::shared_ptr<GlFramebuffer> Ctx::load_framebuffer(Id id, const FramebufferSpecification& specification) {
        const auto [framebuffer, present] {res.framebuffer->load_check(id, specification)};

        if (present) {
            return framebuffer;
        }

        rnd.register_framebuffer(framebuffer);

        return framebuffer;
    }

    std::shared_ptr<Font> Ctx::load_font(Id id, const std::string& file_path, const FontSpecification& specification, const std::function<void(Font*)>& bake) {
        if (res.font->contains(id)) {
            return res.font->get(id);
        }

        const auto font {res.font->force_load(id, utils::read_file(file_path), specification)};

        bake(font.get());

        return font;
    }

    std::shared_ptr<SoundData> Ctx::load_sound_data(const std::string& file_path) {
        const auto id {Id(utils::file_name(file_path))};

        if (res.sound_data->contains(id)) {
            return res.sound_data->get(id);
        }

        return res.sound_data->force_load(id, utils::read_file(file_path));
    }

    std::shared_ptr<MusicTrack> Ctx::load_music_track(Id id, std::shared_ptr<SoundData> sound_data) {
        return res.music_track->load(id, sound_data);
    }

    std::shared_ptr<AlSource> Ctx::load_source(Id id) {
        return res.source->load(id);
    }

    std::shared_ptr<AlBuffer> Ctx::load_buffer(Id id, std::shared_ptr<SoundData> sound_data) {
        return res.buffer->load(id, sound_data);
    }
}
