#include <cppblowfish/cppblowfish.h>

#include "engine/application_base/panic.h"
#include "engine/other/encrypt.h"
#include "engine/other/logging.h"

namespace sm {
    std::pair<unsigned char*, size_t> Encrypt::load_file(EncryptedFile file_path) {
        std::ifstream file {std::string(file_path), std::ios::binary};

        if (!file.is_open()) {
            LOG_DIST_CRITICAL("Could not open encrypted file `{}` for reading", file_path);
            panic();
        }

        file.seekg(0, file.end);
        const size_t length = file.tellg();
        file.seekg(0, file.beg);

        char* raw_buffer = new char[length];
        file.read(raw_buffer, length);

        cppblowfish::Buffer buffer;
        cppblowfish::Buffer cipher = cppblowfish::Buffer::from_whole_data(raw_buffer, length);

        delete[] raw_buffer;

        context.decrypt(cipher, buffer);

        unsigned char* pointer = buffer.steal();
        const size_t size = buffer.size();

        return std::make_pair(pointer, size);
    }

    void Encrypt::initialize(std::string_view key) {
        context = cppblowfish::BlowfishContext {std::string(key) + 'S'};
    }

    cppblowfish::BlowfishContext Encrypt::context {};
}
