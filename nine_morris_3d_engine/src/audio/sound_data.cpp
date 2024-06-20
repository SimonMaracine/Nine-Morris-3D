#include "engine/audio/sound_data.hpp"

#include <cstdlib>
#include <cassert>

#include <stb_vorbis.h>

#include "engine/application_base/error.hpp"
#include "engine/application_base/logging.hpp"

namespace sm {
    static void check_bits_per_sample(std::size_t bits_per_sample, [[maybe_unused]] const std::string& file_path) {
        if (bits_per_sample == 8) {
            LOG_WARNING("bits_per_sample = 8 for sound file `{}`", file_path);
        }
    }

    SoundData::SoundData(const std::string& file_path) {
        LOG_DEBUG("Loading sound data `{}`...", file_path);

        samples = stb_vorbis_decode_filename(file_path.c_str(), &channels, &sample_rate, &data);

        if (data == nullptr) {
            LOG_DIST_CRITICAL("Could not load sound data `{}`", file_path);
            throw RuntimeError::ResourceLoading;
        }

        size = compute_size();
        bits_per_sample = compute_bits_per_sample();

        check_bits_per_sample(bits_per_sample, file_path);
    }

    // SoundData::SoundData(const EncrFile& file_path)
    //     : file_path(file_path) {
    //     LOG_DEBUG("Loading sound data `{}`...", file_path);

    //     const auto [buffer, buffer_size] {Encrypt::load_file(file_path)};

    //     samples = stb_vorbis_decode_memory(buffer, buffer_size, &channels, &sample_rate, &data);

    //     if (data == nullptr) {
    //         LOG_DIST_CRITICAL("Could not load sound data `{}`", file_path);
    //         throw ResourceLoadingError;
    //     }

    //     size = compute_size();
    //     bits_per_sample = compute_bits_per_sample();

    //     check_bits_per_sample(bits_per_sample, file_path);
    // }

    SoundData::~SoundData() {
        assert(data != nullptr);

        std::free(data);

        LOG_DEBUG("Freed sound data");
    }

    std::size_t SoundData::compute_size() const {
        return samples * channels * sizeof(short);
    }

    std::size_t SoundData::compute_bits_per_sample() const {
        return (8 * size) / (samples * channels);
    }
}
