#pragma once

#include <cppblowfish/cppblowfish.h>

#include "engine/application_base/platform.h"

#ifdef NM3D_TREAT_ENCRYPTED_FILES_AS_NORMAL_FILES
    #define ENCR(file_string) (file_string)
#else
    #define ENCR(file_string) (file_string ".dat")
#endif

namespace sm {
    class Encrypt final {
    public:
        Encrypt() = default;
        ~Encrypt() = default;

        Encrypt(const Encrypt&) = delete;
        Encrypt& operator=(const Encrypt&) = delete;
        Encrypt(Encrypt&&) = delete;
        Encrypt& operator=(Encrypt&&) = delete;

        class EncryptedFile final {
        public:
            explicit constexpr EncryptedFile(std::string_view file_path)
                : file_path(file_path) {}

            EncryptedFile(const EncryptedFile&) = default;
            EncryptedFile& operator=(const EncryptedFile&) = default;
            EncryptedFile(EncryptedFile&&) noexcept = default;
            EncryptedFile& operator=(EncryptedFile&&) noexcept = default;

            constexpr operator std::string_view() const { return file_path; }
        private:
            std::string_view file_path;
        };

        static cppblowfish::Buffer load_file(EncryptedFile file_path);

#ifdef NM3D_TREAT_ENCRYPTED_FILES_AS_NORMAL_FILES
        static constexpr std::string_view encr(std::string_view file_path) {
            return file_path;
        }
#else
        static constexpr EncryptedFile encr(std::string_view file_path) {
            return EncryptedFile {file_path};
        }
#endif
    private:
        static void initialize(std::string_view key);

        static cppblowfish::BlowfishContext context;

        friend class Application;
    };
}
