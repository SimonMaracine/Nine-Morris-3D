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

    ConcurrentLoader(const ConcurrentLoader&) = delete;
    ConcurrentLoader& operator=(const ConcurrentLoader&) = delete;
    ConcurrentLoader(ConcurrentLoader&&) = delete;
    ConcurrentLoader& operator=(ConcurrentLoader&&) = delete;

    ResourcesCache& operator()() {
        return local_res;
    }

    bool done_loading() const {
        return loaded.load();
    }

    void join_and_merge(ResourcesCache& res) {
        loading_thread.join();

        res.merge(local_res);

        DEB_INFO("Merged local resources into global resources");
    }

    void join() {
        loading_thread.join();
    }

    bool joinable() const {
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
    ResourcesCache local_res;
    Function load_function;
    std::thread loading_thread;
    std::atomic<bool> loaded = false;
};
