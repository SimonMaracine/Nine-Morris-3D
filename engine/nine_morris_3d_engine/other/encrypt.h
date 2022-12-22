#pragma once

#include <cppblowfish/cppblowfish.h>

#include "nine_morris_3d_engine/application/platform.h"

#ifdef NM3D_TREAT_ENCRYPTED_FILES_AS_NORMAL_FILES
    #define ENCR(file_string) (file_string)
#else
    #define ENCR(file_string) (file_string ".dat")
#endif

namespace encrypt {
    class EncryptedFile {
    public:
        explicit constexpr EncryptedFile(std::string_view file_path)
            : file_path(file_path) {}
        ~EncryptedFile() = default;

        EncryptedFile(const EncryptedFile&) = default;
        EncryptedFile& operator=(const EncryptedFile&) = default;
        EncryptedFile(EncryptedFile&&) = default;
        EncryptedFile& operator=(EncryptedFile&&) = default;

        operator std::string_view() const { return file_path; }
    private:
        const std::string_view file_path;
    };

    void initialize(std::string_view key);
    cppblowfish::Buffer load_file(EncryptedFile file_path);

#ifdef NM3D_TREAT_ENCRYPTED_FILES_AS_NORMAL_FILES
    constexpr std::string_view encr(std::string_view file_path) {
        return file_path;
    }
#else
    constexpr EncryptedFile encr(std::string_view file_path) {
        return EncryptedFile {file_path};
    }
#endif
}
