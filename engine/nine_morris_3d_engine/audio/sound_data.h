#pragma once

#include "nine_morris_3d_engine/other/encrypt.h"

class SoundData {
public:
	SoundData(std::string_view file_path);
	SoundData(encrypt::EncryptedFile file_path);
	~SoundData();

    SoundData(const SoundData&) = delete;
    SoundData& operator=(const SoundData&) = delete;
    SoundData(SoundData&&) = delete;
    SoundData& operator=(SoundData&&) = delete;

    short* get_data() { return data; }
    size_t get_size() { return size; }
    int get_samples() { return samples; }
    int get_channels() { return channels; }
    int get_sample_rate() { return sample_rate; }
    int get_bits_per_sample() { return bits_per_sample; }
	std::string_view get_file_path() { return file_path; }
private:
    size_t compute_size();
    size_t compute_bits_per_sample();

	short* data = nullptr;
    size_t size = 0;
    int samples = 0;  // Total number of samples
    int channels = 0;  // Usually mono or stereo
    int sample_rate = 0;  // Frequency
    size_t bits_per_sample = 0;
	std::string file_path;
};
