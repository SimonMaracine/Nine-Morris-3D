#pragma once

#include <thread>
#include <atomic>
#include <unordered_map>

#include "other/asset_manager.h"
#include "other/logging.h"

using namespace model;

class Loader {
public:
    Loader(AssetManager& asset_manager)
        : asset_manager(asset_manager) {}
    ~Loader() = default;

    bool done_loading();
    std::thread& get_thread();
    void start_loading_thread(const std::unordered_map<unsigned int, AssetManager::Asset>& required);
private:
    void load();

    std::unordered_map<unsigned int, AssetManager::Asset> required;
    std::atomic<bool> loaded = false;
    std::thread loading_thread;

    AssetManager& asset_manager;
};
