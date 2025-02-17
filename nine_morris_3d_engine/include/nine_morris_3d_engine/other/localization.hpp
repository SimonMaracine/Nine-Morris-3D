#pragma once

#include <filesystem>

#include "nine_morris_3d_engine/application/id.hpp"

/*
    Language localization support.

    There should be a JSON file containing all the piece of texts for the game, in the default language
    and in other optional languages (translations).

    In the game code, refer to the texts by their unique ID.

    This is different from gettext implementations, where the default language text is used as an identifier.
    But our approach has the benefit that contextual translations are not a problem anymore and that the default
    language text is not duplicated (once in the code and once in the catalog).

    If the catalog file gets deleted or corrupted, the game cannot run anymore, which seems to be a huge problem.
    This is in fact not a big deal, because all the other resources are in the same situation, thus it is consistent
    with everything in the project. The project should become open-source anyway, which almost defeats the purpose of
    any "resource protection". The resource files should be stored with write protection flags on the filesystem anyway.
*/

namespace sm {
    namespace localization {
        // Load the file containing the text and the translations
        // There must be at least one "translation", the default one, usually english
        // It should be catastrophic, if an error occurrs
        void load_catalog(const std::filesystem::path& file_path);

        // Set the preferred language
        void set_language(Id id);

        // Retrieve the piece of text using the ID and the set preferred language
        // It is a hard error, if the ID doesn't exist
        // If the language ID for that text is not present, fall back to the default language
        // Can only be called after the catalog is loaded
        const char* get_text(Id id);

        // Retrieve the piece of text that corresponds with the default language text referred to by string
        // This is very slow
        const char* get_text(const char* string);
    }

    namespace localization_literals {
        // Shortcut for getting the text
        // Should only be called after the catalog is loaded
        inline const char* operator""_L(const char* id, std::size_t) {
            return localization::get_text(Id(id));
        }
    }
}
