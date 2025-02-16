#include "localization.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <cassert>

static struct Localization {
    using TranslationSet = std::unordered_map<resmanager::HashedStr64, std::string>;

    std::vector<std::pair<std::string, TranslationSet>> translation_sets;
    std::size_t index {};
} g_localization;

void load_translations(std::initializer_list<Translation> translations) {
    for (const auto& translation : translations) {
        auto& translation_set {g_localization.translation_sets.emplace_back()};

        translation_set.first = translation.get_name();

        for (std::size_t i {0}; i < translation.get_size(); i++) {
            translation_set.second[translation.get_array()[i].first] = translation.get_array()[i].second;
        }
    }
}

void set_language(std::size_t index) {
    assert(index < g_localization.translation_sets.size());

    g_localization.index = index;
}

const char* get_string(resmanager::HashedStr64 identifier) {
    return g_localization.translation_sets[g_localization.index].second.at(identifier).c_str();
}
