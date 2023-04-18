#include <stb_vorbis.h>
#include <cppblowfish/cppblowfish.h>

#include "engine/audio/sound_data.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"
#include "engine/other/encrypt.h"
#include "engine/application_base/panic.h"

static void check_bits_per_sample(size_t bits_per_sample, std::string_view file_path) {
    if (bits_per_sample == 8) {
        LOG_WARNING("bits_per_sample = 8 for sound file `{}`", file_path);
    }

    static_cast<void>(file_path);
}

SoundData::SoundData(std::string_view file_path)
    : file_path(file_path) {
    LOG_DEBUG("Loading sound data `{}`...", file_path);

    samples = stb_vorbis_decode_filename(file_path.data(), &channels, &sample_rate, &data);

    if (data == nullptr) {
        LOG_DIST_CRITICAL("Could not load sound data `{}`, exiting...", file_path);
        panic::panic();
    }

    size = compute_size();
    bits_per_sample = compute_bits_per_sample();

    check_bits_per_sample(bits_per_sample, file_path);
}

SoundData::SoundData(Encrypt::EncryptedFile file_path)
    : file_path(file_path) {
    LOG_DEBUG("Loading sound data `{}`...", file_path);

    const cppblowfish::Buffer buffer = Encrypt::load_file(file_path);

    samples = stb_vorbis_decode_memory(buffer.get(), buffer.size() - buffer.padding(), &channels, &sample_rate, &data);

    if (data == nullptr) {
        LOG_DIST_CRITICAL("Could not load sound data `{}`, exiting...", file_path);
        panic::panic();
    }

    size = compute_size();
    bits_per_sample = compute_bits_per_sample();

    check_bits_per_sample(bits_per_sample, file_path);
}

SoundData::~SoundData() {
    ASSERT(data != nullptr, "No data");

    free(data);

    LOG_DEBUG("Freed sound data `{}`", file_path);
}

size_t SoundData::compute_size() {
    return samples * channels * sizeof(short);
}

size_t SoundData::compute_bits_per_sample() {
    return (8 * size) / (samples * channels);
}
