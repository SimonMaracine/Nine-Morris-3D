#include <stb_vorbis.h>
#include <cppblowfish/cppblowfish.h>

#include "nine_morris_3d_engine/audio/sound_data.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"
#include "nine_morris_3d_engine/other/encrypt.h"
#include "nine_morris_3d_engine/other/exit.h"

SoundData::SoundData(std::string_view file_path)
    : file_path(file_path) {
    DEB_DEBUG("Loading sound data `{}`...", file_path);

    samples = stb_vorbis_decode_filename(file_path.data(), &channels, &sample_rate, &data);

    if (data == nullptr) {
        REL_CRITICAL("Could not load sound data `{}`, exiting...", file_path);
        game_exit::exit_critical();
    }

    size = compute_size();
    bits_per_sample = compute_bits_per_sample();
}

SoundData::SoundData(encrypt::EncryptedFile file_path)
    : file_path(file_path) {
    DEB_DEBUG("Loading sound data `{}`...", file_path);

    const cppblowfish::Buffer buffer = encrypt::load_file(file_path);

    samples = stb_vorbis_decode_memory(buffer.get(), buffer.size() - buffer.padding(), &channels, &sample_rate, &data);

    if (data == nullptr) {
        REL_CRITICAL("Could not load sound data `{}`, exiting...", file_path);
        game_exit::exit_critical();
    }

    size = compute_size();
    bits_per_sample = compute_bits_per_sample();
}

SoundData::~SoundData() {
    ASSERT(data != nullptr, "No data");

    free(data);

    DEB_DEBUG("Freed sound data `{}`", file_path);
}

size_t SoundData::compute_size() {
    return samples * channels * sizeof(short);
}

size_t SoundData::compute_bits_per_sample() {
    return (8 * size) / (samples * channels);
}
