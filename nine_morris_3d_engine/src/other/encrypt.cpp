#include <string>
#include <utility>
#include <cstddef>
#include <memory>

#include <cppblowfish/cppblowfish.hpp>

#include "engine/application_base/panic.hpp"
#include "engine/other/encrypt.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/utilities.hpp"

namespace sm {
    std::pair<unsigned char*, std::size_t> Encrypt::load_file(const EncrFile& file_path) {
        const auto contents {Utils::read_file(file_path)};

        if (!contents) {
            LOG_DIST_CRITICAL("Could not open encrypted file `{}` for reading", file_path);
            throw ResourceLoadingError;
        }

        const auto [raw_buffer, length] {*contents};

        cppblowfish::Buffer cipher {cppblowfish::Buffer::from_whole_data(raw_buffer, length)};

        delete[] raw_buffer;

        cppblowfish::Buffer original;
        context->decrypt(cipher, original);

        unsigned char* data {original.steal()};
        const std::size_t size {original.size()};

        return std::make_pair(data, size);
    }

    void Encrypt::initialize(const std::string& key) {
        context = std::make_unique<cppblowfish::BlowfishContext>(key + 'S');
    }

    std::unique_ptr<cppblowfish::BlowfishContext> Encrypt::context {};
}
