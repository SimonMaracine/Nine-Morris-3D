#include "options.hpp"

#include <fstream>

using namespace std::string_literals;

void load_options(Options& options, const std::filesystem::path& file_path) {
    std::ifstream stream {file_path, std::ios::binary};

    if (!stream.is_open()) {
        throw OptionsFileError("Could not open options file for reading: `" + file_path.string() + "`");
    }

    try {
        cereal::BinaryInputArchive archive {stream};
        archive(options);
    } catch (const cereal::Exception& e) {
        throw OptionsFileError("Error reading from options file: "s + e.what());
    }
}

void save_options(const Options& options, const std::filesystem::path& file_path) {
    std::ofstream stream {file_path, std::ios::binary};

    if (!stream.is_open()) {
        throw OptionsFileError("Could not open options file for writing: `" + file_path.string() + "`");
    }

    try {
        cereal::BinaryOutputArchive archive {stream};
        archive(options);
    } catch (const cereal::Exception& e) {
        throw OptionsFileError("Error writing to options file: "s + e.what());
    }
}
