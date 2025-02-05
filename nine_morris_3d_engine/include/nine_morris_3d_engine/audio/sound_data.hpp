#pragma once

#include <filesystem>

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
    private:
        void* m_data {};
    };
}
