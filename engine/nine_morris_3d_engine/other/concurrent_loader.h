#pragma once

#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/resource_manager.h"

/**
 * Objects of this class load assets in a separate thread.
 * Pass a loading function to load the assets.
 */
template<typename... Args>
class ConcurrentLoader {
public:
    using Function = std::function<void(ConcurrentLoader<Args...>&, const Args&...)>;

    ConcurrentLoader(const Function& load_function)
        : load_function(load_function) {}
    ~ConcurrentLoader() = default;

    bool done_loading() {
        return loaded.load();
    }

    void join_and_merge(Resources& res) {
        loading_thread.join();

        res.texture.merge(std::move(temp_res.texture));
        res.texture_3d.merge(std::move(temp_res.texture_3d));
        res.vertex_array.merge(std::move(temp_res.vertex_array));
        res.shader.merge(std::move(temp_res.shader));
        res.buffer.merge(std::move(temp_res.buffer));
        res.index_buffer.merge(std::move(temp_res.index_buffer));
        res.uniform_buffer.merge(std::move(temp_res.uniform_buffer));
        res.pixel_buffer.merge(std::move(temp_res.pixel_buffer));
        res.framebuffer.merge(std::move(temp_res.framebuffer));
        res.font.merge(std::move(temp_res.font));
        res.material.merge(std::move(temp_res.material));
        res.material_instance.merge(std::move(temp_res.material_instance));
        res.texture_data.merge(std::move(temp_res.texture_data));
        res.mesh_ptnt.merge(std::move(temp_res.mesh_ptnt));
        res.mesh_ptn.merge(std::move(temp_res.mesh_ptn));
        res.mesh_p.merge(std::move(temp_res.mesh_p));
    }

    bool joinable() {
        return loading_thread.joinable();
    }

    void start_loading_thread(const Args&... args) {
        DEB_INFO("Loading some assets from separate thread...");

        loading_thread = std::thread(load_function, std::ref(*this), args...);
    }

    void set_done() {
        loaded.store(true);
    }
private:
    Resources temp_res;
    Function load_function;
    std::thread loading_thread;
    std::atomic<bool> loaded = false;
};
