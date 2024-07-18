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
        : fs(properties.application_name, properties.assets_directory), log(properties.log_file, fs),
        shd({"engine_assets", properties.assets_directory}), win(properties, &evt),
        snd(properties.audio), inp(win.get_handle()),
        rnd(properties.width, properties.height, properties.samples, fs, shd) {
        if (!fs.error_string.empty()) {
            LOG_DIST_ERROR("{}", fs.error_string);
        }
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
        text.font = rnd.storage.default_font;
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
        const TexturePostProcessing& post_processing
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

        return res.texture_cubemap.force_load(resmanager::HashedStr64(identifier), list);
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
                utils::read_file(vertex_file_path),
                utils::read_file(fragment_file_path),
                shd
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
