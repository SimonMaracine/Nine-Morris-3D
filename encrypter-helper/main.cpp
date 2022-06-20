#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h> 

#include <cppblowfish.h>

static void open_file(const std::string& file_name, unsigned char** out, size_t* size) {
    std::ifstream file (file_name, std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Could not open file '" << file_name << "'" << std::endl;
        exit(1);
    }

    file.seekg(0, file.end);
    const size_t length = file.tellg();
    file.seekg(0, file.beg);

    char* buffer = new char[length];
    file.read(buffer, length);

    *out = reinterpret_cast<unsigned char*>(buffer);
    *size = length;
}

static void write_file(const std::string& file_name, const unsigned char* data, size_t size) {
    std::ofstream file (file_name, std::ios::binary | std::ios::trunc);

    if (!file.is_open()) {
        std::cout << "Could not open file '" << file_name << "' for writing" << std::endl;
        exit(1);
    }

    file.write(reinterpret_cast<const char*>(data), size);
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cout << "Invalid arguments" << std::endl;
        std::cout << "Usage: encrypter-helper <input_file> <output_file> <key>" << std::endl;
        exit(1);
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];
    std::string key = argv[3];

    cppblowfish::BlowfishContext blowfish (key);

    unsigned char* contents = nullptr;
    size_t contents_size = 0;
    open_file(input_file, &contents, &contents_size);

    cppblowfish::Buffer input (contents, contents_size);
    delete[] contents;
    
    cppblowfish::Buffer cipher;
    blowfish.encrypt(input, cipher);

    unsigned char* buffer = new unsigned char[cipher.size() + cppblowfish::BUFFER_OFFSET];
    cipher.write_whole_data(buffer);
    write_file(output_file, buffer, cipher.size() + cppblowfish::BUFFER_OFFSET);
    delete[] buffer;

    std::cout << "Succesfully written cipher to '" << output_file << "'" << std::endl;
}
