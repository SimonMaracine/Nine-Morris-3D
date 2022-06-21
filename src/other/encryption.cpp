#include <cppblowfish.h>

#include "application/platform.h"
#include "other/encryption.h"
#include "other/logging.h"

namespace encryption {
    static const char* KEY = "data/models/board/board.obj";
    static cppblowfish::BlowfishContext blowfish;

    void initialize() {
        blowfish.initialize(std::string(KEY) + 'S');
    }

    cppblowfish::Buffer load_file(const EncryptedFile& file_path) {
        std::ifstream file {std::string(file_path.get()), std::ios::binary};

        if (!file.is_open()) {
            REL_CRITICAL("Could not open encrypted file '{}', exiting...", file_path.get());
            exit(1);
        }

        file.seekg(0, file.end);
        const size_t length = file.tellg();
        file.seekg(0, file.beg);

        char* buff = new char[length];
        file.read(buff, length);

        cppblowfish::Buffer buffer;
        cppblowfish::Buffer cipher = cppblowfish::Buffer::from_whole_data(buff, length);

        blowfish.decrypt(cipher, buffer);

        delete[] buff;

        return buffer;
    }

#if defined(NINE_MORRIS_3D_DEBUG)
    std::string convert(std::string_view file_path) {
        return std::string(file_path);
    }
#elif defined(NINE_MORRIS_3D_RELEASE)
    EncryptedFile convert(std::string_view file_path) {
        return EncryptedFile(file_path);
    }
#endif
}
