#include <string_view>
#include <utility>
#include <cstddef>
#include <fstream>
#include <string>

#include <cppblowfish/cppblowfish.hpp>

#include "engine/application_base/panic.hpp"
#include "engine/other/encrypt.hpp"
#include "engine/other/logging.hpp"

namespace sm {
    std::pair<unsigned char*, std::size_t> Encrypt::load_file(EncryptedFile file_path) {
        std::ifstream file {std::string(file_path), std::ios::binary};

        if (!file.is_open()) {
            LOG_DIST_CRITICAL("Could not open encrypted file `{}` for reading", file_path);
            panic();
        }

        file.seekg(0, file.end);
        const std::size_t length = file.tellg();
        file.seekg(0, file.beg);

        char* raw_buffer = new char[length];
        file.read(raw_buffer, length);

        cppblowfish::Buffer cipher = cppblowfish::Buffer::from_whole_data(raw_buffer, length);

        delete[] raw_buffer;

        cppblowfish::Buffer original;
        context.decrypt(cipher, original);

        unsigned char* data = original.steal();
        const std::size_t size = original.size();

        return std::make_pair(data, size);
    }

    void Encrypt::initialize(std::string_view key) {
        context = cppblowfish::BlowfishContext(std::string(key) + 'S');
    }

    cppblowfish::BlowfishContext Encrypt::context {};
}
