#pragma once

#include <filesystem>

namespace sm {
    // Resource representing a short sound
    class SoundData {
    public:
        explicit SoundData(const std::filesystem::path& file_path);
        ~SoundData();

        SoundData(const SoundData&) = delete;
        SoundData& operator=(const SoundData&) = delete;
        SoundData(SoundData&&) = delete;
        SoundData& operator=(SoundData&&) = delete;

        // Retrieve the sound data
        void* get_data() const { return m_data; }
    private:
        void* m_data {};
    };
}
