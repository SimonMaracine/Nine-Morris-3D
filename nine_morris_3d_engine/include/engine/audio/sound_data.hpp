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

        short* get_data() { return data; }
        std::size_t get_size() { return size; }
        int get_samples() { return samples; }
        int get_channels() { return channels; }
        int get_frequency() { return sample_rate; }
        std::size_t get_bps() { return bits_per_sample; }
        const std::string& get_file_path() { return file_path; }
    private:
        std::size_t compute_size();
        std::size_t compute_bits_per_sample();

        short* data {nullptr};  // TODO reorder members
        std::size_t size {0};
        int samples {0};  // Total number of samples
        int channels {0};  // Usually mono or stereo
        int sample_rate {0};  // Frequency
        std::size_t bits_per_sample {0};
        std::string file_path;
    };
}
