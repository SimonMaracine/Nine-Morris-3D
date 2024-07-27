#include "nine_morris_3d_engine/audio/sound_data.hpp"

#include <cstdlib>
#include <cassert>

#include <stb_vorbis.h>

#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm {
    SoundData::SoundData(const std::string& buffer) {
        samples = stb_vorbis_decode_memory(
            reinterpret_cast<const unsigned char*>(buffer.data()),
            static_cast<int>(buffer.size()),
            &channels,
            &sample_rate,
            &data
        );

        if (data == nullptr) {
            SM_THROW_ERROR(ResourceError, "Could not load sound data");
        }

        size = compute_size();
        bits_per_sample = compute_bits_per_sample();

        if (bits_per_sample == 8) {
            LOG_WARNING("bits_per_sample = 8");
        }

        LOG_DEBUG("Loaded sound data");
    }

    SoundData::~SoundData() noexcept {
        assert(data != nullptr);

        std::free(data);

        LOG_DEBUG("Freed sound data");
    }

    int SoundData::get_samples() const noexcept {
        return samples;
    }

    int SoundData::get_channels() const noexcept {
        return channels;
    }

    int SoundData::get_frequency() const noexcept {
        return sample_rate;
    }

    const short* SoundData::get_data() const noexcept {
        return data;
    }

    std::size_t SoundData::get_size() const noexcept {
        return size;
    }

    std::size_t SoundData::get_bps() const noexcept {
        return bits_per_sample;
    }

    std::size_t SoundData::compute_size() const noexcept {
        return samples * channels * sizeof(short);
    }

    std::size_t SoundData::compute_bits_per_sample() const noexcept {
        return (8 * size) / (samples * channels);
    }
}
