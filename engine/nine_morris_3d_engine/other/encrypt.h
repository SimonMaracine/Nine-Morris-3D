#pragma once

#include <cppblowfish.h>

#include "nine_morris_3d_engine/application/platform.h"

#define ENCR(file_string) (file_string ".dat")

namespace encrypt {
    class EncryptedFile {
    public:
        explicit constexpr EncryptedFile(std::string_view file_path)
            : file_path(file_path) {}
        ~EncryptedFile() = default;

        operator std::string_view() const { return file_path; }
    private:
        std::string_view file_path;
    };

    void initialize(std::string_view key);
    cppblowfish::Buffer load_file(EncryptedFile file_path);

    constexpr EncryptedFile encr(std::string_view file_path) {
        return EncryptedFile(file_path);
    }
}
