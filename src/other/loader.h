#pragma once

#include "other/logging.h"

/**
 * Objects of this class load assets in a separate thread.
 * Pass a struct with the assets and a function to load them.
 */
template<typename Assets, typename... Args>
class Loader {
public:
    using FunctionType = std::function<void(Loader<Assets, Args...>*, const Args&...)>;

    Loader(std::shared_ptr<Assets> assets, const FunctionType& load_function)
        : assets(assets), load_function(load_function) {}
    ~Loader() = default;

    bool done_loading() const {
        return loaded.load();
    }

    std::thread& get_thread() {
        return loading_thread;
    }

    void start_loading_thread(const Args&... args) {
        DEB_INFO("Loading some assets from separate thread...");

        loading_thread = std::thread(load_function, this, args...);
    }

    void set_done() {
        loaded.store(true);
    }

    std::shared_ptr<Assets> get() {
        return assets;
    }
private:
    std::shared_ptr<Assets> assets;
    FunctionType load_function;
    std::thread loading_thread;
    std::atomic<bool> loaded = false;
};
