#pragma once

#include <string_view>
#include <utility>

#include <cppblowfish/cppblowfish.hpp>

#include "engine/application_base/platform.hpp"

#ifdef SM_BUILD_DISTRIBUTION
    #define SM_ENCR(file_string) (file_string ".dat")
#else
    #define SM_ENCR(file_string) (file_string)
#endif

namespace sm {
    class Encrypt {
    public:
        Encrypt() = default;
        ~Encrypt() = default;

        Encrypt(const Encrypt&) = delete;
        Encrypt& operator=(const Encrypt&) = delete;
        Encrypt(Encrypt&&) = delete;
        Encrypt& operator=(Encrypt&&) = delete;

        class EncryptedFile {
        public:
            explicit constexpr EncryptedFile(std::string_view file_path)
                : file_path(file_path) {}
            ~EncryptedFile() = default;

            EncryptedFile(const EncryptedFile&) = default;
            EncryptedFile& operator=(const EncryptedFile&) = default;
            EncryptedFile(EncryptedFile&&) noexcept = default;
            EncryptedFile& operator=(EncryptedFile&&) noexcept = default;

            constexpr operator std::string_view() const { return file_path; }
        private:
            std::string_view file_path;
        };

        static std::pair<unsigned char*, size_t> load_file(EncryptedFile file_path);

#ifdef SM_BUILD_DISTRIBUTION
        static constexpr EncryptedFile encr(std::string_view file_path) {
            return EncryptedFile(file_path);
        }
#else
        static constexpr std::string_view encr(std::string_view file_path) {
            return file_path;
        }
#endif
    private:
        static void initialize(std::string_view key);

        static cppblowfish::BlowfishContext context;

        friend class Application;
    };
}
