#include "nine_morris_3d_engine/audio/sound_data.hpp"

#include <cstdlib>
#include <cassert>

#include <stb_vorbis.h>

#include "nine_morris_3d_engine/application/internal/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm {
    SoundData::SoundData(const std::string& buffer) {
        m_samples = stb_vorbis_decode_memory(
            reinterpret_cast<const unsigned char*>(buffer.data()),
            static_cast<int>(buffer.size()),
            &m_channels,
            &m_sample_rate,
            &m_data
        );

        if (m_data == nullptr) {
            SM_THROW_ERROR(internal::ResourceError, "Could not load sound data");
        }

        m_size = compute_size();
        m_bits_per_sample = compute_bits_per_sample();

        if (m_bits_per_sample == 8) {
            LOG_WARNING("bits_per_sample = 8");
        }

        LOG_DEBUG("Loaded sound data");
    }

    SoundData::~SoundData() {
        assert(m_data != nullptr);

        std::free(m_data);

        LOG_DEBUG("Freed sound data");
    }

    int SoundData::get_samples() const {
        return m_samples;
    }

    int SoundData::get_channels() const {
        return m_channels;
    }

    int SoundData::get_frequency() const {
        return m_sample_rate;
    }

    const short* SoundData::get_data() const {
        return m_data;
    }

    std::size_t SoundData::get_size() const {
        return m_size;
    }

    std::size_t SoundData::get_bps() const {
        return m_bits_per_sample;
    }

    std::size_t SoundData::compute_size() const {
        return m_samples * m_channels * sizeof(short);
    }

    std::size_t SoundData::compute_bits_per_sample() const {
        return (8 * m_size) / (m_samples * m_channels);
    }
}
