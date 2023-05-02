#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>

#include <cppblowfish/cppblowfish.h>

static void open_file(const std::string& file_name, unsigned char** out, size_t* size) {
    std::ifstream file {file_name, std::ios::binary};

    if (!file.is_open()) {
        std::cout << "Could not open file `" << file_name << "` for reading" << std::endl;
        exit(1);
    }

    file.seekg(0, file.end);
    const size_t length = file.tellg();
    file.seekg(0, file.beg);

    char* buffer = new char[length];
    file.read(buffer, length);

    *out = reinterpret_cast<unsigned char*>(buffer);  // It is safe
    *size = length;
}

static void write_file(const std::string& file_name, const unsigned char* data, size_t size) {
    std::ofstream file {file_name, std::ios::binary | std::ios::trunc};

    if (!file.is_open()) {
        std::cout << "Could not open file `" << file_name << "` for writing" << std::endl;
        exit(1);
    }

    file.write(reinterpret_cast<const char*>(data), size);  // It is safe
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cout << "Invalid arguments\n";
        std::cout << "Usage: encrypter <input_file> <output_file> <key>" << std::endl;
        exit(1);
    }

    const std::string input_file = argv[1];
    const std::string output_file = argv[2];
    const std::string key = argv[3];

    cppblowfish::BlowfishContext blowfish {key};

    unsigned char* contents = nullptr;
    size_t contents_size = 0;
    open_file(input_file, &contents, &contents_size);

    cppblowfish::Buffer input {contents, contents_size};
    delete[] contents;

    cppblowfish::Buffer cipher;
    blowfish.encrypt(input, cipher);

    const size_t size = cipher.size() + cipher.padding() + cppblowfish::BUFFER_OFFSET;
    unsigned char* buffer = new unsigned char[size];
    cipher.write_whole_data(buffer);
    write_file(output_file, buffer, size);
    delete[] buffer;

    std::cout << "Successfully written cipher to `" << output_file << "`" << std::endl;
}
