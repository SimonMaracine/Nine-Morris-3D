#pragma once

#include <cppblowfish.h>

#include "nine_morris_3d_engine/application/platform.h"

namespace encryption {
    class EncryptedFile {
    public:
        explicit constexpr EncryptedFile(std::string_view file_path)
            : file_path(file_path) {}
        ~EncryptedFile() = default;

        operator std::string_view() const { return file_path; }
    private:
        std::string_view file_path;
    };

    void initialize();
    cppblowfish::Buffer load_file(EncryptedFile file_path);

// #if defined(PLATFORM_GAME_DEBUG)
//     std::string encr(std::string_view file_path);
// #elif defined(PLATFORM_GAME_RELEASE)
//     EncryptedFile encr(std::string_view file_path);
// #endif

    constexpr EncryptedFile encr(std::string_view file_path) {
        return EncryptedFile(file_path);
    }
}
