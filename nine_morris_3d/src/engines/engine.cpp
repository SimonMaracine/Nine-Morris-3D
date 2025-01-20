#include "engines/engine.hpp"

#include <cstring>

void Engine::set_info_callback(std::function<void(const Info&)>&& info_callback) {
    m_info_callback = std::move(info_callback);
}

void Engine::set_log_output(bool enable, const std::string& file_path) {
    if (enable) {
        m_log_output_stream.open(file_path, std::ios::app);
    } else {
        m_log_output_stream.close();
    }
}

std::vector<std::string> Engine::parse_message(const std::string& message) {
    std::vector<std::string> tokens;
    std::string buffer {message};

    char* token {std::strtok(buffer.data(), " \t")};

    while (token != nullptr) {
        tokens.emplace_back(token);
        token = std::strtok(nullptr, " \t");
    }

    tokens.erase(std::remove_if(tokens.begin(), tokens.end(), [](const std::string& token) {
        return token.empty();
    }), tokens.end());

    return tokens;
}

bool Engine::token_available(const std::vector<std::string>& tokens, std::size_t index) {
    return index < tokens.size();
}
