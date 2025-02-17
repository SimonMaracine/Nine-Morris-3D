#include "nine_morris_3d_engine/other/localization.hpp"

#include <unordered_map>
#include <string>

#include <cereal/external/rapidjson/document.h>

#include "nine_morris_3d_engine/application/internal/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/other/utilities.hpp"

namespace sm {
    static struct Localization {
        using Language = Id;
        using Translation = std::string;
        using Translations = std::unordered_map<Language, Translation, Hash>;
        using Identifier = Id;
        using Catalog = std::unordered_map<Identifier, Translations, Hash>;

        Catalog catalog;  // A map of string identifiers to multiple possible translations
        Id default_language;  // As a fallback
        Id language;  // The preferred language
    } g_localization;

    struct CatalogFile {
        using Language = std::string;
        using Translation = std::string;
        using Translations = std::unordered_map<Language, Translation>;
        using Identifier = std::string;
        using Catalog = std::unordered_map<Identifier, Translations>;

        Catalog catalog;
        std::string default_language;
    };

    static void initialize_localization(CatalogFile&& catalog_file) {
        for (const auto& [identifier, translations] : catalog_file.catalog) {
            auto& translations_ {g_localization.catalog[Id(identifier)] = Localization::Translations()};

            for (const auto& [language, translation] : translations) {
                translations_[Id(language)] = translation;
            }
        }

        g_localization.default_language = Id(catalog_file.default_language);
        g_localization.language = g_localization.default_language;

        for (const auto& [_, translations] : g_localization.catalog) {
            if (translations.find(g_localization.default_language) == translations.cend()) {
                SM_THROW_ERROR(internal::ResourceError, "Catalog has missing default translation");
            }
        }
    }

    void localization::load_catalog(const std::filesystem::path& file_path) {
        CatalogFile catalog_file;

        std::string buffer;

        try {
            buffer = utils::read_file_ex(file_path);
        } catch (const internal::ResourceError& e) {
            SM_THROW_ERROR(internal::ResourceError, "Could not read catalog: {}", e.what());
        }

        rapidjson::StringStream stream {buffer.data()};
        rapidjson::Document document;
        document.ParseStream(stream);

        if (!document.IsObject()) {
            SM_THROW_ERROR(internal::ResourceError, "Invalid document");
        }

        if (!document.HasMember("default_language")) {
            SM_THROW_ERROR(internal::ResourceError, "Missing member: `default_language`");
        }

        if (!document["default_language"].IsString()) {
            SM_THROW_ERROR(internal::ResourceError, "Invalid member: `default_language`");
        }

        catalog_file.default_language = document["default_language"].GetString();

        if (!document.HasMember("catalog")) {
            SM_THROW_ERROR(internal::ResourceError, "Missing member: `catalog`");
        }

        if (!document["catalog"].IsObject()) {
            SM_THROW_ERROR(internal::ResourceError, "Invalid member: `catalog`");
        }

        const auto& catalog {document["catalog"].GetObject()};

        for (auto iter {catalog.MemberBegin()}; iter != catalog.MemberEnd(); iter++) {
            if (!iter->value.IsObject()) {
                SM_THROW_ERROR(internal::ResourceError, "Invalid member");
            }

            const auto& identifier {iter->value.GetObject()};

            catalog_file.catalog[iter->name.GetString()] = CatalogFile::Translations();

            for (auto iter2 {identifier.MemberBegin()}; iter2 != identifier.MemberEnd(); iter2++) {
                if (!iter2->value.IsString()) {
                    SM_THROW_ERROR(internal::ResourceError, "Invalid member");
                }

                const auto& translation {iter2->value.GetString()};

                catalog_file.catalog.at(iter->name.GetString())[iter2->name.GetString()] = translation;
            }
        }

        initialize_localization(std::move(catalog_file));
    }

    void localization::set_language(Id id) {
        g_localization.language = id;
    }

    static const char* get_text_translations(const Localization::Translations& translations) {
        const auto iter {translations.find(g_localization.language)};

        if (iter != translations.cend()) {
            return iter->second.c_str();
        }

        {
            const auto iter {translations.find(g_localization.default_language)};

            if (iter == translations.cend()) {
                SM_THROW_ERROR(internal::ResourceError, "Could not find text in catalog");
            }

            return iter->second.c_str();
        }
    }

    const char* localization::get_text(Id id) {
        const auto iter {g_localization.catalog.find(id)};

        if (iter == g_localization.catalog.cend()) {
            SM_THROW_ERROR(internal::ResourceError, "Could not find text in catalog");
        }

        return get_text_translations(iter->second);
    }

    const char* localization::get_text(const char* string) {
        for (const auto& [_, translations] : g_localization.catalog) {
            const auto iter {translations.find(g_localization.default_language)};

            if (iter == translations.cend()) {
                continue;
            }

            // Compare the full string to find the one we are looking for
            if (iter->second != string) {
                continue;
            }

            return get_text_translations(translations);
        }

        SM_THROW_ERROR(internal::ResourceError, "Could not find text in catalog");
    }
}
