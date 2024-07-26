#pragma once

#include <string>
#include <cstddef>

namespace sm {
    class SoundData {
    public:
        explicit SoundData(const std::string& buffer);
        ~SoundData();

        SoundData(const SoundData&) = delete;
        SoundData& operator=(const SoundData&) = delete;
        SoundData(SoundData&&) = delete;
        SoundData& operator=(SoundData&&) = delete;

        int get_samples() const noexcept { return samples; }
        int get_channels() const noexcept { return channels; }
        int get_frequency() const noexcept { return sample_rate; }
        const short* get_data() const noexcept { return data; }
        std::size_t get_size() const noexcept { return size; }
        std::size_t get_bps() const noexcept { return bits_per_sample; }
    private:
        std::size_t compute_size() const noexcept;
        std::size_t compute_bits_per_sample() const noexcept;

        int samples {};  // Total number of samples
        int channels {};  // Usually mono or stereo
        int sample_rate {};  // Frequency
        short* data {};
        std::size_t size {};
        std::size_t bits_per_sample {};
    };
}
