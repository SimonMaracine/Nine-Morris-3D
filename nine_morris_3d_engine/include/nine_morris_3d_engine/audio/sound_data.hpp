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

        int get_samples() const { return samples; }
        int get_channels() const { return channels; }
        int get_frequency() const { return sample_rate; }
        const short* get_data() const { return data; }
        std::size_t get_size() const { return size; }
        std::size_t get_bps() const { return bits_per_sample; }
    private:
        std::size_t compute_size() const;
        std::size_t compute_bits_per_sample() const;

        int samples {};  // Total number of samples
        int channels {};  // Usually mono or stereo
        int sample_rate {};  // Frequency
        short* data {nullptr};
        std::size_t size {};
        std::size_t bits_per_sample {};
    };
}
