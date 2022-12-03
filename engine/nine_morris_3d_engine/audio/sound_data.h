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

	std::string_view get_file_path() { return file_path; }
private:
	short* data = nullptr;
    int samples = 0;
    int channels = 0;
    int sample_rate = 0;
	std::string file_path;
};
