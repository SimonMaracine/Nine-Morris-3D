#pragma once

// #include <string>
#include <filesystem>
// #include <cstddef>

namespace sm {
    class SoundData {
    public:
        explicit SoundData(const std::filesystem::path& file_path);
        ~SoundData();

        SoundData(const SoundData&) = delete;
        SoundData& operator=(const SoundData&) = delete;
        SoundData(SoundData&&) = delete;
        SoundData& operator=(SoundData&&) = delete;

        void* get_data() const { return m_data; }

        // int get_samples() const;
        // int get_channels() const;
        // int get_frequency() const;
        // const short* get_data() const;
        // std::size_t get_size() const;
        // std::size_t get_bps() const;
    private:
        // std::size_t compute_size() const;
        // std::size_t compute_bits_per_sample() const;

        // int m_samples {};  // Total number of samples
        // int m_channels {};  // Usually mono or stereo
        // int m_sample_rate {};  // Frequency
        // short* m_data {};
        // std::size_t m_size {};
        // std::size_t m_bits_per_sample {};

        void* m_data {};
    };
}
