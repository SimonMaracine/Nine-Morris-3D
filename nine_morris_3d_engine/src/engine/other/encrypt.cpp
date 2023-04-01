#include <cppblowfish/cppblowfish.h>

#include "engine/application/platform.h"
#include "engine/other/encrypt.h"
#include "engine/other/logging.h"
#include "engine/application/panic.h"

static cppblowfish::BlowfishContext _blowfish;

constexpr size_t foo = sizeof(_blowfish);

namespace encrypt {
    void initialize(std::string_view key) {
        _blowfish = cppblowfish::BlowfishContext {std::string(key) + 'S'};
    }

    cppblowfish::Buffer load_file(EncryptedFile file_path) {
        std::ifstream file {std::string(file_path), std::ios::binary};

        if (!file.is_open()) {
            LOG_DIST_CRITICAL("Could not open encrypted file `{}` for reading, exiting...", file_path);
            panic::panic();
        }

        file.seekg(0, file.end);
        const size_t length = file.tellg();
        file.seekg(0, file.beg);

        char* raw_buffer = new char[length];
        file.read(raw_buffer, length);

        cppblowfish::Buffer buffer;
        cppblowfish::Buffer cipher = cppblowfish::Buffer::from_whole_data(raw_buffer, length);

        delete[] raw_buffer;

        _blowfish.decrypt(cipher, buffer);

        return buffer;
    }
}
