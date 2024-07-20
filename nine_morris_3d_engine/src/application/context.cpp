#include "nine_morris_3d_engine/application/context.hpp"

#include <utility>
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

    std::string Ctx::load_shader(const std::string& source) const {
        return shd.load_shader(source);
    }

    const Monitors& Ctx::get_monitors() {
        return win.get_monitors();
    }

    int Ctx::get_width() const {
        return win.get_width();
    }

    int Ctx::get_height() const {
        return win.get_height();
    }

    void Ctx::show() const {
        win.show();
    }

    void Ctx::set_vsync(int interval) const {
        win.set_vsync(interval);
    }

    void Ctx::add_cursor(Id id, std::unique_ptr<TextureData>&& cursor, int x_hotspot, int y_hotspot) {
        win.add_cursor(id, std::move(cursor), x_hotspot, y_hotspot);
    }

    void Ctx::set_cursor(Id id) const {
        win.set_cursor(id);
    }

    void Ctx::set_icons(std::initializer_list<std::unique_ptr<TextureData>> icons) const {
        win.set_icons(icons);
    }

    double Ctx::get_time() {
        return internal::Window::get_time();
    }

    std::shared_ptr<Font> Ctx::get_default_font() const {
        return rnd.get_default_font();
    }

    void Ctx::set_color_correction(bool enable) {
        rnd.set_color_correction(enable);
    }

    void Ctx::set_clear_color(glm::vec3 color) {
        rnd.set_clear_color(color);
    }

    AlListener& Ctx::get_listener() {
        return snd.get_listener();
    }

    void Ctx::play_music_track(std::shared_ptr<MusicTrack> music_track) {
        mus.play_music_track(music_track);
    }

    void Ctx::stop_music_track() {
        mus.stop_music_track();
    }

    void Ctx::pause_music_track() {
        mus.pause_music_track();
    }

    void Ctx::continue_music_track() {
        mus.continue_music_track();
    }

    void Ctx::set_music_gain(float gain) {
        mus.set_music_gain(gain);
    }

    void Ctx::add_task(Id id, const Task::TaskFunction& function) {
        tsk.add_task(id, function);
    }

    void Ctx::remove_task(Id id) {
        tsk.remove_task(id);
    }

    bool Ctx::is_key_pressed(Key key) const {
        return inp.is_key_pressed(key);
    }

    bool Ctx::is_mouse_button_pressed(MouseButton button) const {
        return inp.is_mouse_button_pressed(button);
    }

    float Ctx::get_mouse_x() const {
        return inp.get_mouse_x();
    }

    float Ctx::get_mouse_y() const {
        return inp.get_mouse_y();
    }

    std::pair<float, float> Ctx::get_mouse() const {
        return inp.get_mouse();
    }

    void Ctx::capture(const Camera& camera, glm::vec3 position) {
        scn.capture(camera, position);
    }

    void Ctx::capture(const Camera2D& camera_2d) {
        scn.capture(camera_2d);
    }

    void Ctx::skybox(std::shared_ptr<GlTextureCubemap> texture) {
        scn.skybox(texture);
    }

    void Ctx::shadow(ShadowBox& box) {
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

    void Ctx::debug_add_lines(const std::vector<glm::vec3>& points, glm::vec3 color) {  // TODO rename to positions
        scn.debug_add_lines(points, color);
    }

    void Ctx::debug_add_lines(std::initializer_list<glm::vec3> points, glm::vec3 color) {  // TODO rename to positions
        scn.debug_add_lines(points, color);
    }

    void Ctx::debug_add_point(glm::vec3 position, glm::vec3 color) {
        scn.debug_add_point(position, color);
    }

    void Ctx::debug_add_lamp(glm::vec3 position, glm::vec3 color) {
        scn.debug_add_lamp(position, color);
    }




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

    Model Ctx::load_model(
        resmanager::HashedStr64 identifier,
        const std::string& file_path,
        const std::string& mesh_name,
        Mesh::Type type
    ) {
        const auto mesh {res.mesh.load(identifier, utils::read_file(file_path), mesh_name, type)};

        const auto vertex_buffer {res.vertex_buffer.load(identifier, mesh->get_vertices(), mesh->get_vertices_size())};
        const auto index_buffer {res.index_buffer.load(identifier, mesh->get_indices(), mesh->get_indices_size())};

        const auto [vertex_array, present] {res.vertex_array.load_check(identifier)};

        if (present) {
            return std::make_pair(mesh, vertex_array);
        }

        vertex_array->configure([&](GlVertexArray* va) {
            VertexBufferLayout layout;

            switch (type) {
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

        return std::make_pair(mesh, vertex_array);
    }

    Model Ctx::load_model(
        const std::string& file_path,
        const std::string& mesh_name,
        Mesh::Type type
    ) {
        return load_model(resmanager::HashedStr64(utils::file_name(file_path)), file_path, mesh_name, type);
    }

    std::shared_ptr<GlTexture> Ctx::load_texture(
        const std::string& file_path,
        const TexturePostProcessing& post_processing,
        const TextureSpecification& specification
    ) {
        const auto identifier {resmanager::HashedStr64(utils::file_name(file_path))};

        const auto data {res.texture_data.load(identifier, utils::read_file(file_path), post_processing)};

        return res.texture.load(identifier, data, specification);
    }

    std::shared_ptr<GlTextureCubemap> Ctx::load_texture_cubemap(
        const char* identifier,
        std::initializer_list<std::string> file_paths,
        const TexturePostProcessing& post_processing,
        TextureFormat format
    ) {
        if (res.texture_cubemap.contains(resmanager::HashedStr64(identifier))) {
            return res.texture_cubemap.get(resmanager::HashedStr64(identifier));
        }

        std::array<std::shared_ptr<TextureData>, 6> textures {};
        std::size_t i {0};  // TODO C++20

        for (const auto& file_path : file_paths) {
            textures[i++] = res.texture_data.force_load(
                resmanager::HashedStr64(std::string(identifier) + ":" + file_path),
                utils::read_file(file_path),
                post_processing
            );
        }

        // For some reason only like this you can pass a list to a variadic template argument
        const std::initializer_list list {
            textures[0],
            textures[1],
            textures[2],
            textures[3],
            textures[4],
            textures[5]
        };

        return res.texture_cubemap.force_load(resmanager::HashedStr64(identifier), list, format);
    }

    std::shared_ptr<Material> Ctx::load_material(
        MaterialType type,
        unsigned int flags
    ) {
        using namespace resmanager::literals;

        switch (type) {
            case MaterialType::Flat: {
                const auto identifier {"flat"_H};

                const auto shader {load_shader(
                    identifier,
                    fs.path_engine_assets("shaders/flat.vert"),
                    fs.path_engine_assets("shaders/flat.frag")
                )};

                const auto [material, present] {res.material.load_check(identifier, shader, flags)};

                if (present) {
                    return material;
                }

                material->add_uniform(Material::Uniform::Vec3, "u_material.color"_H);

                return material;
            }
            case MaterialType::Phong: {
                const auto identifier {"phong"_H};

                const auto shader {load_shader(
                    identifier,
                    fs.path_engine_assets("shaders/phong.vert"),
                    fs.path_engine_assets("shaders/phong.frag")
                )};

                const auto [material, present] {res.material.load_check(identifier, shader, flags)};

                if (present) {
                    return material;
                }

                material->add_uniform(Material::Uniform::Vec3, "u_material.ambient_diffuse"_H);
                material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
                material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);

                return material;
            }
            case MaterialType::PhongShadow: {
                const auto identifier {"phong_shadow"_H};

                const auto shader {load_shader(
                    identifier,
                    fs.path_engine_assets("shaders/phong_shadow.vert"),
                    fs.path_engine_assets("shaders/phong_shadow.frag")
                )};

                const auto [material, present] {res.material.load_check(identifier, shader, flags)};

                if (present) {
                    return material;
                }

                material->add_uniform(Material::Uniform::Vec3, "u_material.ambient_diffuse"_H);
                material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
                material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);

                return material;
            }
            case MaterialType::PhongDiffuse: {
                const auto identifier {"phong_diffuse"_H};

                const auto shader {load_shader(
                    identifier,
                    fs.path_engine_assets("shaders/phong_diffuse.vert"),
                    fs.path_engine_assets("shaders/phong_diffuse.frag")
                )};

                const auto [material, present] {res.material.load_check(identifier, shader, flags)};

                if (present) {
                    return material;
                }

                material->add_uniform(Material::Uniform::Vec3, "u_material.ambient_diffuse"_H);
                material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
                material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);

                return material;
            }
            case MaterialType::PhongDiffuseShadow: {
                const auto identifier {"phong_diffuse_shadow"_H};

                const auto shader {load_shader(
                    identifier,
                    fs.path_engine_assets("shaders/phong_diffuse_shadow.vert"),
                    fs.path_engine_assets("shaders/phong_diffuse_shadow.frag")
                )};

                const auto [material, present] {res.material.load_check(identifier, shader, flags)};

                if (present) {
                    return material;
                }

                material->add_texture("u_material.ambient_diffuse"_H);
                material->add_uniform(Material::Uniform::Vec3, "u_material.specular"_H);
                material->add_uniform(Material::Uniform::Float, "u_material.shininess"_H);

                return material;
            }
            case MaterialType::PhongDiffuseNormalShadow: {
                const auto identifier {"phong_diffuse_normal_shadow"_H};

                const auto shader {load_shader(
                    identifier,
                    fs.path_engine_assets("shaders/phong_diffuse_normal_shadow.vert"),
                    fs.path_engine_assets("shaders/phong_diffuse_normal_shadow.frag")
                )};

                const auto [material, present] {res.material.load_check(identifier, shader, flags)};

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

    std::shared_ptr<Material> Ctx::load_material(
        resmanager::HashedStr64 identifier,
        const std::string& vertex_file_path,
        const std::string& fragment_file_path,
        MaterialType type,
        unsigned int flags
    ) {
        using namespace resmanager::literals;

        const auto shader {load_shader(identifier, vertex_file_path, fragment_file_path)};

        const auto [material, present] {res.material.load_check(identifier, shader, flags)};

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

    std::shared_ptr<MaterialInstance> Ctx::load_material_instance(
        resmanager::HashedStr64 identifier,
        std::shared_ptr<Material> material
    ) {
        return res.material_instance.load(identifier, material);
    }

    std::shared_ptr<GlShader> Ctx::load_shader(
        resmanager::HashedStr64 identifier,
        const std::string& vertex_file_path,
        const std::string& fragment_file_path,
        bool include_processing
    ) {
        if (res.shader.contains(identifier)) {
            return res.shader.get(identifier);
        }

        std::shared_ptr<GlShader> shader;

        if (include_processing) {
            shader = res.shader.force_load(
                identifier,
                shd.load_shader(utils::read_file(vertex_file_path)),
                shd.load_shader(utils::read_file(fragment_file_path))
            );
        } else {
            shader = res.shader.force_load(
                identifier,
                utils::read_file(vertex_file_path),
                utils::read_file(fragment_file_path)
            );
        }

        rnd.register_shader(shader);

        return shader;
    }

    std::shared_ptr<GlFramebuffer> Ctx::load_framebuffer(
        resmanager::HashedStr64 identifier,
        const FramebufferSpecification& specification
    ) {
        const auto [framebuffer, present] {res.framebuffer.load_check(identifier, specification)};

        if (present) {
            return framebuffer;
        }

        rnd.register_framebuffer(framebuffer);

        return framebuffer;
    }

    std::shared_ptr<Font> Ctx::load_font(
        resmanager::HashedStr64 identifier,
        const std::string& file_path,
        const FontSpecification& specification
    ) {
        return res.font.load(
            identifier,
            utils::read_file(file_path),
            specification
        );
    }
}
