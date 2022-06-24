#pragma once

#include <cppblowfish.h>

#include "application/platform.h"

namespace encryption {
    class EncryptedFile {
    public:
        explicit EncryptedFile(std::string_view file_path)
            : file_path(std::string(file_path)) {}
        ~EncryptedFile() = default;

        std::string_view get() const { return file_path; }
    private:
        std::string file_path;
    };

    void initialize();
    cppblowfish::Buffer load_file(const EncryptedFile& file_path);

#if defined(PLATFORM_GAME_DEBUG)
    std::string convert(std::string_view file_path);
#elif defined(PLATFORM_GAME_RELEASE)
    EncryptedFile convert(std::string_view file_path);
#endif
}
