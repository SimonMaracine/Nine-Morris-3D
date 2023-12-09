#pragma once

#include <string>
#include <cstddef>

#include "engine/other/encrypt.hpp"

namespace sm {
    class SoundData {
    public:
        SoundData(const std::string& file_path);
        SoundData(const EncrFile& file_path);
        ~SoundData();

        SoundData(const SoundData&) = delete;
        SoundData& operator=(const SoundData&) = delete;
        SoundData(SoundData&&) = delete;
        SoundData& operator=(SoundData&&) = delete;

        const short* get_data() const { return data; }
        std::size_t get_size() const { return size; }
        int get_samples() const { return samples; }
        int get_channels() const { return channels; }
        int get_frequency() const { return sample_rate; }
        std::size_t get_bps() const { return bits_per_sample; }
        const std::string& get_file_path() const { return file_path; }
    private:
        std::size_t compute_size() const;
        std::size_t compute_bits_per_sample() const;

        short* data {nullptr};
        std::size_t size {0};
        int samples {0};  // Total number of samples
        int channels {0};  // Usually mono or stereo
        int sample_rate {0};  // Frequency
        std::size_t bits_per_sample {0};
        std::string file_path;
    };
}
