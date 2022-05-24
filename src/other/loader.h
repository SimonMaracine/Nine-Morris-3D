#pragma once

#include "other/logging.h"

/**
 * Objects of this class load assets in a separate thread.
 * Pass a struct with the assets and a function to load them.
 * Wrap objects of this class with smart pointers.
 */
template<typename Assets>
class Loader {
public:
    Loader(std::shared_ptr<Assets> assets, const std::function<void(Loader<Assets>*)>& load_function)
        : assets(assets), load_function(load_function) {}
    ~Loader() = default;

    bool done_loading() const {
        return loaded.load();
    }

    std::thread& get_thread() {
        return loading_thread;
    }

    void start_loading_thread() {
        DEB_INFO("Loading some assets from separate thread...");

        loading_thread = std::thread(load_function, this);
    }

    void set_done() {
        loaded.store(true);
    }

    std::shared_ptr<Assets> get() {
        return assets;
    }
private:
    std::shared_ptr<Assets> assets;
    std::function<void(Loader<Assets>*)> load_function;
    std::thread loading_thread;
    std::atomic<bool> loaded = false;
};
