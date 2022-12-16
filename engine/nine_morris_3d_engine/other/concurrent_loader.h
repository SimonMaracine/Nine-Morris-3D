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
    using LoadFunction = std::function<void(ConcurrentLoader<Args...>&, const Args&...)>;
    using CallbackFunction = std::function<void()>;

    ConcurrentLoader(const LoadFunction& load_function, const CallbackFunction& callback_function)
        : load_function(load_function), callback_function(callback_function) {}
    ~ConcurrentLoader() = default;

    ConcurrentLoader(const ConcurrentLoader&) = delete;
    ConcurrentLoader& operator=(const ConcurrentLoader&) = delete;
    ConcurrentLoader(ConcurrentLoader&&) = delete;
    ConcurrentLoader& operator=(ConcurrentLoader&&) = delete;

    void start_loading_thread(const Args&... args);
    void update(Application* app);
    bool is_done() const;
    void join_and_merge(ResourcesCache& res);
    void join();
    bool joinable() const;
    void set_done();
    ResourcesCache& operator()();
private:
    void reset();

    ResourcesCache local_res;
    LoadFunction load_function;
    CallbackFunction callback_function;
    std::thread loading_thread;
    std::atomic<bool> loaded = false;
    bool in_use = false;
};

template<typename... Args>
void ConcurrentLoader<Args...>::start_loading_thread(const Args&... args) {
    DEB_INFO("Loading some assets from separate thread...");

    in_use = true;
    loading_thread = std::thread(load_function, std::ref(*this), args...);
}

template<typename... Args>
void ConcurrentLoader<Args...>::update(Application* app) {
    if (in_use && is_done()) {
        join_and_merge(app->res);
        reset();

        callback_function();
    }
}

template<typename... Args>
bool ConcurrentLoader<Args...>::is_done() const {
    return loaded.load();
}

template<typename... Args>
void ConcurrentLoader<Args...>::join_and_merge(ResourcesCache& res) {
    loading_thread.join();

    res.merge(local_res);

    DEB_INFO("Merged local resources into global ones");
}

template<typename... Args>
void ConcurrentLoader<Args...>::join() {
    loading_thread.join();
}

template<typename... Args>
bool ConcurrentLoader<Args...>::joinable() const {
    return loading_thread.joinable();
}

template<typename... Args>
void ConcurrentLoader<Args...>::set_done() {
    loaded.store(true);
}

template<typename... Args>
ResourcesCache& ConcurrentLoader<Args...>::operator()() {
    return local_res;
}

template<typename... Args>
void ConcurrentLoader<Args...>::reset() {
    local_res = ResourcesCache {};
    loading_thread = std::thread {};
    loaded.store(false);
    in_use = false;
}
