#pragma once

#include <string>
#include <cstddef>

namespace sm {
    class SoundData {
    public:
        explicit SoundData(const std::string& buffer);
        ~SoundData() noexcept;

        SoundData(const SoundData&) = delete;
        SoundData& operator=(const SoundData&) = delete;
        SoundData(SoundData&&) = delete;
        SoundData& operator=(SoundData&&) = delete;

        int get_samples() const noexcept;
        int get_channels() const noexcept;
        int get_frequency() const noexcept;
        const short* get_data() const noexcept;
        std::size_t get_size() const noexcept;
        std::size_t get_bps() const noexcept;
    private:
        std::size_t compute_size() const noexcept;
        std::size_t compute_bits_per_sample() const noexcept;

        int m_samples {};  // Total number of samples
        int m_channels {};  // Usually mono or stereo
        int m_sample_rate {};  // Frequency
        short* m_data {};
        std::size_t m_size {};
        std::size_t m_bits_per_sample {};
    };
}
